#include <iostream>

#include "subg.h"
#include "nn_subg.h"
#include "onnx.proto3.pb.h"

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
    OnnxModel* model = new OnnxModel(argv[1]);
    OnnxSubgraphExtractor ex(model);
    NNModel* new_model = ex.extract({argv[2]}, {argv[3]});
    new_model->save("cropped.onnx");
    return 0;
}
