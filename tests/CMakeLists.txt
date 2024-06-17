add_executable(
  test_directed_graph
  test_directed_graph.cc
)

target_compile_options(
    test_directed_graph
    PRIVATE
    -g
)

target_link_libraries(
  test_directed_graph
  GTest::gtest_main
  sgex
)

add_executable(
  test_subgraph_extractor
  test_subgraph_extractor.cc
)

target_compile_options(
    test_subgraph_extractor
    PRIVATE
    -g
)

target_link_libraries(
  test_subgraph_extractor
  GTest::gtest_main
  sgex
)

include(GoogleTest)
gtest_discover_tests(test_directed_graph)
gtest_discover_tests(test_subgraph_extractor)
