#ifndef GRAPH_H
#define GRAPH_H

#include <memory>
#include <map>

#include "node.h"

namespace sgex {

template <typename T>
class Graph {
    std::map<T, std::unique_ptr<Node<T>>> m_nodes;
    public:
        Graph<T>() = default;
        Graph<T>(std::vector<std::pair<T, T>> edges);

        bool add_node(T data);
        void add_edge(T a, T b); // add an edge if one doesn't already exist
};

template <typename T>
void Graph<T>::add_edge(T a, T b) {
    Node<T>* pa = nullptr;
    Node<T>* pb = nullptr;
    if (m_nodes.find(a) == m_nodes.end()) {
        m_nodes.insert(std::make_pair(a, std::make_unique<Node<T>>(a))); 
    }
    if (m_nodes.find(b) == m_nodes.end()) {
        m_nodes.insert(std::make_pair(b, std::make_unique<Node<T>>(b)));
    }
    pa = m_nodes.at(a).get();
    pb = m_nodes.at(b).get();
    pa->outbound().push_back(pb);
    pb->inbound().push_back(pa);
}

template <typename T>
bool Graph<T>::add_node(T a) {
    if (m_nodes.find(a) == m_nodes.end()) {
        m_nodes.insert(std::make_pair(a, std::make_unique<Node<T>>(a)));
        return true;
    }
    return false;
}

}

#endif
