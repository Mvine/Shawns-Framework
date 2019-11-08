#include "GameObject.h"

marcos::GameObject::GameObject(Mesh::Sptr a_mesh = 0)
{
	mesh = a_mesh;
}

marcos::GameObject::GameObject()
{
	mesh = 0;
	position = glm::vec3(0.0f, 0.0f, 0.0f);
	direction = glm::vec3(0.0f, 0.0f, 0.0f);
	speed = 0;
	size = glm::vec2(0.0f, 0.0f);	
}
