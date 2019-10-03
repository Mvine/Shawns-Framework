#include "Game.h"
#include <stdexcept>
#define IMGUI_IMPL_OPENGL_LOADER_GLAD
#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_glfw.h"
#include "Logging.h"
#include <GLM/gtc/matrix_transform.hpp>
#include <GLM/gtc/type_ptr.hpp>

#define HEIGHT 800
#define WIDTH 1500
void GlfwWindowResizedCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

Game::Game():
		myWindow(nullptr),
		myWindowTitle("Game"),
		myClearColor(glm::vec4(0.2f, 0.5f, 0.55f, 1.0f))
{}

Game::~Game()
{
}

void Game::Initialize()
{
	// Initialize GLFW
	if (glfwInit() == GLFW_FALSE) {
		LOG_ASSERT(false, "Failed to init GLAD");
	}

	// Enable transparent backbuffers for our windows (note that Windows expects our colors to be pre-multiplied with alpha)
	glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, true);
	// Create a new GLFW window
	myWindow = glfwCreateWindow(WIDTH, HEIGHT, myWindowTitle, nullptr, nullptr);
	// We want GL commands to be executed for our window, so we make our window's context the current one
	glfwMakeContextCurrent(myWindow);

	//Tie our game to the window
	glfwSetWindowUserPointer(myWindow, this);
	
	//change whats drawn on window resize
	glfwSetWindowSizeCallback(myWindow, GlfwWindowResizedCallback);
	
	// Let glad know what function loader we are using (will call gl commands via glfw)
	if (gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) == 0) {
		std::cout << "Failed to initialize Glad" << std::endl;
		throw std::runtime_error("Failed to initialize GLAD");
	}

	glEnable(GL_DEPTH_TEST);
	
}

void Game::Shutdown()
{
	glfwTerminate();
}

void Game::LoadContent()
{
	myCamera = std::make_shared<Camera>();
	myCamera->SetPosition(glm::vec3(0, 0, -20));
	myCamera->LookAt(glm::vec3(0));
	// Create a new mesh from the data

	cubeMesh = std::make_shared<Mesh>("cube.obj");
}

void Game::UnloadContent()
{
}

void Game::InitImGui()
{
		// Creates a new ImGUI context
		ImGui::CreateContext();
		// Gets our ImGUI input/output
		ImGuiIO& io = ImGui::GetIO();
		// Enable keyboard navigation
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		// Allow docking to our window
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		// Allow multiple viewports (so we can drag ImGui off our window)
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
		// Allow our viewports to use transparent backbuffers
		io.ConfigFlags |= ImGuiConfigFlags_TransparentBackbuffers;
		// Set up the ImGui implementation for OpenGL
		ImGui_ImplGlfw_InitForOpenGL(myWindow, true);
		ImGui_ImplOpenGL3_Init("#version 410");
		// Dark mode FTW
		ImGui::StyleColorsDark();
		// Get our imgui style
		ImGuiStyle& style = ImGui::GetStyle();
		//style.Alpha = 1.0f;
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 0.8f;
		}

}

void Game::ShutdownImGui()
{
	// Cleanup the ImGui implementation
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	// Destroy our ImGui context
	ImGui::DestroyContext();
}

void Game::ImGuiNewFrame()
{
	// Implementation new frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	// ImGui context new frame
	ImGui::NewFrame();

}

void Game::ImGuiEndFrame()
{
	// Make sure ImGui knows how big our window is
	ImGuiIO& io = ImGui::GetIO();
	int width{ 0 }, height{ 0 };
	glfwGetWindowSize(myWindow, &width, &height);
	io.DisplaySize = ImVec2(width, height);
	// Render all of our ImGui elements
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	// If we have multiple viewports enabled (can drag into a new window)
	
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
		// Update the windows that ImGui is using
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		// Restore our gl context
		glfwMakeContextCurrent(myWindow);
	}

}

void Game::Run()
{
	Initialize();
	InitImGui();
	LoadContent();
	static float prevFrame = glfwGetTime();

	//creating the shaders from strings
	myShader = std::make_shared<Shader>();
	myShader->Load("source.vert", "source.frag");


	while (!glfwWindowShouldClose(myWindow))
	{
		// Poll for events from windows
		// clicks, key presses, closing, all that
		glfwPollEvents();

		//Enabling Wireframe / filll draw mode
		if (isWireframe)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		float thisFrame = glfwGetTime();
		float deltaTime = thisFrame - prevFrame;
		Update(deltaTime);

		//draw vertices
		Draw(deltaTime);

		cubeMesh->model = glm::mat4(1.0f);
		//check to see if we should be passing in the ortho matrix or the perspective matrix
		if (myCamera->isOrtho)
			myCamera->SetProjection(glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.1f, 100.0f));
		else
			myCamera->SetProjection(glm::perspective(fov, (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f));

		//model translation
		cubeMesh->model = glm::translate(cubeMesh->model, glm::vec3(-5.0f, 0.0f, -2.0f));
		cubeMesh->model = glm::rotate(cubeMesh->model,glm::radians(thisFrame * 20), glm::vec3(0.0f, 1.0f, 0.0f));
		
		
		//set projection matrix
		myShader->Use();
		myShader->setMat4("model", cubeMesh->model);
		myShader->setMat4("view", myCamera->GetView());
		myShader->setMat4("projection", myCamera->GetProjection());
		
		//other updates
		ImGuiNewFrame();
		DrawGui(deltaTime);
		ImGuiEndFrame();
		prevFrame = thisFrame;
		// Present our image to windows
		glfwSwapBuffers(myWindow);
	}
	UnloadContent();
	ShutdownImGui();
	Shutdown();
}

void Game::Update(float deltaTime)
{
	glm::vec3 movement = glm::vec3(0.0f);
	glm::vec3 rotation = glm::vec3(0.0f);
	float speed = 10.0f;
	float rotSpeed = 20.0f;

	if (glfwGetKey(myWindow, GLFW_KEY_W) == GLFW_PRESS)
		movement.z -= speed * deltaTime;
	if (glfwGetKey(myWindow, GLFW_KEY_S) == GLFW_PRESS)
		movement.z += speed * deltaTime;
	if (glfwGetKey(myWindow, GLFW_KEY_A) == GLFW_PRESS)
		movement.x -= speed * deltaTime;
	if (glfwGetKey(myWindow, GLFW_KEY_D) == GLFW_PRESS)
		movement.x += speed * deltaTime;
	if (glfwGetKey(myWindow, GLFW_KEY_SPACE) == GLFW_PRESS)
		movement.y += speed * deltaTime;
	if (glfwGetKey(myWindow, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
		movement.y -= speed * deltaTime;

	if (glfwGetKey(myWindow, GLFW_KEY_Q) == GLFW_PRESS)
		rotation.z -= rotSpeed * deltaTime;
	if (glfwGetKey(myWindow, GLFW_KEY_E) == GLFW_PRESS)
		rotation.z += rotSpeed * deltaTime;
	if (glfwGetKey(myWindow, GLFW_KEY_UP) == GLFW_PRESS)
		rotation.x -= rotSpeed * deltaTime;
	if (glfwGetKey(myWindow, GLFW_KEY_DOWN) == GLFW_PRESS)
		rotation.x += rotSpeed * deltaTime;
	if (glfwGetKey(myWindow, GLFW_KEY_LEFT) == GLFW_PRESS)
		rotation.y -= rotSpeed * deltaTime;
	if (glfwGetKey(myWindow, GLFW_KEY_RIGHT) == GLFW_PRESS)
		rotation.y += rotSpeed * deltaTime;
	
	// Rotate and move our camera based on input
	myCamera->Rotate(rotation);
	myCamera->Move(movement);
}

void Game::Draw(float deltaTime)
{
	// Clear our screen every frame
	glClearColor(myClearColor.x, myClearColor.y, myClearColor.z, myClearColor.w);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	myShader->Use();
	cubeMesh->Draw();
	//changing the matrix to draw it in a different place a second time
	cubeMesh->model = glm::mat4(1.0f);
	cubeMesh->model = glm::translate(cubeMesh->model, glm::vec3(10.0f, 0.0f, -2.0f));
	cubeMesh->model = glm::rotate(cubeMesh->model, glm::radians((float)glfwGetTime() * 20), glm::vec3(0.0f, 1.0f, 0.0f));
	myShader->setMat4("model", cubeMesh->model);
	cubeMesh->Draw();
}

void Game::DrawGui(float deltaTime)
{
	// Open a new ImGui window
	ImGui::Begin("Test");
	// Draw widgets here
	ImGui::End();
	// Draw a color editor
	ImGui::ColorEdit4("Clear Color", &myClearColor[0]);
	// Check if a textbox has changed, and update our window title if it has
	if (ImGui::InputText("Window Title", myWindowTitle, 32)) {
		glfwSetWindowTitle(myWindow, myWindowTitle);
	}
	//orthographic camera view
	ImGui::Checkbox("Orthographic", &myCamera->isOrtho);
	//wireframe draw mode
	ImGui::Checkbox("Wireframe", &isWireframe);
	ImGui::SliderFloat("FOV", &fov, 40.0f, 60.0f);
}