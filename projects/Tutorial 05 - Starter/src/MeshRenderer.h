#pragma once
#include "Materials.h"
#include "Mesh.h"
struct MeshRenderer {
	Materials::Sptr Material;
	Mesh_sptr Mesh;
};