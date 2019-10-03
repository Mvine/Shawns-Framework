#pragma once
#include <GLM/glm.hpp>
#include <memory>
#include <GLM/glm.hpp>
#include <glm/gtc/quaternion.hpp>
class Camera {
public:
	typedef std::shared_ptr<Camera> Sptr;

	Camera();
	virtual ~Camera();

	glm::mat4 Projection;
	bool isOrtho = false; 

	const glm::mat4& GetView() const { return cameraView; }
	// Gets the camera's view projection 
	const glm::mat4& GetProjection() const { return Projection; }
	//set projection matrix
	void SetProjection(glm::mat4 other);
	//set view matrix  
	void SetView(glm::mat4 other);
	
	const glm::vec3& GetPosition() const { return cameraPosition; }

	
	void SetPosition(const glm::vec3& pos);
	// Gets the front facing vector of this camera
	inline glm::vec3 GetForward() const { return glm::vec3(-BackX, -BackY, -BackZ); }
	// Gets the up vector of this camera
	inline glm::vec3 GetUp() const { return glm::vec3(UpX, UpY, UpZ); }
	// Gets the right hand vector of this camera
	inline glm::vec3 GetRight() const { return glm::vec3(-LeftX, -LeftY, -LeftZ); }
	// Get projection mode
	inline bool getOrthoMode() const { return isOrtho; };

	void LookAt(const glm::vec3& target, const glm::vec3& up = glm::vec3(0, 1, 0));
	void Rotate(const glm::quat& rot);
	void Rotate(const glm::vec3& rot) { Rotate(glm::quat(glm::radians(rot))); }
	void Move(const glm::vec3& local);
protected:
	glm::vec3 cameraPosition;

	// We are going to declare a union between multiple data members
	// These will exist in the same spot in memory, but can be accessed
	// with different names
	union {
		// The base type is our view matrix
		glm::mat4 cameraView;
		// The next types are all grouped together
		struct {
			float
				LeftX, UpX, BackX, M03,
				LeftY, UpY, BackY, M13,
				LeftZ, UpZ, BackZ, M33,
				TransX, TransY, TransZ, M43;
		};
	};
};