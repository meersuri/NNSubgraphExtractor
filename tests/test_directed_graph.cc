#include "graph.h"
#include <gtest/gtest.h>

TEST(GraphManipulation, addNode) {
    DirectedGraph graph("g");
    auto n1 = std::make_shared<Node>(1);
    auto n2 = std::make_shared<Node>(2);
    auto n3 = std::make_shared<Node>(3);
    ASSERT_TRUE(graph.addNode(n1));
    ASSERT_TRUE(!graph.addNode(n1));
    ASSERT_TRUE(graph.hasNode(n1));
    ASSERT_TRUE(!graph.hasNode(n2));
    ASSERT_TRUE(!graph.hasNode(n3));
    graph.addNode(n2);
    ASSERT_TRUE(graph.hasNode(n2));
    graph.addNode(n3);
    ASSERT_TRUE(graph.hasNode(n3));
}

TEST(GraphManipulation, addEdge) {
    DirectedGraph graph("g");
    auto n1 = std::make_shared<Node>(1);
    auto n2 = std::make_shared<Node>(2);
    auto n3 = std::make_shared<Node>(3);
    ASSERT_TRUE(graph.addEdge(n1, n2));
    ASSERT_TRUE(!graph.addEdge(n1, n2));
    ASSERT_TRUE(graph.addEdge(n1, n3));
    ASSERT_TRUE(graph.nodes().size() == 3);
    ASSERT_TRUE(graph.edges().size() == 2);
}

TEST(GraphManipulation, removeEdge) {
    DirectedGraph graph("g");
    auto n1 = std::make_shared<Node>(1);
    auto n2 = std::make_shared<Node>(2);
    auto n3 = std::make_shared<Node>(3);
    graph.addEdge(n1, n2);
    ASSERT_TRUE(graph.removeEdge(n1, n2));
    ASSERT_TRUE(!graph.removeEdge(n1, n2));
    ASSERT_TRUE(graph.edges().size() == 0);
    ASSERT_TRUE(graph.addEdge(n1, n3));
    ASSERT_TRUE(graph.edges().size() == 1);
}

TEST(GraphManipulation, topBottom) {
    DirectedGraph graph("g");
    auto n1 = std::make_shared<Node>(1);
    auto n2 = std::make_shared<Node>(2);
    auto n3 = std::make_shared<Node>(3);
    auto n4 = std::make_shared<Node>(4);
    graph.addEdge(n1, n4);
    graph.addEdge(n2, n4);
    graph.addEdge(n3, n4);
    auto top = graph.top();
    auto bottom = graph.bottom();
    ASSERT_TRUE(bottom == std::vector<std::shared_ptr<Node>>{n4});
    ASSERT_TRUE(top.size() == 3);
    auto top_set = std::set<std::shared_ptr<Node>>(top.begin(), top.end());
    ASSERT_TRUE((top_set == std::set<std::shared_ptr<Node>>{n1, n2, n3}));
}
