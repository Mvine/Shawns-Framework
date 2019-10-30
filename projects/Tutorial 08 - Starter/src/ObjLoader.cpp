#include "ObjLoader.h"
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>

struct VIndex {
	uint32_t Pos, Tex, Norm;
};

struct Face {
	VIndex Vertices[3];
};

Mesh::Sptr ObjLoader::LoadObj(const char* filename, glm::vec4 baseColor) {
	// Open our file in binary mode
	std::ifstream file;
	file.open(filename, std::ios::binary);

	// If our file fails to open, we will throw an error
	if (!file) {
		throw new std::runtime_error("Failed to open file");
	}

	// Declare vectors for our positions, normals, and face indices
	std::vector<glm::vec3>   positions;
	std::vector<glm::vec2>   texUvs;
	std::vector<glm::vec4>   normals;
	std::vector<Face>        faces;
	// Stores our w value if it is provided
	float garb;
	// Stores the line that we are operating on
	std::string line;
	
	while (std::getline(file, line)) {
		if (line.substr(0, 2) == "v ") {
			std::istringstream ss = std::istringstream(line.substr(2));
			glm::vec3 pos; ss >> pos.x; ss >> pos.y; ss >> pos.z; ss >> garb;
			positions.push_back(pos);
		}
		else if (line.substr(0, 3) == "vn ") {
			std::istringstream ss = std::istringstream(line.substr(3));
			glm::vec4 col; ss >> col.x; ss >> col.y; ss >> col.z; ss >> col.w;
			col.w = 1.0f;
			normals.push_back(col);
		}
		else if (line.substr(0, 3) == "vt ") {
			std::istringstream ss = std::istringstream(line.substr(3));
			glm::vec2 uv;
			ss >> uv.x; ss >> uv.y;
			texUvs.push_back(uv);
		}
		else if (line.substr(0, 2) == "f ") {
			std::istringstream ss = std::istringstream(line.substr(2));
			GLuint a{ 0 }, b{ 0 }, c{ 0 };
			Face face;
			if (line.find('/') == std::string::npos) {
				ss >> a >> b >> c;
				a--; b--; c--;
				face.Vertices[0] = { a, a, a }; 
				face.Vertices[1] = { b, b, b }; 
				face.Vertices[2] = { c,c,c };
			}
			else {
				for (int ix = 0; ix < 3; ix++) {
					std::string token;
					ss >> token;
					sscanf(token.c_str(), "%d/%d/%d", &a, &b, &c);
					a--; b--; c--;
					face.Vertices[ix].Pos = a; face.Vertices[ix].Tex = b; face.Vertices[ix].Norm = c;
				}
			}
			faces.push_back(face);
		}
	}

	// Allocate a new array for our vertices
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;
	// Iterate over all the positions we've read
	for (int ix = 0; ix < faces.size(); ix++) {
		for (auto fIX : faces[ix].Vertices) {
			indices.push_back(vertices.size());
			// Load the vertex from the attributes
			Vertex vertex;
			vertex.Position = fIX.Pos != (uint32_t)-1 ? positions[fIX.Pos] : glm::vec3(0);
			vertex.Color = baseColor;
			vertex.Normal = fIX.Norm != (uint32_t)-1 ? normals[fIX.Norm] : baseColor;
			// Add the vertex to the buffer
			vertices.push_back(vertex);
		}
	}
	// Create the mesh. Note that we are directly accessing the index vector's underlying data here
	return std::make_shared<Mesh>(
		vertices.data(), 
		vertices.size(), 
		indices.data(), 
		indices.size());
}
