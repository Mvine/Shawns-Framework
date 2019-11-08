#include "Shader.h"
#include <Logging.h>
#include <fstream>

char* readFile(const char* fileName)
{
	// Declare and open the file stream
	std::ifstream file;
	file.open(fileName, std::ios::binary);
	// Only read if the file is open
	if (file.is_open()) {
		// Get the starting location in the file
		uint64_t fileSize = file.tellg();
		// Seek to the end
		file.seekg(0, std::ios::end);
		// Calculate the file size from end to beginning
		fileSize = (uint64_t)file.tellg() - fileSize;
		// Seek back to the beginning of the file
		file.seekg(0, std::ios::beg);
		// Allocate space for our entire file, +1 byte at the end for null terminator
		char* result = new char[fileSize + 1];
		// Read the entire file to our memory
		file.read(result, fileSize);
		// Make our text null-terminated
		result[fileSize] = '\0';
		// Close the file before returning
		file.close();
		return result;
	}
	// Otherwise, we failed to open our file, throw a runtime error
	else {
		throw std::runtime_error("We cannot open the file!");
	}
}
Shader::Shader() {
	myShaderHandle = glCreateProgram();
}
Shader::~Shader() {
	glDeleteProgram(myShaderHandle);
}
void Shader::Use() {
	glUseProgram(myShaderHandle);
}

void Shader::setFloat(const std::string& name, const float& a_Float)
{
	glUniform1f(glGetUniformLocation(myShaderHandle, name.c_str()), a_Float);
}

void Shader::setInt(const std::string& name, const int& a_Int)
{
	glUniform1i(glGetUniformLocation(myShaderHandle, name.c_str()), a_Int);
}
 
void Shader::setBoolean(const std::string& name, const bool& a_bool) 
{
	glUniform1i(glGetUniformLocation(myShaderHandle, name.c_str()), a_bool);
}

void Shader::setVec2(const std::string& name, const glm::vec2& vec2)
{
	glUniform2fv(glGetUniformLocation(myShaderHandle, name.c_str()),1 , &vec2[0]);
}

void Shader::setVec3(const std::string& name, const glm::vec3& vec3)
{
	glUniform3fv(glGetUniformLocation(myShaderHandle, name.c_str()), 1, &vec3[0]);
}

void Shader::setVec4(const std::string& name, const glm::vec4& vec4)
{
	glUniform4fv(glGetUniformLocation(myShaderHandle, name.c_str()), 1, &vec4[0]);
}

void Shader::setMat4(const std::string& name, const glm::mat4& mat4) 
{
	glUniformMatrix4fv(glGetUniformLocation(myShaderHandle, name.c_str()), 1, GL_FALSE, &mat4[0][0]);
}

void Shader::setMat3(const std::string& name, const glm::mat3& mat3)
{
	glUniformMatrix3fv(glGetUniformLocation(myShaderHandle, name.c_str()), 1, GL_FALSE, &mat3[0][0]);
}

void Shader::setMat2(const std::string& name, const glm::mat2& mat2)
{
	glUniformMatrix2fv(glGetUniformLocation(myShaderHandle, name.c_str()), 1, GL_FALSE, &mat2[0][0]);
}

GLuint Shader::getID()
{
	return myShaderHandle;
}



void Shader::Load(const char* vsFile, const char* fsFile) {
	// Load in our shaders
	char* vs_source = readFile(vsFile);
	char* fs_source = readFile(fsFile);
	// Compile our program
	Compile(vs_source, fs_source);
	// Clean up our memory
	delete[] fs_source;
	delete[] vs_source;

}

GLuint Shader::__CompileShaderPart(const char* source, GLenum type) {
	GLuint result = glCreateShader(type);
	// Load in our shader source and compile it
	glShaderSource(result, 1, &source, NULL);
	glCompileShader(result);
	// Check our compile status
	GLint compileStatus = 0;
	glGetShaderiv(result, GL_COMPILE_STATUS, &compileStatus);
	// If we failed to compile
	if (compileStatus == GL_FALSE) {
		// Get the size of the error log
		GLint logSize = 0;
		glGetShaderiv(result, GL_INFO_LOG_LENGTH, &logSize);
		// Create a new character buffer for the log
		char* log = new char[logSize];
		// Get the log
		glGetShaderInfoLog(result, logSize, &logSize, log);
		// Dump error log
		LOG_ERROR("Failed to compile shader part:\n{}", log);
		// Clean up our log memory
		delete[] log;
		// Delete the broken shader result
		glDeleteShader(result);
		// Throw a runtime exception
		throw new std::runtime_error("Failed to compile shader part!");
	}
	else {
		LOG_TRACE("Shader part has been compiled!");
	}
	// Return the compiled shader part
	return result;
}

void Shader::Compile(const char* vs_source, const char* fs_source) {
	// Compile our two shader programs
	GLuint vs = __CompileShaderPart(vs_source, GL_VERTEX_SHADER);
	GLuint fs = __CompileShaderPart(fs_source, GL_FRAGMENT_SHADER);
	// Attach our two shaders
	glAttachShader(myShaderHandle, vs); 
	glAttachShader(myShaderHandle, fs);
	// Perform linking
	glLinkProgram(myShaderHandle);
	// Remove shader parts to save space
	glDetachShader(myShaderHandle, vs);
	glDeleteShader(vs);
	glDetachShader(myShaderHandle, fs);
	glDeleteShader(fs);
	// Get whether the link was successful
	GLint success = 0;
	glGetProgramiv(myShaderHandle, GL_LINK_STATUS, &success);
	// If not, we need to grab the log and throw an exception
	if (success == GL_FALSE) {
		// TODO: Read the log file
		// Throw a runtime exception
		throw new std::runtime_error("Failed to link shader program!");
	}
	else {
		LOG_TRACE("Shader has been linked");
	}
	if (success == GL_FALSE) {
		// Get the length of the log
		GLint length = 0;
		glGetProgramiv(myShaderHandle, GL_INFO_LOG_LENGTH, &length);
		if (length > 0) {
			// Read the log from openGL
			char* log = new char[length]; 
			glGetProgramInfoLog(myShaderHandle, length, &length, log);
			LOG_ERROR("Shader failed to link:\n{}", log);
			delete[] log;
		}
		else {
			LOG_ERROR("Shader failed to link for an unknown reason!");
		}
		// Delete the partial program
		glDeleteProgram(myShaderHandle);
		// Throw a runtime exception
		throw new std::runtime_error("Failed to link shader program!");
	}
}