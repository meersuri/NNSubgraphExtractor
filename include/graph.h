#ifndef GRAPH_H
#define GRAPH_H

#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <any>
#include <filesystem>
#include <optional>
#include <memory>
#include <iostream>

class Node {
    std::string m_name;
    std::any m_data; 
    static size_t m_default_name_idx;
    public:
        Node(const std::any& data, const std::string& name);
        Node(const std::any& data);
        Node(const Node& other) = default;
        Node(Node&& other) = default;
        std::string name() const;
        const std::any& data() const;
};

using PtrNode = std::shared_ptr<Node>;

struct DirectedEdge {
    PtrNode from;
    PtrNode to;
    std::string label;
};

class DirectedGraph {
    std::string m_name;
    using MapStrVecNodeLabelPair = std::unordered_map<std::string, std::vector<std::pair<PtrNode, std::string>>>;
    std::unordered_map<PtrNode, MapStrVecNodeLabelPair> m_adj;
    public:
        DirectedGraph();
        DirectedGraph(const std::string& name);
        DirectedGraph(const std::string& name, const std::vector<PtrNode>& nodes);
        bool hasNode(PtrNode node) const;
        std::optional<PtrNode> nodeByName(const std::string& name) const;
        bool addNode(PtrNode node);
        bool addEdge(PtrNode from, PtrNode to);
        bool addEdge(PtrNode from, PtrNode to, const std::string& label);
        bool removeEdge(PtrNode from, PtrNode to);
        std::vector<PtrNode> nodes() const;
        std::vector<PtrNode> top() const;
        std::vector<PtrNode> bottom() const;
        std::vector<DirectedEdge> edges() const;
        std::vector<PtrNode> inbound(PtrNode node) const ;
        std::vector<PtrNode> outbound(PtrNode node) const ;
};

enum class Direction {
    bi,
    in,
    out
};

class SubgraphExtractor {
    public:
        SubgraphExtractor(DirectedGraph* graph);
        std::unique_ptr<DirectedGraph> extract(const std::vector<PtrNode>& inputs, const std::vector<PtrNode>& outputs);
    private:
        void dfs(PtrNode node, std::unordered_set<PtrNode>& visited, Direction d);
        void ensureNodesExist(const std::vector<PtrNode>& inputs, const std::vector<PtrNode>& outputs);
        std::unique_ptr<DirectedGraph> cloneGraph(const std::unordered_set<PtrNode>& nodes) const;
        DirectedGraph* m_graph;
};

#endif
