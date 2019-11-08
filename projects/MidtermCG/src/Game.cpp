#include "Game.h"
#include <stdexcept>
#define IMGUI_IMPL_OPENGL_LOADER_GLAD
#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_glfw.h"
#include "Logging.h"
#include <GLM/gtc/matrix_transform.hpp>
#include <GLM/gtc/type_ptr.hpp>

#define HEIGHT 800.0f
#define WIDTH 800.0f
void GlfwWindowResizedCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

Game::Game():
		myWindow(nullptr),
		myWindowTitle("Game"),
		myClearColor(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f))
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

	// paddle is white so i'm giving it it's own data
	Vertex vertices[4] = {
		//       Position                   Color
		//    x      y     z         r    g     b     a
		{{  100.0f,  15.0f, 0.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }},
		{{  0.0f,  15.0f, 0.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }},
		{{  100.0f,  0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }},
		{{  0.0f,  0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }},
	};


	//10 pixels by 10 pixels for the ball
	Vertex verticesball[4] = {
		//       Position                   Color
		//    x      y     z         r    g     b     a
		{{  10.0f, 10.0f, 0.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }},
		{{  0.0f,  10.0f, 0.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }}, 
		{{  10.0f, 0.0f, 0.0f },  { 1.0f, 1.0f, 1.0f, 1.0f }},
		{{  0.0f,  0.0f, 0.0f },  { 1.0f, 1.0f, 1.0f, 1.0f }},
	};

	Vertex verticesbrick[4] = {
		//       Position                   Color
		//    x      y     z         r    g     b     a
		{{  50.0f, 25.0f, 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f }},
		{{  0.0f,  25.0f, 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f }},
		{{  50.0f, 0.0f, 0.0f },  { 1.0f, 0.0f, 0.0f, 1.0f }},
		{{  0.0f,  0.0f, 0.0f },  { 1.0f, 0.0f, 0.0f, 1.0f }},
	};

	// Create our 6 indices
	uint32_t indices[6] = {
		0, 1, 2,
		2, 1, 3
	};


	//sooo the bad practice begins

	ballMesh = std::make_shared<Mesh>(verticesball, 4, indices, 6);
	ball.mesh = ballMesh;
	ball.model = glm::mat4(1.0f);
	ball.position.x = ball.model[3][0];
	ball.position.y = ball.model[3][1];
	ball.model = glm::translate(ball.model, glm::vec3(395.0f, 300.0f, 0.0f));
	ball.size = glm::vec2(10.0f, 10.0f);
	ball.speed = 500.0f;
	ball.direction = glm::normalize(glm::vec3(0.0f, 1.0f, 0.0f));

	paddleMesh = std::make_shared<Mesh>(vertices, 4, indices, 6);
	paddle.mesh = paddleMesh;
	paddle.model = glm::mat4(1.0f);
	paddle.position.x = ball.model[3][0];
	paddle.position.y = ball.model[3][1];
	paddle.model = glm::translate(paddle.model, glm::vec3(395.0f, 690.0f, 0.0f));
	paddle.size = glm::vec2(100.0f, 15.0f);
	paddle.speed = 400.0f;

	brickMesh = std::make_shared<Mesh>(verticesbrick, 4, indices, 6);
	
		for (int i = 0; i < 16; i++)
			for (int k = 0; k < 3; k++)
			{
				brick[i][k] = marcos::GameObject(brickMesh);
				brick[i][k].mesh = brickMesh;
				brick[i][k].model = glm::mat4(1.0f);
				brick[i][k].size = glm::vec2(50.0f, 25.0f);
				brick[i][k].model = glm::translate(brick[i][k].model, glm::vec3(brick[i][k].size.x * (i), brick[i][k].size.y * (k), 0.0f));
				brick[i][k].position.x = brick[i][k].model[3][0];
				brick[i][k].position.y = brick[i][k].model[3][1];
				brick[i][k].speed = 0.0f;
			}
		

	
	myShader = std::make_shared<Shader>();
	myShader->Load("source.vert", "source.frag");
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
		ImGui::StyleColorsClassic();
		// Get our imgui style
		ImGuiStyle& style = ImGui::GetStyle();
		//style.Alpha = 1.0f;
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
			style.WindowRounding = 10.0f;
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

		float thisFrame = glfwGetTime();
		float deltaTime = thisFrame - prevFrame;
		

		//Physics updates
		Update(deltaTime);
		updateCollisions();
		
		//draw vertices
		Draw(deltaTime);
		
		//check to see if we should be passing in the ortho matrix or the perspective matrix
		myCamera->SetProjection(glm::ortho(0.0f, WIDTH, HEIGHT, 0.0f, -1.0f, 1.0f));
		
		//set projection matrix
		myShader->Use();
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
	if (glfwGetKey(myWindow, GLFW_KEY_A) == GLFW_PRESS)
		ball.model = glm::translate(ball.model, glm::vec3(-1.0f * ball.speed * deltaTime, 0.0f, 0.0f));
	if (glfwGetKey(myWindow, GLFW_KEY_D) == GLFW_PRESS)
		ball.model = glm::translate(ball.model, glm::vec3(1.0f * ball.speed * deltaTime, 0.0f, 0.0f));
	if (glfwGetKey(myWindow, GLFW_KEY_W) == GLFW_PRESS)
		ball.model = glm::translate(ball.model, glm::vec3(0.0f, -1.0f * ball.speed * deltaTime, 0.0f));
	if (glfwGetKey(myWindow, GLFW_KEY_S) == GLFW_PRESS)
		ball.model = glm::translate(ball.model, glm::vec3(0.0f, 1.0f * ball.speed * deltaTime, 0.0f));
	
	if (glfwGetKey(myWindow, GLFW_KEY_LEFT) == GLFW_PRESS)
		paddle.model = glm::translate(paddle.model, glm::vec3(-1.0f * paddle.speed * deltaTime, 0.0f, 0.0f));
	if (glfwGetKey(myWindow, GLFW_KEY_RIGHT) == GLFW_PRESS)
		paddle.model = glm::translate(paddle.model, glm::vec3(1.0f * paddle.speed * deltaTime, 0.0f, 0.0f));


	//updating model positions
	
	ball.position.x = ball.model[3][0];
	ball.position.y = ball.model[3][1];
	ball.model = glm::translate(ball.model, glm::vec3(ball.direction.x * ball.speed * deltaTime, ball.direction.y * ball.speed * deltaTime, 0.0f));

	paddle.position.x = paddle.model[3][0];
	paddle.position.y = paddle.model[3][1];
	
	
}

void Game::Draw(float deltaTime)
{
	// Clear our screen every frame
	glClearColor(myClearColor.x, myClearColor.y, myClearColor.z, myClearColor.w);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	myShader->Use();
	myShader->setMat4("model", ball.model);
	ball.mesh->Draw();
	myShader->setMat4("model", paddle.model);
	paddle.mesh->Draw();

	for(int i = 0 ; i < 16 ; i++)
		for(int j = 0; j < 3 ; j++)
		{
		myShader->setMat4("model", brick[i][j].model);
		brick[i][j].mesh->Draw();
		}
}

void Game::DrawGui(float deltaTime)
{
	// Open a new ImGui window
	ImGui::Text("Score: %i", score);
	ImGui::Text("Lives: %i", lives);

}

enum Direction
{
	Up,
	Right,
	Left,
	Down
};

Direction getCollisionDirection()
{
	return (Direction)1;

	//need to get the point of impact and see if it is higher, lower, right or left of the center of the brick
	
};

void Game::updateCollisions()
{
	//bouncing the ball of the sides of the screen
	if (ball.position.x <= 0)
	{
		ball.model = glm::translate(ball.model, glm::vec3(15.0f, 0.0f, 0.0f));
		ball.direction.x *= -1;
	}
	if (ball.position.x >= WIDTH - 10)
	{
		ball.model = glm::translate(ball.model, glm::vec3(-15.0f, 0.0f, 0.0f));
		ball.direction.x *= -1;
	}
	if (ball.position.y <= 0)
	{
		ball.model = glm::translate(ball.model, glm::vec3(0.0f, 10.0, 0.0f));
		ball.direction.y = glm::abs(ball.direction.y);
	}
	if(ball.position.y > HEIGHT)
	{
		glm::vec3 translation(395.0f -ball.position.x, 300.0f - ball.position.y, 0.0f);
		ball.model = glm::translate(ball.model, translation);

		ball.position.x = ball.model[3][0];
		ball.position.y = ball.model[3][1];
		
		lives--;
		if(lives <= 0) 
		{
			exit(1); //not going for creativity here
		}
	}
	
	if(testCollisions(paddle, ball))
	{
		//changing the angle the ball bounces based on where the paddle hits it
		//shouldn't really be making a new set on every collision but eh
		glm::vec3 paddleCenter = glm::vec3(paddle.position.x + paddle.size.x / 2, paddle.position.y + paddle.size.y / 2, 0.0f);
		glm::vec3 ballCenter = glm::vec3(ball.position.x + ball.size.x / 2, ball.position.y + ball.size.y / 2, 0.0f);
		glm::vec3 result = glm::normalize(paddleCenter - ballCenter);

		ball.model = glm::translate(ball.model, glm::vec3(0.0f, -15.0f, 0.0f));
		ball.direction = glm::normalize(glm::vec3(ball.direction.x + result.x, -1, 0.0f));
	};

	for(int i = 0; i < 16 ; i++)
		for(int j = 0; j < 3 ; j++)
		{
			if(testCollisions(brick[i][j], ball))
			{

				score += 10;
				
				brick[i][j].model = glm::translate(brick[i][j].model, glm::vec3(1000.0f, 1000.0f, 1000.0f));
				brick[i][j].position.x = brick[i][j].model[3][0];
				brick[i][j].position.y = brick[i][j].model[3][1];
				ball.model = glm::translate(ball.model, glm::vec3(0.0f, 10.0f, 0.0f));
				ball.direction = glm::vec3(ball.direction.x, ball.direction.y * -1, 0.0f);

				//same with these, the gameobject should store and update these in some kind of GameObject->update()
				glm::vec3 brickCenter = glm::vec3(brick[i][j].position.x + brick[i][j].size.x / 2, brick[i][j].position.y + brick[i][j].size.y / 2, 0.0f);
				glm::vec3 ballCenter = glm::vec3(ball.position.x + ball.size.x / 2, ball.position.y + ball.size.y / 2, 0.0f);
				glm::vec3 result = brickCenter - ballCenter;
			}
		}
	
}

bool Game::testCollisions(marcos::GameObject obj1, marcos::GameObject obj2)
{
	//AABB collision checking
	bool collisionX = obj1.position.x + obj1.size.x >= obj2.position.x &&
		obj2.position.x + obj2.size.x >= obj1.position.x;

	bool collisionY = obj1.position.y + obj1.size.y >= obj2.position.y &&
		obj2.position.y + obj2.size.y >= obj1.position.y;

	return collisionX && collisionY;
}
