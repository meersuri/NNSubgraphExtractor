#include <iostream>

#include "subgraph_extractor.h"
#include "onnx.proto3.pb.h"

std::vector<std::string> parseNames(const std::string& names) {
    std::vector<std::string> parsed_names;
    std::string name;
    for (char c: names) {
        if (c == ',') {
            if (!name.empty()) {
                parsed_names.push_back(std::move(name));
                name.clear();
            }
        }
        else {
            name.push_back(c);
        }
    }
    parsed_names.push_back(std::move(name));
    return parsed_names;
}

int main(int argc, const char* argv[]) {
    if (argc < 2) {
        throw std::runtime_error("Specify path to model");
    }
    if (argc < 3) {
        throw std::runtime_error("Enter inputs and outputs");
    }
    if (argc < 4) {
        throw std::runtime_error("Enter outputs");
    }
    auto model = std::make_shared<OnnxModel>(argv[1]);
    OnnxSubgraphExtractor ex(model);
    auto new_model = ex.extract(parseNames(argv[2]), parseNames(argv[3]));
    new_model->save("cropped.onnx");
    return 0;
}
