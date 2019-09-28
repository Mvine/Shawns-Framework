#pragma once
#include <glad/glad.h>
#include <GLM/glm.hpp> // For vec3 and vec4
#include <cstdint> // Needed for uint32_t
#include <memory> // Needed for smart pointers
struct Vertex {
	glm::vec3 Position;
	glm::vec4 Color;
};

class Mesh {
public:
	// Shorthand for shared_ptr
	typedef std::shared_ptr<Mesh> Sptr;
	// Creates a new mesh from the given vertices and indices
	Mesh(Vertex* vertices, size_t numVerts, uint32_t* indices, size_t numIndices);
	// Create a new mesh with an obj file
	Mesh(const char* objFilePath);
	~Mesh();
	
	// Draws this mesh
	void Draw();
private:
	// Our GL handle for the Vertex Array Object
	GLuint myVao;
	// 0 is vertices, 1 is indices
	GLuint myBuffers[2];
	// The number of vertices and indices in this mesh
	size_t myVertexCount, myIndexCount;
	//model matrix
	glm::mat4 model = glm::mat4(1.0f);
};