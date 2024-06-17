add_executable(SubgraphExtractor main.cc subg.cc nn_subg.cc onnx.proto3.pb.cc)
target_include_directories(SubgraphExtractor PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../include)
target_link_libraries(SubgraphExtractor PRIVATE protobuf)

add_library(sgex STATIC subg.cc nn_subg.cc onnx.proto3.pb.cc)
target_include_directories(sgex PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(sgex protobuf)
target_compile_options(sgex PRIVATE -g -O0)
