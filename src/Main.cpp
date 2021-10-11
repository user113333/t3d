#include "flags.h"

#include <iostream>
#include <string_view>

#include "Core.h"
#include "Util.h"
#include "t3d_export.h"

int main(int argc, char** argv) {
    flags::args args(argc, argv);

    const auto& files = args.positional();
    if (files.size() == 0) {
        ERROR << "no input files" << std::endl;
        return 0;
    }
    std::string file = std::string(files[0]);

    std::string_view out_file = args.get<std::string_view>("o", GetBasename(file) + SUFFIX);

    // other

    t3d_export(file, std::string(out_file));

    return 0;
}
