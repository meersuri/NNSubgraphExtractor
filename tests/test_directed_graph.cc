#include "graph.h"
#include <gtest/gtest.h>

TEST(GraphManipulation, addNode) {
    DirectedGraph graph("g");
    auto n1 = std::make_unique<Node>(1);
    auto n2 = std::make_unique<Node>(2);
    auto n3 = std::make_unique<Node>(3);
    ASSERT_TRUE(graph.addNode(n1.get()));
    ASSERT_TRUE(!graph.addNode(n1.get()));
    ASSERT_TRUE(graph.hasNode(n1.get()));
    ASSERT_TRUE(!graph.hasNode(n2.get()));
    ASSERT_TRUE(!graph.hasNode(n3.get()));
    graph.addNode(n2.get());
    ASSERT_TRUE(graph.hasNode(n2.get()));
    graph.addNode(n3.get());
    ASSERT_TRUE(graph.hasNode(n3.get()));
}

TEST(GraphManipulation, addEdge) {
    DirectedGraph graph("g");
    auto n1 = std::make_unique<Node>(1);
    auto n2 = std::make_unique<Node>(2);
    auto n3 = std::make_unique<Node>(3);
    ASSERT_TRUE(graph.addEdge(n1.get(), n2.get()));
    ASSERT_TRUE(!graph.addEdge(n1.get(), n2.get()));
    ASSERT_TRUE(graph.addEdge(n1.get(), n3.get()));
    ASSERT_TRUE(graph.nodes().size() == 3);
    ASSERT_TRUE(graph.edges().size() == 2);
}

TEST(GraphManipulation, removeEdge) {
    DirectedGraph graph("g");
    auto n1 = std::make_unique<Node>(1);
    auto n2 = std::make_unique<Node>(2);
    auto n3 = std::make_unique<Node>(3);
    graph.addEdge(n1.get(), n2.get());
    ASSERT_TRUE(graph.removeEdge(n1.get(), n2.get()));
    ASSERT_TRUE(!graph.removeEdge(n1.get(), n2.get()));
    ASSERT_TRUE(graph.edges().size() == 0);
    ASSERT_TRUE(graph.addEdge(n1.get(), n3.get()));
    ASSERT_TRUE(graph.edges().size() == 1);
}

TEST(GraphManipulation, topBottom) {
    DirectedGraph graph("g");
    auto n1 = std::make_unique<Node>(1);
    auto n2 = std::make_unique<Node>(2);
    auto n3 = std::make_unique<Node>(3);
    auto n4 = std::make_unique<Node>(4);
    graph.addEdge(n1.get(), n4.get());
    graph.addEdge(n2.get(), n4.get());
    graph.addEdge(n3.get(), n4.get());
    auto top = graph.top();
    auto bottom = graph.bottom();
    ASSERT_TRUE(bottom == std::vector<Node*>{n4.get()});
    ASSERT_TRUE(top.size() == 3);
    auto top_set = std::set<Node*>(top.begin(), top.end());
    ASSERT_TRUE((top_set == std::set<Node*>{n1.get(), n2.get(), n3.get()}));
}
