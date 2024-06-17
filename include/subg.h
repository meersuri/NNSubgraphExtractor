#ifndef SUBG_H
#define SUBG_H

#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <any>
#include <filesystem>
#include <optional>

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

struct DirectedEdge {
    Node* from;
    Node* to;
    std::string label;
};

class DirectedGraph {
    std::string m_name;
    using MapStrVecNodeLabelPair = std::unordered_map<std::string, std::vector<std::pair<Node*, std::string>>>;
    std::unordered_map<Node*, MapStrVecNodeLabelPair> m_adj;
    public:
        DirectedGraph();
        DirectedGraph(const std::string& name);
        DirectedGraph(const std::string& name, const std::vector<Node*>& nodes);
        bool hasNode(Node* node) const;
        std::optional<Node*> nodeByName(const std::string& name) const;
        bool addNode(Node* node);
        bool addEdge(Node* from, Node* to);
        bool addEdge(Node* from, Node* to, const std::string& label);
        bool removeEdge(Node* from, Node* to);
        std::vector<Node*> nodes() const;
        std::vector<Node*> top() const;
        std::vector<Node*> bottom() const;
        std::vector<DirectedEdge> edges() const;
        std::vector<Node*> inbound(Node* node) const ;
        std::vector<Node*> outbound(Node* node) const ;
};

enum class Direction {
    bi,
    in,
    out
};

class SubgraphExtractor {
    public:
        SubgraphExtractor(const DirectedGraph& graph);
        DirectedGraph extract(const std::vector<Node*>& inputs, const std::vector<Node*>& outputs);
    private:
        void dfs(Node* node, std::unordered_set<Node*>& visited, Direction d);
        void ensureNodesExist(const std::vector<Node*>& inputs, const std::vector<Node*>& outputs);
        DirectedGraph cloneGraph(const std::unordered_set<Node*>& nodes) const;
        DirectedGraph m_graph;
};

#endif


