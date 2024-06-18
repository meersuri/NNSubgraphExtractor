#include "subgraph_extractor.h"
#include <gtest/gtest.h>

TEST(LineGraphTests, extractSequence) {
    auto graph = std::make_unique<DirectedGraph>("g");
    std::vector<std::shared_ptr<Node>> nodes;
    std::vector<std::string> node_names{"a", "b", "c", "d", "e", "f"};
    for (int i = 0; i < node_names.size(); ++i) {
        nodes.push_back(std::make_shared<Node>(node_names[i]));
        if (i > 0) {
            graph->addEdge(nodes[i - 1], nodes[i]);
        }
    }
    SubgraphExtractor ex(graph.get());
    for (auto& [start, end]: std::vector<std::pair<int, int>>{{0,1}, {1,3}, {2,4}, {2,3}, {0, 4}}) {
        auto subg = ex.extract({nodes[start]}, {nodes[end]});
        ASSERT_TRUE(subg->nodes().size() == end - start + 1);
    }
}

