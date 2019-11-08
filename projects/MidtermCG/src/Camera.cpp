#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <GLFW/glfw3.h>

Camera::Camera() : cameraPosition(glm::vec3(0)),
cameraView(glm::mat4(1.0f)),
Projection(glm::mat4(1.0f))
{
}

Camera::~Camera()
{
}

void Camera::LookAt(const glm::vec3& target, const glm::vec3& up)
{
	cameraView = glm::lookAt(cameraPosition, target, up);
}

void Camera::Rotate(const glm::quat& rot)
{
	// Only update if we have an actual value to rotate by
	if (rot != glm::quat(glm::vec3(0))) {
		cameraView = glm::mat4_cast(rot) * cameraView;
	}
}

void Camera::Move(const glm::vec3& local)
{
	// Only update if we have actually moved
	if (local != glm::vec3(0)) {
		// We only need to subtract since we are already in the camera's local space
		cameraView[3] -= glm::vec4(local, 0);
		// Recalculate our position in world space and cache it
		cameraPosition = -glm::inverse(glm::mat3(cameraView)) * cameraView[3];
	}
}

void Camera::SetProjection(glm::mat4 other)
{
	Projection = other;
}

void Camera::SetView(glm::mat4 other)
{
	cameraView = other;
}

void Camera::SetPosition(const glm::vec3& pos)
{
	cameraView[3] = glm::vec4(-(glm::mat3(cameraView) * pos), 1.0f);
	cameraPosition = pos;
}