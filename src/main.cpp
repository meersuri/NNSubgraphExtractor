#include <iostream>
#include <vector>
#include <string>

#include "node.h"
#include "graph.h"

int main() {
    auto a = sgex::Node("hello");
    auto b = sgex::Node(2);
    auto c = sgex::Node(3);
    std::cout << a << '\n';
    std::cout << b << '\n';
    b.inbound().push_back(&c);
    auto G = sgex::Graph<int>();
    G.add_edge(5,7);
    G.add_edge(3,4);
    G.add_edge(4,5);
//    std::cout << G.edges() << '\n';
//    std::cout << G.nodes() << '\n';
//    H = sgex::Graph({{"a", "b"}, {"b", "c"}});
//    H.add_edge("c", "d");
//    std::cout << H.edges() << '\n';
//    std::cout << H.nodes() << '\n';
    return 0;
}

