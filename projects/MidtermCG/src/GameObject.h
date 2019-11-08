#pragma once
#include <glad/glad.h>
#include <GLM/glm.hpp> // For vec3 and vec4
#include <cstdint> // Needed for uint32_t
#include <memory> // Needed for smart pointers
#include <string> // For filepath
#include "Mesh.h"
#include "Shader.h"

//TODO:
//The meshes need to be seperate they can't all share the same data obviously,
//so having a pointer split between them wont work
//
//objects need basic physics components like colliders, velocity, direction etc
//
//then you can delete the meshes on collisions
//
//some of like the functions and stuff should be taken care of by this class, not the game class but like whatever I guess
//it's not about being clean

namespace marcos
{
	class GameObject
	{

	public:
		GameObject(Mesh::Sptr a_mesh);
		GameObject();
		Mesh::Sptr mesh;
		glm::vec3 position;
		glm::vec3 direction;
		glm::mat4 model;
		float speed;
		glm::vec2 size;

	};
}

