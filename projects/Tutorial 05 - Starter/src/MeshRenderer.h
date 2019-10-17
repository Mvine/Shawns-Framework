#pragma once
#include "Materials.h"
#include "Mesh.h"
struct MeshRenderer {
	Material_sptr Material;
	Mesh_sptr Mesh;
};