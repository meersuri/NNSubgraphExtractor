#include "subgraph_extractor.h"
#include <gtest/gtest.h>

TEST(LineGraphTests, extractSequence) {
    DirectedGraph graph("g");
    std::vector<std::unique_ptr<Node>> nodes;
    std::vector<std::string> node_names{"a", "b", "c", "d", "e", "f"};
    for (int i = 0; i < node_names.size(); ++i) {
        nodes.push_back(std::make_unique<Node>(node_names[i]));
        if (i > 0) {
            graph.addEdge(nodes[i - 1].get(), nodes[i].get());
        }
    }
    SubgraphExtractor ex(graph);
    for (auto& [start, end]: std::vector<std::pair<int, int>>{{0,1}, {1,3}, {2,4}, {2,3}, {0, 4}}) {
        DirectedGraph subg = ex.extract({nodes[start].get()}, {nodes[end].get()});
        ASSERT_TRUE(subg.nodes().size() == end - start + 1);
    }
}

