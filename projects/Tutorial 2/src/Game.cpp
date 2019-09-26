#include "Game.h"
#include <stdexcept>
#define IMGUI_IMPL_OPENGL_LOADER_GLAD
#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_glfw.h"
#include "Logging.h"

Game::Game():
		myWindow(nullptr),
		myWindowTitle("Game"),
		myClearColor(glm::vec4(0.1f, 0.7f, 0.5f, 0.8f))
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
	myWindow = glfwCreateWindow(600, 600, myWindowTitle, nullptr, nullptr);
	// We want GL commands to be executed for our window, so we make our window's context the current one
	glfwMakeContextCurrent(myWindow);
	// Let glad know what function loader we are using (will call gl commands via glfw)
	if (gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) == 0) {
		std::cout << "Failed to initialize Glad" << std::endl;
		throw std::runtime_error("Failed to initialize GLAD");
	}

}

void Game::Shutdown()
{
	glfwTerminate();
}

void Game::LoadContent()
{
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
	// Run as long as the window is open


	//triangle data
	static const GLfloat points[] =
	{
		//vertex			color
		-0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
		-0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
		 0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f,
		 0.5f,  0.5f, 0.0f, 0.0f, 0.0f, 0.0f
	};

	static const GLuint indices[] =
	{
		0,1,2,
		1,2,3
	};

	const char* vertexShader = 
	"#version 400\n"
	"layout (location = 0) in vec3 aPos;\n"
	"layout (location = 1) in vec3 aColor;\n"
	"out vec3 ourColor;\n"
	"void main()\n"
	"{\n"
	"gl_Position = vec4(aPos, 1.0);\n"
	"ourColor = aColor;\n"
	"}0\n";

	const char* fragmentShader =
	"#version 400\n"
	"out vec4 FragColor;\n"
	"in vec3 ourColor;\n"
	"void main()\n"
	" {\n"
	"FragColor = vec4(ourColor, 1.0);\n"
	"}0\n";
	
	//Vertex Buffer Object
	GLuint pos_VBO;
	GLuint VAO;
	GLuint EBO;

	
	glGenBuffers(1, &pos_VBO);
	glGenBuffers(1, &EBO);
	glGenVertexArrays(1, &VAO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, pos_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	//position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	//colour
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);



	//creating the shaders from strings
	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, &vertexShader, NULL);
	glCompileShader(vs);
	
	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, &fragmentShader, NULL);
	glCompileShader(fs);

	//creating shader program
	GLuint myShader = glCreateProgram();
	glAttachShader(myShader, vs);
	glAttachShader(myShader, fs);
	glLinkProgram(myShader);

	glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
	
	while (!glfwWindowShouldClose(myWindow)) {
		// Poll for events from windows
		// clicks, key presses, closing, all that
		glfwPollEvents();
		
		float thisFrame = glfwGetTime();
		float deltaTime = thisFrame - prevFrame;
		Update(deltaTime);


		//draw vertices
		
		glUseProgram(myShader);
		glBindVertexArray(VAO);
		
		Draw(deltaTime);
		ImGuiNewFrame();
		DrawGui(deltaTime);
		ImGuiEndFrame();
		prevFrame = thisFrame;
		//glDrawArrays(GL_TRIANGLES, 0, 6);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		// Present our image to windows
		glfwSwapBuffers(myWindow);
	}
	UnloadContent();
	ShutdownImGui();
	Shutdown();
}

void Game::Update(float deltaTime)
{
}

void Game::Draw(float deltaTime)
{
	// Clear our screen every frame
	glClearColor(myClearColor.x, myClearColor.y, myClearColor.z, myClearColor.w);
	glClear(GL_COLOR_BUFFER_BIT);
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
}
