#ifndef NODE_H
#define NODE_H

#include <string>
#include <vector>
#include <iostream>

namespace sgex {

template <typename T>
class Node {
    T m_data;
    std::vector<Node<T>*> m_inbound;
    std::vector<Node<T>*> m_outbound;
    public:
        Node<T>(T data): m_data(data) {}
        T& data() { return m_data; }
        const T& data() const { return m_data; }
        std::vector<Node<T>*>& inbound() { return m_inbound; }
        const std::vector<Node<T>*>& inbound() const { return m_inbound; }
        std::vector<Node<T>*>& outbound() { return m_outbound; }
        const std::vector<Node<T>*>& outbound() const { return m_outbound; }
};

template <typename T>
std::ostream& operator<<(std::ostream& os, const Node<T>& node) {
    return os << node.data();
}

}

#endif
