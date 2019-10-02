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
#define FRAMES_PER_SECOND 60
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

GameObject object1;
GameObject* activeObject; // This is initialized to object1

Path path;
float pointSize = 24.0f;

void GUI();

#include <algorithm>

// This is where we draw stuff
void DisplayCallbackFunction(void)
{
	// Set up scene
	TTK::Graphics::ClearScreen();
	TTK::Graphics::SetBackgroundColour(0.5f, 0.5f, 0.5f);
	TTK::Graphics::SetCameraMode2D(windowWidth, windowHeight);

	for (PointHandle& p : path.pointHandles)
	{
		p.draw();
	}

	if (path.pointHandles.size() > 1)
	{
		for (int i = 0; i < path.pointHandles.size() - 1; ++i)
		{
			TTK::Graphics::DrawLine(path.pointHandles[i].position, path.pointHandles[i + 1].position, glm::vec4(0.f, 0.f, 0.f, 1.f));
			for (int j = 0; j < 9; ++j)
			{
				// Draw a dotted line over each line
				glm::vec3 pointPosition = lerp(path.pointHandles[i].position, path.pointHandles[i + 1].position, (j + 1.0f) / 10.0f);
				TTK::Graphics::DrawPoint(pointPosition, 4.0f);
			}
		}
	}

	// Draw the object
	activeObject->draw();


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


	ImGui::Text("There are %i", path.pointHandles.size());
	ImGui::Text("Lerping between point %i and %i", path.index, path.index + 1);
	ImGui::SliderFloat("t", &path.t, 0.0f, 1.0f);
	ImGui::SliderFloat("Speed", &path.speed, 0.0f, 5.0f);
	ImGui::Text("Object Position: ");
	ImGui::Text("%f\t%f", activeObject->getPosition().x, activeObject->getPosition().y);

	/*
		Our camera sees everything from the origin point to windowWidth, windowHeight
		With an orthographic camera viewing the scene straight on, it's possible
		to convert the coordinates of objects onscreen to 0 - 1

		Important to know in the future for Computer Graphics!
	*/
	glm::vec2 screenSpacePosition;
	screenSpacePosition.x = inverseLerp(activeObject->getPosition().x, 0.0f, windowWidth);
	screenSpacePosition.y = inverseLerp(activeObject->getPosition().y, 0.0f, windowHeight);
	ImGui::Text("ScreenSpace Position: ");
	ImGui::Text("%f\t%f", screenSpacePosition.x, screenSpacePosition.y);

	ImGui::Text("Object Color: ");
	ImGui::Text("r\t\t\tg\t\t\tb");
	ImGui::Text("%f\t%f\t%f", activeObject->color.r, activeObject->color.g, activeObject->color.b);


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


void Update()
{
	path.update(deltaTime);
	activeObject->setScale(lerp(35.f, 65.f, path.t));
	activeObject->setPosition(path.position);

	// TODO: Set the red & green color of the object based on the position of the mouse
	// HINT: using inverseLerp(float, float, float) is essential for these
	// ANOTHER HINT: the position of the mouse will be between 0,0 and the window width/height
	activeObject->color.r = inverseLerp(mousePos.x, 0, windowWidth); // Plug in the correct parameters into the function
	activeObject->color.g = inverseLerp(mousePos.y, 0, windowHeight);
	activeObject->color.b = path.t;

	// Update the transformation
	activeObject->update(deltaTime);
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

	//if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	//	camera.moveForward();
	//if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	//	camera.moveBackward();
	//if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	//	camera.moveLeft();
	//if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	//	camera.moveRight();
	//if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
	//	camera.moveDown();
	//if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
	//	camera.moveUp();

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		isRunning = false;
}

/* function TimerCallbackFunction(int value)
* Description:
*  - this is called many times per second
*  - this enables you to animate things
*  - no drawing, just changing the state
*  - changes the frame number and calls for a redisplay
*  - FRAME_DELAY is the number of milliseconds to wait before calling the timer again
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

	if (mouseState.button[GLFW_MOUSE_BUTTON_LEFT] == GLFW_RELEASE)
		mouseDragActive = false;

	// Don't place an element if the mouse is hovering over imgui
	// This will help avoid unintended behaviour
	if (!ImGui::IsAnyWindowHovered())
	{
		// Only call these functions if the mouse is being pressed down, not up
		if (action == GLFW_PRESS)
		{
			bool canPlace;
			switch (button)
			{
				// Place a new point if the left mouse is pressed down
			case GLFW_MOUSE_BUTTON_LEFT:
				// Check if a point already exists in the same spot before adding a new point
				canPlace = true;
				for (int i = 0; i < path.pointHandles.size(); ++i)
					if (path.pointHandles[i].isInside(mousePos))
						canPlace = false;

				// if a point can be placed, add it to the list of points
				if (canPlace)
				{
					path.pointHandles.push_back(PointHandle(pointSize, mousePos, std::to_string(path.pointHandles.size())));
					path.computeDistances(); // Compute the distance between all points
				}
				break;
				// Delete a point if the right mouse is pressed down
			case GLFW_MOUSE_BUTTON_RIGHT:
				for (int i = 0; i < path.pointHandles.size(); ++i)
				{
					if (path.pointHandles[i].isInside(mousePos))
					{
						PointHandle::erasePoint(path.pointHandles, i);
						i = path.pointHandles.size();
					}
				}
				break;
			}
		}
	}
}

// Called when the mouse is clicked and moves
void MouseMotionCallbackFunction(GLFWwindow* window, double xpos, double ypos)
{
	ImGui::GetIO().MousePos = ImVec2((float)xpos, (float)ypos);

	mousePos.x = xpos;
	mousePos.y = ypos;

	/*
		The mouse motion callback function is called whenever any mouse button is clicked and moving
		We want only want to move points when holding down left click however
		Use a MouseClass state manager to remember what buttons are held down

		Loop through the list to find a point inside the range of the mouse
		We loop backwards so that recent points placed have a higher priority
		Tracking the last point moved allows the point to moved over other
		points without them stealing attention
	*/
	static int lastPointMoved = 0;
	if (!ImGui::IsAnyWindowHovered() && mouseState.button[GLFW_MOUSE_BUTTON_LEFT] == GLFW_PRESS && path.pointHandles.size() > 0)
	{
		if (mouseDragActive)
		{
			path.pointHandles[lastPointMoved].position = mousePos;
		}
		else
			for (int i = path.pointHandles.size() - 1; i >= 0; --i)
			{
				if (path.pointHandles[i].isInside(mousePos))
				{

					lastPointMoved = i;
					mouseDragActive = true;
					path.pointHandles[i].position = mousePos;
					break;
				}
			}
	}
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
	window = glfwCreateWindow(windowWidth, windowHeight, "Hello!", nullptr, nullptr);

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

	activeObject = &object1;

	// Init IMGUI
	TTK::Graphics::InitImGUI(window);


	while (!glfwWindowShouldClose(window) && isRunning) {

		// Handle the change in frames
		TimerCallbackFunction(0);

		// Poll for events from windows (clicks, keypressed, closing, all that)
		glfwPollEvents();

		Update();

		DisplayCallbackFunction();
	}


	TTK::Graphics::ShutdownImGUI();
	TTK::Graphics::Cleanup();
	Logger::Uninitialize();

	glfwTerminate();

	return 0;
}



