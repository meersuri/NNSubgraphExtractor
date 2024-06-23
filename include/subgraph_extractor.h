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
        NNModel(std::unique_ptr<DirectedGraph> graph): m_graph(std::move(graph)) {};
        virtual DirectedGraph* graph() { return m_graph.get(); }
        virtual void save(std::filesystem::path fpath) = 0;
        virtual ~NNModel() = default;
    protected:
        std::unique_ptr<DirectedGraph> m_graph;
};

class OnnxModel: public NNModel {
    public:
        OnnxModel(std::filesystem::path fpath);
        OnnxModel(std::unique_ptr<onnx::ModelProto> model_proto);
        onnx::ValueInfoProto getValueInfo(const std::string& vinfo_name);
        onnx::TensorProto getTensorProto(const std::string& tensor_name);
        bool isConst(const std::string& node_name) const;
        std::unique_ptr<onnx::ModelProto> makeModel(const std::vector<onnx::NodeProto>& nodes,
                const std::vector<onnx::ValueInfoProto>& values,
                const std::vector<onnx::ValueInfoProto>& inputs,
                const std::vector<onnx::ValueInfoProto>& outputs,
                const std::vector<onnx::TensorProto>& inits);
        void save(std::filesystem::path fpath) override;
    private:
        std::unique_ptr<DirectedGraph> convert(std::filesystem::path fpath);
        std::unique_ptr<DirectedGraph> convert(std::unique_ptr<onnx::ModelProto> model_proto);
        std::unique_ptr<onnx::ModelProto> load(std::filesystem::path fpath);
        std::unique_ptr<onnx::ModelProto> m_model_proto;
        std::unordered_map<std::string, onnx::ValueInfoProto> m_vinfo_map;
        std::unordered_map<std::string, onnx::TensorProto> m_init_map;
        std::unordered_map<std::string, onnx::NodeProto> m_const_map;
        
};

class NNModelSubgraphExtractor {
    public:
        NNModelSubgraphExtractor(std::shared_ptr<NNModel> model): m_sgex(SubgraphExtractor(model->graph())) {}
//        NNModelSubgraphExtractor(std::filesystem::path model_path): NNModelSubgraphExtractor(load(model_path)){}
        virtual std::unique_ptr<NNModel> extract(const std::vector<std::string>& inputs, const std::vector<std::string>& outputs) = 0;
        virtual ~NNModelSubgraphExtractor() = default;
    protected:
        SubgraphExtractor m_sgex;
};

class OnnxSubgraphExtractor: public NNModelSubgraphExtractor {
    public:
        OnnxSubgraphExtractor(std::shared_ptr<OnnxModel> model): m_model(model), NNModelSubgraphExtractor(model) {}
        std::unique_ptr<NNModel> extract(const std::vector<std::string>& inputs, const std::vector<std::string>& outputs) override;
    private:
        std::shared_ptr<OnnxModel> m_model;
};
        
#endif
