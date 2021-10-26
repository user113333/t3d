#pragma once

#include <string>
#include "TextureCPU.h"

std::string GetDir(std::string& path);
std::string GetNotDir(std::string& path);
std::string GetSuffix(std::string& path);
std::string GetBasename(std::string& path);

t3d::TextureCPU LoadImageTT(const char* path);

unsigned long HashStr(unsigned char *str);
