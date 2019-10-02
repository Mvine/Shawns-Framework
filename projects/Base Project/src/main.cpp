// Core Libraries (std::)
#include <iostream>
#include <string>
#include <math.h>

// 3rd Party Libraries
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <TTK/GraphicsUtils.h>
#include <TTK/Camera.h>
#include <imgui.h>
#include "Logging.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "GLFW/glfw3.h"


#include "GameObject.h"
#include "PointHandle.h"
#include "MathHelpers.h"
#include "Path.h"

// Defines and Core variables
#ifndef FRAMES_PER_SECOND
	#define FRAMES_PER_SECOND 60
#endif
constexpr int FRAME_DELAY = 1000 / FRAMES_PER_SECOND; // Milliseconds per frame

// Window size
GLFWwindow* window;
int windowWidth = 800;
int windowHeight = 600;

// Angle conversions conversions
constexpr float degToRad = 3.14159f / 180.0f;
constexpr float radToDeg = 180.0f / 3.14159f;

double deltaTime = 0.0; // amount of time since last update (set every frame in timer callback)
double totalTime = 0.0;

// Mouse position in pixels
glm::dvec3 mousePos, lastMousePos; // x,y,0
bool mouseDragActive = false;

bool isRunning = true;

TTK::Camera camera;

void GUI();

#include <algorithm>

// This is where we draw stuff
void Display()
{
	// Set up scene
	TTK::Graphics::ClearScreen();
	TTK::Graphics::SetBackgroundColour(0.5f, 0.5f, 0.5f);
	TTK::Graphics::SetCameraMode2D(windowWidth, windowHeight);

#ifdef _DEBUG
	GUI();
#endif

	// Swap buffers
	// This is how we tell the program to put the things we just drew on the screen
	TTK::Graphics::EndFrame();

	// Present our image to windows
	glfwSwapBuffers(window);
}

void GUI()
{
	// You must call this prior to using any imgui functions
	ImGuiIO& io = ImGui::GetIO();
	io.DisplaySize.x = windowWidth;
	io.DisplaySize.y = windowHeight;

	// Implementation new frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	// ImGui context new frame
	ImGui::NewFrame();

	// Set GUI
	ImGui::Text("Hello World");

	// Cache the active window
	GLFWwindow* window = glfwGetCurrentContext();

	// Make sure ImGui knows how big our window is
	//ImGuiIO& io = ImGui::GetIO();
	int width{ 0 }, height{ 0 };
	glfwGetWindowSize(window, &width, &height);
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
		glfwMakeContextCurrent(window);
	}
}

/* function void KeyboardCallbackFunction(unsigned char, int,int)
* Description:
*   - this is where you should call update functions for game objects
*/
void Update()
{

}


/* function void KeyboardCallbackFunction(unsigned char, int,int)
* Description:
*   - this handles keyboard input when a button is pressed
*/
void KeyboardCallbackFunction(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	ImGuiIO& io = ImGui::GetIO();
	io.KeysDown[key] = (action == GLFW_PRESS);
	// Example of how to use keyboard input

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		isRunning = false;
}

/* function TimerCallbackFunction(int value)
* Description:
*  - this is called many times per second
*  - this enables you to animate things
*  - calculates the time between frames
*/
void TimerCallbackFunction(int value)
{
	// Calculate the amount of time since the last frame
	static double elapsedTimeAtLastTick = 0;
	double totalElapsedTime = glfwGetTime();

	deltaTime = totalElapsedTime - elapsedTimeAtLastTick;
	elapsedTimeAtLastTick = totalElapsedTime;
	totalTime = totalElapsedTime;
}

/* function WindowReshapeCallbackFunction()
* Description:
*  - this is called whenever the window is resized
*  - and sets up the projection matrix properly
*/
void WindowReshapeCallbackFunction(GLFWwindow* window, int w, int h)
{
	/* Update our Window Properties */
	windowWidth = w;
	windowHeight = h;
}

class MouseState
{
public:
	union
	{
		struct
		{
			int leftClick;
			int middleClick;
			int rightClick;
			int scrollUp;
			int scrollDown;
		};
		int button[5];
	};
};
MouseState mouseState;

// This is called when a mouse button is clicked
void MouseClickCallbackFunction(GLFWwindow* window, int button, int action, int mods)
{
	// ImGui states are inverted, so we must invert GLUT's state 
	// Currently it interprets any click as a left click for imgui
	// ImGui has a slightly different order for its buttons
	ImGui::GetIO().MouseDown[0] = !action;
	mouseState.button[button] = action;

	// Do things with mouse clicks
	// ex. if (mouseState.button[GLFW_MOUSE_BUTTON_LEFT] == GLFW_RELEASE)
}

// Called when the mouse is clicked and moves
void MouseMotionCallbackFunction(GLFWwindow* window, double xpos, double ypos)
{
	ImGui::GetIO().MousePos = ImVec2((float)xpos, (float)ypos);

	mousePos.x = xpos;
	mousePos.y = ypos;
}


/* function main()
* Description:
*  - this is the main function
*  - does initialization and then calls glutMainLoop() to start the event handler
*/
int main(int argc, char** argv)
{
	// Memory Leak Detection
#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	Logger::Init();

	// Initialize GLFW
	if (glfwInit() == GLFW_FALSE) {
		std::cout << "Failed to initialize Glad" << std::endl;
		return 1;
	}

	// Create a new GLFW window
	window = glfwCreateWindow(windowWidth, windowHeight, "New Window", nullptr, nullptr);

	// We want GL commands to be executed for our window, so we make our window's context the current one
	glfwMakeContextCurrent(window);

	// Let glad know what function loader we are using (will call gl commands via glfw)
	if (gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) == 0) {
		std::cout << "Failed to initialize Glad" << std::endl;
		return 2;
	}

	// Display our GPU and OpenGL version
	std::cout << glGetString(GL_RENDERER) << std::endl;
	std::cout << glGetString(GL_VERSION) << std::endl;

	// Init GL
	TTK::Graphics::SetCameraMode3D(windowWidth, windowHeight);
	glfwSetWindowSizeCallback(window, [](GLFWwindow* window, int width, int height) {
		TTK::Graphics::SetCameraMode3D(width, height);
		});
	TTK::Graphics::SetBackgroundColour(0.5f, 0.5f, 0.5f);
	TTK::Graphics::SetDepthEnabled(true);
	glDepthFunc(GL_LEQUAL);

	// Set GLFW callbacks
	glfwSetKeyCallback(window, KeyboardCallbackFunction);
	glfwSetCursorPosCallback(window, MouseMotionCallbackFunction);
	glfwSetMouseButtonCallback(window, MouseClickCallbackFunction);
	glfwSetWindowSizeCallback(window, WindowReshapeCallbackFunction);

	/* Start Game Loop */
	deltaTime = glfwGetTime();
	deltaTime /= 1000.0f;

	// Init IMGUI
	TTK::Graphics::InitImGUI(window);


	while (!glfwWindowShouldClose(window) && isRunning) {

		// Handle the change in frames
		TimerCallbackFunction(0);

		// Poll for events from windows (clicks, keypressed, closing, all that)
		glfwPollEvents();

		Update();

		Display();
	}


	TTK::Graphics::ShutdownImGUI();
	TTK::Graphics::Cleanup();
	Logger::Uninitialize();

	glfwTerminate();

	return 0;
}