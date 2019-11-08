#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "GLM/glm.hpp"
#include "Shader.h"
#include "Mesh.h"
#include "Camera.h"
#include "GameObject.h"

class Game {
public:
	Game();
	~Game();
	void Run();
protected:
	void Initialize();
	void Shutdown();
	void LoadContent();
	void UnloadContent();
	void InitImGui();
	void ShutdownImGui();
	void ImGuiNewFrame();
	void ImGuiEndFrame();
	void Update(float deltaTime);
	void Draw(float deltaTime);
	void DrawGui(float deltaTime);
	bool testCollisions(marcos::GameObject obj1, marcos::GameObject obj2);
	void updateCollisions();
private:
	// Stores the main window that the game is running in
	GLFWwindow* myWindow;
	// Stores the clear color of the game's window
	glm::vec4 myClearColor;
	// Stores the title of the game's window
	char myWindowTitle[32];
	// A shared pointer to our mesh

	//TODO:
	//find a way to seperate these things man, this ain't gonna work
	Mesh::Sptr paddleMesh;

	Mesh::Sptr ballMesh;

	Mesh::Sptr brickMesh;

	marcos::GameObject ball = marcos::GameObject(ballMesh);
	marcos::GameObject paddle = marcos::GameObject(paddleMesh);
	marcos::GameObject brick[16][3];


	//things that I really shouldn't be putting here
	int lives = 3;
	int score = 0;
	
	
	// A shared pointer to our shader
	Shader::Sptr myShader;
	// A shared pointer to our camera
	Camera::Sptr myCamera;
	//wireframe mode
	bool isWireframe = false;

	float fov = 45.0f;

};