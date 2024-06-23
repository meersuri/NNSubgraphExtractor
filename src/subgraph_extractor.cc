#include <vector>
#include <iostream>
#include <filesystem>
#include <unordered_map>
#include <map>
#include <fstream>

#include "subgraph_extractor.h"
#include "onnx.proto3.pb.h"

struct HashNodeProto {
    size_t operator()(const onnx::NodeProto& proto) const {
        return std::hash<std::string>()(proto.name() + proto.op_type());
    }
};

struct NodeProtoEqual {
    bool operator()(const onnx::NodeProto& a, const onnx::NodeProto& b) const {
        return a.name() == b.name() && a.op_type() == b.op_type();
    }
};

OnnxModel::OnnxModel(std::filesystem::path fpath): NNModel() {
    m_graph = convert(fpath);
}

OnnxModel::OnnxModel(std::unique_ptr<onnx::ModelProto> model_proto): NNModel() {
    m_graph = convert(std::move(model_proto));
}

std::unique_ptr<DirectedGraph> OnnxModel::convert(std::filesystem::path fpath) {
    return convert(load(fpath));
}

std::unique_ptr<DirectedGraph> OnnxModel::convert(std::unique_ptr<onnx::ModelProto> model_proto) {
    m_model_proto = std::move(model_proto);
    auto& graph = m_model_proto->graph();
    for (auto& vinfo: graph.value_info()) {
        m_vinfo_map[vinfo.name()] = vinfo;
    }

    for (auto& tensor_proto: graph.initializer()) {
        m_init_map[tensor_proto.name()] = tensor_proto;
    }

    std::unordered_map<std::string, std::vector<onnx::NodeProto>> vinfo_consumers;
    for (auto& node_proto: graph.node()) {
        if (node_proto.op_type() == "Constant") {
            m_const_map[node_proto.name()] = node_proto;
        }
        for (auto& in_vinfo_name: node_proto.input()) {
            vinfo_consumers[in_vinfo_name].push_back(node_proto);
        }
    }
    auto converted = std::make_unique<DirectedGraph>();
    std::unordered_map<onnx::NodeProto, PtrNode, HashNodeProto, NodeProtoEqual> clone_map;
    for (auto& node_proto: graph.node()) {
        if (clone_map.find(node_proto) == clone_map.end()) {
            clone_map[node_proto] = std::make_shared<Node>(node_proto, node_proto.name());
        }
        converted->addNode(clone_map[node_proto]);
        for (auto& out_vinfo_name: node_proto.output()) {
            for (auto& out_node_proto: vinfo_consumers[out_vinfo_name]) {
                if (clone_map.find(out_node_proto) == clone_map.end()) {
                    clone_map[out_node_proto] = std::make_shared<Node>(out_node_proto, out_node_proto.name());
                }
                converted->addEdge(clone_map[node_proto], clone_map[out_node_proto]);
            }
        }
    }
    assert(graph.node().size() ==  converted->nodes().size());
    return converted;
}

onnx::ValueInfoProto OnnxModel::getValueInfo(const std::string& vinfo_name) {
    return m_vinfo_map.at(vinfo_name);
}

bool OnnxModel::isConst(const std::string& node_name) const {
    return m_const_map.find(node_name) != m_const_map.end();
}

onnx::TensorProto OnnxModel::getTensorProto(const std::string& tensor_name) {
    return m_init_map.at(tensor_name);
}

std::unique_ptr<onnx::ModelProto> OnnxModel::load(std::filesystem::path fpath) {
    std::ifstream ifs(fpath.c_str(), std::ios::ate | std::ios::binary);
    if (!ifs.is_open()) {
        throw std::runtime_error("failed to open file");
    }
    auto size = ifs.tellg();
    std::string buf(size, '\0');
    ifs.seekg(0);
    ifs.read(buf.data(), size);
    auto model = std::make_unique<onnx::ModelProto>();
    model->ParseFromString(buf);
    return model;
}

std::unique_ptr<NNModel> OnnxSubgraphExtractor::extract(const std::vector<std::string>& inputs, const std::vector<std::string>& outputs) {
    std::vector<PtrNode> input_nodes;
    if (inputs.empty()) {
        auto top_nodes = m_model->graph()->top();
        for (auto& node: top_nodes) {
            if (m_model->isConst(node->name())) {
                continue;
            }
            input_nodes.push_back(node);
        }
    }
    std::vector<PtrNode> output_nodes;
    if (outputs.empty()) {
        output_nodes = m_model->graph()->bottom();
    }
    for (const auto& name: inputs) {
        auto node_opt = m_model->graph()->nodeByName(name);
        if (!node_opt.has_value()) {
            throw std::runtime_error("Couldn't find node with name: " + name);
        }
        input_nodes.push_back(node_opt.value());
    }
    for (const auto& name: outputs) {
        auto node_opt = m_model->graph()->nodeByName(name);
        if (!node_opt.has_value()) {
            throw std::runtime_error("Couldn't find node with name: " + name);
        }
        output_nodes.push_back(node_opt.value());
    }
    auto subgraph = m_sgex.extract(input_nodes, output_nodes);
    std::cout << '\n';
    for (const auto& e: subgraph->edges()) {
        std::cout << e.from->name() << "->" << e.to->name() << ' '; 
    }
    std::cout << '\n';

    std::vector<onnx::NodeProto> node_protos;
    for (auto node: subgraph->nodes()) {
        node_protos.push_back(std::any_cast<onnx::NodeProto>(node->data()));
    }

    std::vector<onnx::ValueInfoProto> value_info_protos, input_protos, output_protos;
    std::unordered_set<std::string> done_vinfo;
    for (auto& node: node_protos) {
        for (auto& vinfo_name: node.input()) {
            onnx::ValueInfoProto vinfo_proto;
            try {
                vinfo_proto = m_model->getValueInfo(vinfo_name);
            }
            catch (const std::out_of_range& e) {
                continue;
            }
            if (done_vinfo.find(vinfo_name) == done_vinfo.end()) {
                value_info_protos.push_back(vinfo_proto);
                done_vinfo.insert(vinfo_name);
            }
        }
        for (auto& vinfo_name: node.output()) {
            onnx::ValueInfoProto vinfo_proto;
            try {
                vinfo_proto = m_model->getValueInfo(vinfo_name);
            }
            catch (const std::out_of_range& e) {
                continue;
            }
            if (done_vinfo.find(vinfo_name) == done_vinfo.end()) {
                value_info_protos.push_back(vinfo_proto);
                done_vinfo.insert(vinfo_name);
            }
        }
    }

    for (auto node: subgraph->top()) {
        auto node_proto = std::any_cast<onnx::NodeProto>(node->data());
        for (auto& vinfo_name: node_proto.input()) {
            onnx::ValueInfoProto vinfo_proto;
            try {
                vinfo_proto = m_model->getValueInfo(vinfo_name);
            }
            catch (const std::out_of_range& e) {
                vinfo_proto.set_name(vinfo_name);
            }
            input_protos.push_back(vinfo_proto);
        }
    }

    for (auto node: subgraph->bottom()) {
        auto node_proto = std::any_cast<onnx::NodeProto>(node->data());
        for (auto& vinfo_name: node_proto.output()) {
            onnx::ValueInfoProto vinfo_proto;
            try {
                vinfo_proto = m_model->getValueInfo(vinfo_name);
            }
            catch (const std::out_of_range& e) {
                vinfo_proto.set_name(vinfo_name);
            }
            output_protos.push_back(vinfo_proto);
        }
    }

    std::vector<onnx::TensorProto> inits;
    for (auto& node: node_protos) {
        for (auto& vinfo_name: node.input()) {
            onnx::TensorProto tensor_proto;
            try {
                tensor_proto = m_model->getTensorProto(vinfo_name);
            }
            catch (const std::out_of_range& e) {
                continue;
            }
            inits.push_back(tensor_proto);
        }
    }

    auto new_model = m_model->makeModel(node_protos, value_info_protos, input_protos, output_protos, inits);
    return std::make_unique<OnnxModel>(std::move(new_model));
}

std::unique_ptr<onnx::ModelProto> OnnxModel::makeModel(const std::vector<onnx::NodeProto>& nodes,
        const std::vector<onnx::ValueInfoProto>& values,
        const std::vector<onnx::ValueInfoProto>& inputs,
        const std::vector<onnx::ValueInfoProto>& outputs,
        const std::vector<onnx::TensorProto>& inits) {
    auto model_proto = std::make_unique<onnx::ModelProto>();
    model_proto->set_producer_name("ME");
    onnx::GraphProto* graph_proto = model_proto->mutable_graph();
    graph_proto->set_name("MY GRAPH");
    for (const auto& node: nodes) {
        onnx::NodeProto* node_proto = graph_proto->add_node();
        node_proto->CopyFrom(node);
    }
    for (const auto& vinfo: values) {
        onnx::ValueInfoProto* vinfo_proto = graph_proto->add_value_info();
        vinfo_proto->CopyFrom(vinfo);
    }
    for (const auto& vinfo: inputs) {
        onnx::ValueInfoProto* vinfo_proto = graph_proto->add_input();
        vinfo_proto->CopyFrom(vinfo);
    }
    for (const auto& vinfo: outputs) {
        onnx::ValueInfoProto* vinfo_proto = graph_proto->add_output();
        vinfo_proto->CopyFrom(vinfo);
    }
    for (const auto& tensor: inits) {
        onnx::TensorProto* tensor_proto = graph_proto->add_initializer();
        tensor_proto->CopyFrom(tensor);
    }
    return model_proto;
}

void OnnxModel::save(std::filesystem::path fpath) {
    std::string serialized; 
    m_model_proto->SerializeToString(&serialized);
    std::ofstream ofs(fpath.c_str(), std::ios::binary | std::ios::out);
    if (!ofs.is_open()) {
        throw std::runtime_error("Failed to open output file");
    }
    ofs.write(serialized.c_str(), serialized.size());
    ofs.close();
}
