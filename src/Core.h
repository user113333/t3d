#pragma once

#include <iostream>
#include <string>

#define SUFFIX ".t3d"
#define ERROR std::cout << "Error: "

#define ARRAYSIZE(_ARR)          ((int)(sizeof(_ARR) / sizeof(*(_ARR))))     // Size of a static C-style array. Don't use on pointers!

inline std::string working_dir;
