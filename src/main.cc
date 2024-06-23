#include <iostream>

#include "subgraph_extractor.h"
#include "onnx.proto3.pb.h"
#include "CLI11.hpp"

std::vector<std::string> parseNames(const std::string& names) {
    if (names.empty()) {
        return {};
    }
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
    CLI::App app{"SubgraphExtractor!!"};
    std::string input_names, output_names, model_path, output_path;
    app.add_option("-f, --file", model_path, "Path to .onnx model")->required();
    app.add_option("-i, --inputs", input_names, "Name(s) of input node(s)");
    app.add_option("-o, --outputs", output_names, "Name(s) of output node(s)");
    app.add_option("-n, --name", output_names, "Output model path");
    CLI11_PARSE(app, argc, argv);
    size_t pos = model_path.find(".onnx");
    if (output_path.empty()) {
        output_path = model_path.substr(0, pos) + "_subgraph.onnx";
    }
    auto model = std::make_shared<OnnxModel>(model_path);
    OnnxSubgraphExtractor ex(model);
    auto new_model = ex.extract(parseNames(input_names), parseNames(output_names));
    if (output_path.find(".onnx") == std::string::npos) {
        output_path += ".onnx";
    }
    new_model->save(output_path);
    return 0;
}
