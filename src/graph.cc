#include <iostream>
#include <memory>
#include <algorithm>
#include <cassert>
#include <set>
#include <fstream>

#include "graph.h"

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
        const std::vector<PtrNode>& nodes): m_name(name) {
    for(PtrNode node: nodes) {
        addNode(node);
    }
}

bool DirectedGraph::hasNode(PtrNode node) const {
    return m_adj.find(node) != m_adj.end();
}

std::optional<PtrNode> DirectedGraph::nodeByName(const std::string& name) const {
    for (const auto& [node, adj]: m_adj ) {
        if (node->name() == name) {
            return {node};
        }
    }
    return {};
}


bool DirectedGraph::addNode(PtrNode node) {
    if (m_adj.find(node) != m_adj.end()) {
        return false;
    }
    m_adj[node] = {{"inbound", {}}, {"outbound", {}}};
    return true;
}

bool DirectedGraph::addEdge(PtrNode from, PtrNode to, const std::string& label) {
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

bool DirectedGraph::addEdge(PtrNode from, PtrNode to) {
    return addEdge(from, to, std::string{});
}

bool DirectedGraph::removeEdge(PtrNode from, PtrNode to) {
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

std::vector<PtrNode> DirectedGraph::nodes() const {
    std::vector<PtrNode> nodes;
    for (const auto& [node, adj]: m_adj) {
        nodes.push_back(node);
    }
    return nodes;
}

std::vector<PtrNode> DirectedGraph::top() const {
    std::vector<PtrNode> nodes;
    for (const auto& [node, adj]: m_adj) {
        if (adj.at("inbound").empty()) {
            nodes.push_back(node);
        }
    }
    return nodes;
}

std::vector<PtrNode> DirectedGraph::bottom() const {
    std::vector<PtrNode> nodes;
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

std::vector<PtrNode> DirectedGraph::inbound(PtrNode node) const {
    std::vector<PtrNode> nodes;
    for (const auto& p: m_adj.at(node).at("inbound")) {
        nodes.push_back(p.first);
    }
    return nodes;
}

std::vector<PtrNode> DirectedGraph::outbound(PtrNode node) const {
    std::vector<PtrNode> nodes;
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

void printNodes(const std::vector<PtrNode>& nodes) {
    for (PtrNode n: nodes) {
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

SubgraphExtractor::SubgraphExtractor(DirectedGraph* graph): m_graph(graph) {}

void SubgraphExtractor::dfs(PtrNode node, std::unordered_set<PtrNode>& visited, Direction dir) {
    visited.insert(node);
    std::vector<PtrNode> adj;
    if (dir == Direction::in) {
        adj = m_graph->inbound(node);
    }
    else if (dir == Direction::out) {
        adj = m_graph->outbound(node);
    }
    else {
        adj = m_graph->inbound(node);
        for (PtrNode out_node: m_graph->outbound(node)) {
            adj.push_back(out_node);
        }
    }
    for (PtrNode adj_node: adj) {
        if (visited.find(adj_node) == visited.end()) {
            dfs(adj_node, visited, dir);
        }
    }
}

void SubgraphExtractor::ensureNodesExist(const std::vector<PtrNode>& inputs, const std::vector<PtrNode>& outputs) {
    std::vector<PtrNode> boundary_nodes(inputs.begin(), inputs.end());
    boundary_nodes.insert(boundary_nodes.end(), outputs.begin(), outputs.end());
    for (PtrNode node: boundary_nodes) {
        if (!m_graph->hasNode(node)) {
            throw std::runtime_error("Node: " + node->name() + " not present in graph");
        }
    }
}
                
std::unique_ptr<DirectedGraph> SubgraphExtractor::extract(const std::vector<PtrNode>& inputs, const std::vector<PtrNode>& outputs) {
    // TODO handle case of invalid inputs, outputs - outputs not reachable from inputs
    // TODO handle inputs/outputs where one is ancestor/descendant of another
    ensureNodesExist(inputs, outputs);

    std::unordered_set<PtrNode> outward_subgraph_nodes(outputs.begin(), outputs.end());
    for (PtrNode node: inputs) {
        if (outward_subgraph_nodes.find(node) != outward_subgraph_nodes.end()) {
            continue;
        }
        dfs(node, outward_subgraph_nodes, Direction::out);
    }

    std::unordered_set<PtrNode> inward_subgraph_nodes(inputs.begin(), inputs.end());
    for (PtrNode node: outputs) {
        if (inward_subgraph_nodes.find(node) != inward_subgraph_nodes.end()) {
            continue;
        }
        dfs(node, inward_subgraph_nodes, Direction::in);
    }

    inward_subgraph_nodes.merge(outward_subgraph_nodes);
    std::unordered_set<PtrNode>& subgraph_nodes = inward_subgraph_nodes;

    return cloneGraph(subgraph_nodes);
}

std::unique_ptr<DirectedGraph> SubgraphExtractor::cloneGraph(const std::unordered_set<PtrNode>& nodes) const {
   auto graph_clone = std::make_unique<DirectedGraph>("subgraph");
    std::unordered_map<PtrNode, PtrNode> clone_map;
    for (PtrNode node: nodes) {
        if (clone_map.find(node) == clone_map.end()) {
            clone_map[node] = std::make_shared<Node>(node->data(), node->name());
        }
        PtrNode node_clone = clone_map[node];
        graph_clone->addNode(node_clone);
        for (PtrNode out_node: m_graph->outbound(node)) {
            if (nodes.find(out_node) == nodes.end()) {
                continue;
            }
            if (clone_map.find(out_node) == clone_map.end()) {
                clone_map[out_node] = std::make_shared<Node>(out_node->data(), out_node->name());
            }
            PtrNode out_node_clone = clone_map[out_node];
            graph_clone->addEdge(node_clone, out_node_clone);
        }
    }
    return graph_clone;
}
