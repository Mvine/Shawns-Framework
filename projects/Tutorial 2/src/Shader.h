#pragma once
#include <glad/glad.h>
#include <memory>
#include <GLM/glm.hpp>
#include <string>

class Shader {
public:
	typedef std::shared_ptr<Shader> Sptr;
	Shader();
	~Shader();
	void Compile(const char* vs_source, const char* fs_source);
	
	// Loads a shader program from 2 files. vsFile is the path to
	//the vertex shader, and fsFile is the path to the fragment shader
	
	void Load(const char* vsFile, const char* fsFile);
	void Use();


	//setters for passing data into our shaders' uniforms
	void setFloat(const std::string& name, const float &a_Float);
	void setInt(const std::string& name, const int& a_Int);
	void setBoolean(const std::string& name, const bool& a_bool);
	
	void setVec2(const std::string& name, const glm::vec2& vec2);
	void setVec3(const std::string& name, const glm::vec3& vec3);
	void setVec4(const std::string& name, const glm::vec4& vec4);
	
	void setMat4(const std::string& name, const glm::mat4& mat4);
	void setMat3(const std::string& name, const glm::mat3& mat3);
	void setMat2(const std::string& name, const glm::mat2& mat2);
	
	
private:
	GLuint __CompileShaderPart(const char* source, GLenum type);
	GLuint myShaderHandle;
};