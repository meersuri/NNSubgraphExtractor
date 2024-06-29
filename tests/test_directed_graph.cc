#include "graph.h"
#include <gtest/gtest.h>

TEST(GraphManipulation, addNode) {
    DirectedGraph graph("g");
    auto n1 = std::make_shared<Node>(1);
    auto n2 = std::make_shared<Node>(2);
    auto n3 = std::make_shared<Node>(3);
    ASSERT_TRUE(graph.addNode(n1));
    ASSERT_FALSE(graph.addNode(n1));
    ASSERT_TRUE(graph.hasNode(n1));
    ASSERT_FALSE(graph.hasNode(n2));
    ASSERT_FALSE(graph.hasNode(n3));
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
    ASSERT_FALSE(graph.addEdge(n1, n2));
    ASSERT_TRUE(graph.addEdge(n1, n3));
    ASSERT_EQ(graph.nodes().size(), 3);
    ASSERT_EQ(graph.edges().size(), 2);
}

TEST(GraphManipulation, removeEdge) {
    DirectedGraph graph("g");
    auto n1 = std::make_shared<Node>(1);
    auto n2 = std::make_shared<Node>(2);
    auto n3 = std::make_shared<Node>(3);
    graph.addEdge(n1, n2);
    ASSERT_TRUE(graph.removeEdge(n1, n2));
    ASSERT_FALSE(graph.removeEdge(n1, n2));
    ASSERT_EQ(graph.edges().size(), 0);
    ASSERT_TRUE(graph.addEdge(n1, n3));
    ASSERT_EQ(graph.edges().size(), 1);
}

TEST(GraphQueries, topBottom) {
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
    ASSERT_EQ(top.size(), 3);
    auto top_set = std::set<std::shared_ptr<Node>>(top.begin(), top.end());
    std::set<std::shared_ptr<Node>> true_top_set{n1, n2, n3};
    ASSERT_EQ(top_set, true_top_set);
}

TEST(GraphQueries, getNodeData) {
    auto n1 = std::make_shared<Node>('a');
    ASSERT_EQ(std::any_cast<char>(n1->data()), 'a');
    auto n2 = std::make_shared<Node>(1);
    ASSERT_EQ(std::any_cast<int>(n2->data()), 1);
}

TEST(GraphQueries, adjacency) {
    auto n1 = std::make_shared<Node>('a');
    DirectedGraph graph("g");
    graph.addNode(n1);
    ASSERT_EQ(graph.inbound(n1).size(), 0);
    auto n2 = std::make_shared<Node>('b');
    graph.addEdge(n1, n2);
    ASSERT_EQ(graph.inbound(n2), std::vector<std::shared_ptr<Node>>{n1});
    ASSERT_EQ(graph.outbound(n1), std::vector<std::shared_ptr<Node>>{n2});
    auto n3 = std::make_shared<Node>('c');
    graph.addEdge(n3, n2);
    std::vector<std::shared_ptr<Node>> n2_true_inbound{n1, n3};
    ASSERT_EQ(graph.inbound(n2), n2_true_inbound);
    graph.addEdge(n2, n3);
    graph.addEdge(n2, n1);
    std::vector<std::shared_ptr<Node>> n2_true_outbound{n1, n3};
    auto n2_out = graph.outbound(n2);
    std::sort(n2_out.begin(), n2_out.end(), [](const auto& node1, const auto& node2) {
            return std::any_cast<char>(node1->data()) < std::any_cast<char>(node2->data()); }
            );
    ASSERT_EQ(n2_out, n2_true_outbound);
    ASSERT_EQ(graph.inbound(n1), std::vector<std::shared_ptr<Node>>{n2});
    ASSERT_EQ(graph.inbound(n3), std::vector<std::shared_ptr<Node>>{n2});
}
