#include "subg.h"
#include <iostream>
#include <memory>
#include <algorithm>
#include <cassert>
#include <set>
#include <fstream>

size_t Node::m_default_name_idx = 0;

Node::Node(const std::any& data, const std::string& name): m_data(data), m_name(name) {}

Node::Node(const std::any& data): Node(data, "node" + std::to_string(Node::m_default_name_idx++)){}

std::string Node::name() const { 
    return m_name;
}

const std::any& Node::data() const { 
    return m_data;
}

DirectedGraph::DirectedGraph(): DirectedGraph(std::string{"G"}) {}

DirectedGraph::DirectedGraph(const std::string& name): m_name(name) {}

DirectedGraph::DirectedGraph(const std::string& name,
        const std::vector<Node*>& nodes): m_name(name) {
    for(Node* node: nodes) {
        addNode(node);
    }
}

bool DirectedGraph::hasNode(Node* node) const {
    return m_adj.find(node) != m_adj.end();
}

std::optional<Node*> DirectedGraph::nodeByName(const std::string& name) const {
    for (const auto& [node, adj]: m_adj ) {
        if (node->name() == name) {
            return {node};
        }
    }
    return {};
}


bool DirectedGraph::addNode(Node* node) {
    if (m_adj.find(node) != m_adj.end()) {
        return false;
    }
    m_adj[node] = {{"inbound", {}}, {"outbound", {}}};
    return true;
}

bool DirectedGraph::addEdge(Node* from, Node* to, const std::string& label) {
    const auto& out_nodes = m_adj[from]["outbound"];
    auto to_iter = std::find_if(out_nodes.cbegin(), out_nodes.cend(), [=](const auto& p) { return p.first == to; });
    if (to_iter != out_nodes.end()) {
        return false; // no multi-edges allowed
    }
    m_adj[from]["outbound"].push_back({to, label});
    m_adj[to]["inbound"].push_back({from, label});
    m_adj[from]["inbound"]; // create missing keys
    m_adj[to]["outbound"];
    return true;
}

bool DirectedGraph::addEdge(Node* from, Node* to) {
    return addEdge(from, to, std::string{});
}

bool DirectedGraph::removeEdge(Node* from, Node* to) {
    auto& out_nodes = m_adj[from]["outbound"];
    auto to_iter = std::find_if(out_nodes.begin(), out_nodes.end(), [=](const auto& p) { return p.first == to; });
    if (to_iter == out_nodes.end()) {
        return false; // no edge present
    }
    out_nodes.erase(to_iter);

    auto& in_nodes = m_adj[to]["inbound"];
    auto from_iter = std::find_if(in_nodes.begin(), in_nodes.end(), [=](const auto& p) { return p.first == from; });
    in_nodes.erase(from_iter);
    
    return true;
}

std::vector<Node*> DirectedGraph::nodes() const {
    std::vector<Node*> nodes;
    for (const auto& [node, adj]: m_adj) {
        nodes.push_back(node);
    }
    return nodes;
}

std::vector<Node*> DirectedGraph::top() const {
    std::vector<Node*> nodes;
    for (const auto& [node, adj]: m_adj) {
        if (adj.at("inbound").empty()) {
            nodes.push_back(node);
        }
    }
    return nodes;
}

std::vector<Node*> DirectedGraph::bottom() const {
    std::vector<Node*> nodes;
    for (const auto& [node, adj]: m_adj) {
        if (adj.at("outbound").empty()) {
            nodes.push_back(node);
        }
    }
    return nodes;
}

std::vector<DirectedEdge> DirectedGraph::edges() const {
    std::vector<DirectedEdge> edges;
    for (const auto& [node, adj]: m_adj) {
        for (const auto& p: adj.at("outbound")) {
            edges.push_back({node, p.first});
        }
    }
    return edges;
}

std::vector<Node*> DirectedGraph::inbound(Node* node) const {
    std::vector<Node*> nodes;
    for (const auto& p: m_adj.at(node).at("inbound")) {
        nodes.push_back(p.first);
    }
    return nodes;
}

std::vector<Node*> DirectedGraph::outbound(Node* node) const {
    std::vector<Node*> nodes;
    for (const auto& p: m_adj.at(node).at("outbound")) {
        nodes.push_back(p.first);
    }
    return nodes;
}

std::ostream& operator<<(std::ostream& os, const Node& node) {
    os << node.name();
    return os;
}

std::ostream& operator<<(std::ostream& os, const DirectedEdge& edge) {
    os << edge.from->name() << "->" << edge.to->name();
    return os;
}

void printNodes(const std::vector<Node*>& nodes) {
    for (Node* n: nodes) {
        std::cout << *n << ' ';
    }
    std::cout << '\n';
}

void printEdges(const DirectedGraph& g) {
    for (const auto& e: g.edges()) {
        std::cout << e << ' ';
    }
    std::cout << '\n';
}

SubgraphExtractor::SubgraphExtractor(const DirectedGraph& graph): m_graph(graph) {}

void SubgraphExtractor::dfs(Node* node, std::unordered_set<Node*>& visited, Direction dir) {
    visited.insert(node);
    std::cout << node->name() << ' ';
    std::vector<Node*> adj;
    if (dir == Direction::in) {
        adj = m_graph.inbound(node);
    }
    else if (dir == Direction::out) {
        adj = m_graph.outbound(node);
    }
    else {
        adj = m_graph.inbound(node);
        for (Node* out_node: m_graph.outbound(node)) {
            adj.push_back(out_node);
        }
    }
    for (Node* adj_node: adj) {
        if (visited.find(adj_node) == visited.end()) {
            dfs(adj_node, visited, dir);
        }
    }
}

void SubgraphExtractor::ensureNodesExist(const std::vector<Node*>& inputs, const std::vector<Node*>& outputs) {
    std::vector<Node*> boundary_nodes(inputs.begin(), inputs.end());
    boundary_nodes.insert(boundary_nodes.end(), outputs.begin(), outputs.end());
    for (Node* node: boundary_nodes) {
        if (!m_graph.hasNode(node)) {
            throw std::runtime_error("Node: " + node->name() + " not present in graph");
        }
    }
}
                
DirectedGraph SubgraphExtractor::extract(const std::vector<Node*>& inputs, const std::vector<Node*>& outputs) {
    // TODO handle case where inputs and outputs overlap
    // TODO handle case of invalid inputs, outputs - outputs not reachable from inputs
    ensureNodesExist(inputs, outputs);
//    std::vector<Node*> top_layer;
//    for (Node* node: inputs) {
//        for (Node* out_node: m_graph.outbound(node)) {
//            top_layer.push_back(out_node);
//        }
//    }

    std::unordered_set<Node*> outward_subgraph_nodes(outputs.begin(), outputs.end());
    for (Node* node: inputs) {
        dfs(node, outward_subgraph_nodes, Direction::out);
    }

//    std::vector<Node*> bottom_layer;
//    for (Node* node: outputs) {
//        for (Node* in_node: m_graph.inbound(node)) {
//            bottom_layer.push_back(in_node);
//        }
//    }

    std::unordered_set<Node*> inward_subgraph_nodes(inputs.begin(), inputs.end());
    for (Node* node: outputs) {
        dfs(node, inward_subgraph_nodes, Direction::in);
    }

    inward_subgraph_nodes.merge(outward_subgraph_nodes);
    std::unordered_set<Node*>& subgraph_nodes = inward_subgraph_nodes;

    return cloneGraph(subgraph_nodes);
}

DirectedGraph SubgraphExtractor::cloneGraph(const std::unordered_set<Node*>& nodes) const {
    DirectedGraph graph_clone("subgraph");
    std::unordered_map<Node*, Node*> clone_map;
    for (Node* node: nodes) {
        if (clone_map.find(node) == clone_map.end()) {
            clone_map[node] = new Node(node->data(), node->name());
        }
        Node* node_clone = clone_map[node];
        for (Node* out_node: m_graph.outbound(node)) {
            if (nodes.find(out_node) == nodes.end()) {
                continue;
            }
            if (clone_map.find(out_node) == clone_map.end()) {
                clone_map[out_node] = new Node(out_node->data(), out_node->name());
            }
            Node* out_node_clone = clone_map[out_node];
            graph_clone.addEdge(node_clone, out_node_clone);
        }
    }
    return graph_clone;
}
