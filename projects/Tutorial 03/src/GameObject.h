#pragma once
#define GLM_ENABLE_EXPERIMENTAL 

#include <GLM/glm.hpp>
#include <GLM\gtx\transform.hpp>
#include <GLM\gtc\type_ptr.hpp>
#include <string>

class GameObject
{
protected:
	float m_pScale;

	float m_pRotX, m_pRotY, m_pRotZ; // local rotation angles

	glm::vec3 m_pLocalPosition;

	// TODO: Compute these matrices in the Update function
	glm::mat4 m_pLocalRotation;
	glm::mat4 m_pLocalToWorldMatrix;

public:
	GameObject();
	~GameObject();

	void setPosition(glm::vec3 newPosition);
	void setRotationAngleX(float newAngle);
	void setRotationAngleY(float newAngle);
	void setRotationAngleZ(float newAngle);
	void setScale(float newScale);

	glm::vec3 getPosition();
	float getRotationAngleY();

	glm::mat4 getLocalToWorldMatrix();

	// TODO: Implement this update function
	virtual void update(float dt);	

	virtual void draw();

	// Other Properties
	std::string name;
	glm::vec4 color; 
	// Mesh* ...
	// Material* ...
};