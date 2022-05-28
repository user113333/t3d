#include "flags.h"

#include <string_view>

#define T3D_IMPLEMENTATION
#include "t3d.h"
#include "Core.h"
#include "Util.h"

using namespace t3d;

void LoadModel(std::string const &path, Model* model);

void ExportT3D(std::string& in_file, std::string& out_file) {
    Model model;
    
    LoadModel(in_file, &model);
    
    Serialize(out_file.c_str(), &model);
}

int main(int argc, char** argv) {
    flags::args args(argc, argv);

    const auto& files = args.positional();
    if (files.size() == 0) {
        ERROR << "no input files" << std::endl;
        return 0;
    }

    std::string file = std::string(files[0]);
    std::string out_file = std::string(args.get<std::string_view>("o", GetBasename(file) + SUFFIX));
    
    working_dir = GetDir(file);

    ExportT3D(file, out_file);

    return 0;
}
