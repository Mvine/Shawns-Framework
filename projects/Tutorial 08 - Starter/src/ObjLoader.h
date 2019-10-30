#pragma once

#include "Mesh.h"

class ObjLoader {
public:
	static Mesh::Sptr LoadObj(const char* filename, glm::vec4 baseColor);
};