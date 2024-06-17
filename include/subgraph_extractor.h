#ifndef SUBGRAPH_EXTRACTOR_H
#define SUBGRAPH_EXTRACTOR_H

#include "graph.h"
#include "onnx.proto3.pb.h"
#include <filesystem>

// NNModelSubgraphExtractor ex("/path/to/model.ext");
// ex.extract({"i0"}, {"o1", "o2"}, "/path/to/output_model.ext");

class NNModel {
    public:
        NNModel() = default;
        NNModel(const DirectedGraph& graph): m_graph(graph) {};
        NNModel(DirectedGraph&& graph): m_graph(std::move(graph)) {};
        virtual DirectedGraph& graph() { return m_graph; }
        virtual void save(std::filesystem::path fpath) = 0;
        virtual ~NNModel() = default;
    protected:
        DirectedGraph m_graph;
};

class OnnxModel: public NNModel {
    public:
        OnnxModel(std::filesystem::path fpath);
        OnnxModel(const onnx::ModelProto& model_proto);
        onnx::ValueInfoProto getValueInfo(const std::string& vinfo_name);
        onnx::ModelProto* makeModel(const std::vector<onnx::NodeProto>& nodes,
                const std::vector<onnx::ValueInfoProto>& values,
                const std::vector<onnx::ValueInfoProto>& inputs,
                const std::vector<onnx::ValueInfoProto>& outputs,
                const std::vector<onnx::TensorProto>& inits);
        void save(std::filesystem::path fpath) override;
    private:
        DirectedGraph convert(std::filesystem::path fpath);
        DirectedGraph convert(const onnx::ModelProto& model_proto);
        onnx::ModelProto load(std::filesystem::path fpath);
        onnx::ModelProto m_model_proto;
        std::unordered_map<std::string, onnx::ValueInfoProto> m_vinfo_map;
        
};

class NNModelSubgraphExtractor {
    public:
        NNModelSubgraphExtractor(NNModel* model): m_sgex(SubgraphExtractor(model->graph())) {}
//        NNModelSubgraphExtractor(std::filesystem::path model_path): NNModelSubgraphExtractor(load(model_path)){}
        virtual NNModel* extract(const std::vector<std::string>& inputs, const std::vector<std::string>& outputs) = 0;
        virtual ~NNModelSubgraphExtractor() = default;
    protected:
        SubgraphExtractor m_sgex;
};

class OnnxSubgraphExtractor: public NNModelSubgraphExtractor {
    public:
        OnnxSubgraphExtractor(OnnxModel* model): m_model(model), NNModelSubgraphExtractor(model) {}
        NNModel* extract(const std::vector<std::string>& inputs, const std::vector<std::string>& outputs) override;
    private:
        OnnxModel* m_model;
};
        
#endif
