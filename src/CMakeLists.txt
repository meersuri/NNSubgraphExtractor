add_executable(SubgraphExtractor main.cc graph.cc subgraph_extractor.cc onnx.proto3.pb.cc)
target_include_directories(SubgraphExtractor PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../include)
target_link_libraries(SubgraphExtractor PRIVATE protobuf spdlog::spdlog)

add_library(sgex STATIC graph.cc subgraph_extractor.cc onnx.proto3.pb.cc)
target_include_directories(sgex PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(sgex protobuf spdlog::spdlog)
target_compile_options(sgex PRIVATE -g -O0)
