#pragma once

#include <string>

#include <assimp/matrix4x4.h>
#include <glm/glm.hpp>

#include "t3d.h"

std::string GetDir(std::string& path);
std::string GetNotDir(std::string& path);
std::string GetSuffix(std::string& path);
std::string GetBasename(std::string& path);

t3d::Texture LoadImageTT(const char* path);

static inline glm::mat4 CastMat4(const aiMatrix4x4 &m) { return { m.a1, m.b1, m.c1, m.d1, m.a2, m.b2, m.c2, m.d2, m.a3, m.b3, m.c3, m.d3, m.a4, m.b4, m.c4, m.d4 }; }
