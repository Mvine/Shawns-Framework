#include "Game.h"
#include "Logging.h"

#include <stdexcept>

#define IMGUI_IMPL_OPENGL_LOADER_GLAD
#include "imgui.h"
#include "imgui_impl_opengl3.cpp"
#include "imgui_impl_glfw.cpp"

#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp>

#include "SceneManager.h"
#include "MeshRenderer.h"
#include "Material.h"

#include "Texture2D.h"
#include "ObjLoader.h"

#include "Transform.h"

#include <functional>

struct UpdateBehaviour {
	std::function<void(entt::entity e, float dt)> Function;
};

/*
	Handles debug messages from OpenGL
	https://www.khronos.org/opengl/wiki/Debug_Output#Message_Components
	@param source    Which part of OpenGL dispatched the message
	@param type      The type of message (ex: error, performance issues, deprecated behavior)
	@param id        The ID of the error or message (to distinguish between different types of errors, like nullref or index out of range)
	@param severity  The severity of the message (from High to Notification)
	@param length    The length of the message
	@param message   The human readable message from OpenGL
	@param userParam The pointer we set with glDebugMessageCallback (should be the game pointer)
*/
void GlDebugMessage(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
	switch (severity) {
	case GL_DEBUG_SEVERITY_LOW:          LOG_INFO(message); break;
	case GL_DEBUG_SEVERITY_MEDIUM:       LOG_WARN(message); break;
	case GL_DEBUG_SEVERITY_HIGH:         LOG_ERROR(message); break;
#ifdef LOG_GL_NOTIFICATIONS
	case GL_DEBUG_SEVERITY_NOTIFICATION: LOG_INFO(message); break;
#endif
	default: break;
	}
}

//Mouse Callback Prototype
void mouseClickCallback(GLFWwindow* window, int button, int action, int mods);

//Global Variables
double mousePosX, mousePosY;

std::unordered_map<int, Camera::Sptr> cameraMap;

void GlfwWindowResizedCallback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
	Game* game = (Game*)glfwGetWindowUserPointer(window);
	if (game) {
		game->Resize(width, height);
	}
}

void Game::Resize(int newWidth, int newHeight) {
	myCamera->Projection = glm::perspective(glm::radians(60.0f), newWidth / (float)newHeight, 0.01f, 1000.0f);
	myWindowSize = { newWidth, newHeight };
}


Game::Game() :
	myWindow(nullptr),
	myWindowTitle("Game"),
	myClearColor(glm::vec4(0.3, 0.3, 0.3, 1)),
	myModelTransform(glm::mat4(1)),
	myWindowSize(800, 800)
{ }

Game::~Game() { }

void Game::Run()
{
	Initialize();
	InitImGui();

	LoadContent();

	static float prevFrame = glfwGetTime();

	// Run as long as the window is open
	while (!glfwWindowShouldClose(myWindow)) {
		// Poll for events from windows (clicks, keypressed, closing, all that)
		glfwPollEvents();

		float thisFrame = glfwGetTime();
		float deltaTime = thisFrame - prevFrame;

		Update(deltaTime);
		Draw(deltaTime);

		ImGuiNewFrame();
		DrawGui(deltaTime);
		ImGuiEndFrame();

		// Store this frames time for the next go around
		prevFrame = thisFrame;

		// Present our image to windows
		glfwSwapBuffers(myWindow);
	}

	LOG_INFO("Shutting down...");

	UnloadContent();

	ShutdownImGui();
	Shutdown();
}

void Game::Initialize() {
	// Initialize GLFW
	if (glfwInit() == GLFW_FALSE) {
		std::cout << "Failed to initialize GLFW" << std::endl;
		throw std::runtime_error("Failed to initialize GLFW");
	}

	// Enable transparent backbuffers for our windows (note that Windows expects our colors to be pre-multiplied with alpha)
	glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, true);

	// Create a new GLFW window
	myWindow = glfwCreateWindow(myWindowSize.x, myWindowSize.y, myWindowTitle, nullptr, nullptr);

	// Tie our game to our window, so we can access it via callbacks
	glfwSetWindowUserPointer(myWindow, this);

	// Set our window resized callback
	glfwSetWindowSizeCallback(myWindow, GlfwWindowResizedCallback);

	// We want GL commands to be executed for our window, so we make our window's context the current one
	glfwMakeContextCurrent(myWindow);

	// Let glad know what function loader we are using (will call gl commands via glfw)
	if (gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) == 0) {
		std::cout << "Failed to initialize Glad" << std::endl;
		throw std::runtime_error("Failed to initialize GLAD");
	}

	// Log our renderer and OpenGL version
	LOG_INFO(glGetString(GL_RENDERER));
	LOG_INFO(glGetString(GL_VERSION));

	// Enable debugging, and route messages to our callback
	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(GlDebugMessage, this);

	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	glEnable(GL_SCISSOR_TEST);
}

void Game::Shutdown() {
	glfwTerminate();
}

Mesh::Sptr MakeSubdividedPlane(float size, int numSections, bool worldUvs = true) {
	LOG_ASSERT(numSections > 0, "Number of sections must be greater than 0!");
	LOG_ASSERT(size != 0, "Size cannot be zero!");
	// Determine the number of edge vertices, and the number of vertices and indices we'll need
	int numEdgeVerts = numSections + 1;
	size_t vertexCount = numEdgeVerts * numEdgeVerts;
	size_t indexCount = numSections * numSections * 6;
	// Allocate some memory for our vertices and indices
	Vertex* vertices = new Vertex[vertexCount];
	uint32_t* indices = new uint32_t[indexCount];
	// Determine where to start vertices from, and the step pre grid square
	float start = -size / 2.0f;
	float step = size / numSections;
	// Iterate over the grid's edge vertices
	for (int ix = 0; ix <= numSections; ix++) {
		for (int iy = 0; iy <= numSections; iy++) {
			// Get a reference to the vertex so we can modify it
			Vertex& vert = vertices[ix * numEdgeVerts + iy];
			// Set its position
			vert.Position.x = start + ix * step;
			vert.Position.y = start + iy * step;
			vert.Position.z = 0.0f;
			// Set its normal
			vert.Normal = glm::vec3(0, 0, 1);
			// The UV will go from [0, 1] across the entire plane (can change this later)
			if (worldUvs) {
				vert.UV.x = vert.Position.x;
				vert.UV.y = vert.Position.y;
			}
			else {
				vert.UV.x = vert.Position.x / size;
				vert.UV.y = vert.Position.y / size;
			}
			// Flat white color
			vert.Color = glm::vec4(1.0f);
		}
	}
	// We'll just increment an index instead of calculating it
	uint32_t index = 0;
	// Iterate over the quads that make up the grid
	for (int ix = 0; ix < numSections; ix++) {
		for (int iy = 0; iy < numSections; iy++) {
			// Determine the indices for the points on this quad
			uint32_t p1 = (ix + 0) * numEdgeVerts + (iy + 0);
			uint32_t p2 = (ix + 1) * numEdgeVerts + (iy + 0);
			uint32_t p3 = (ix + 0) * numEdgeVerts + (iy + 1);
			uint32_t p4 = (ix + 1) * numEdgeVerts + (iy + 1);
			// Append the quad to the index list
			indices[index++] = p1;
			indices[index++] = p2;
			indices[index++] = p3;
			indices[index++] = p3;
			indices[index++] = p2;
			indices[index++] = p4;
		}
	}
	// Create the result, then clean up the arrays we used
	Mesh::Sptr result = std::make_shared<Mesh>(vertices, vertexCount, indices, indexCount);
	delete[] vertices;
	delete[] indices;
	// Return the result
	return result;
}


Mesh::Sptr MakeInvertedCube() {
	// Create our 4 vertices
	Vertex verts[8] = {
		// Position
		// x y z
		{{ -1.0f, -1.0f, -1.0f }}, {{ 1.0f, -1.0f, -1.0f }}, {{ -1.0f, 1.0f, -1.0f }}, {{ 1.0f, 1.0f, -1.0f }},
		{{ -1.0f, -1.0f, 1.0f }}, {{ 1.0f, -1.0f, 1.0f }}, {{ -1.0f, 1.0f, 1.0f }}, {{ 1.0f, 1.0f, 1.0f }}
	};
	// Create our 6 indices
	uint32_t indices[36] = {
	0, 1, 2, 2, 1, 3, 4, 6, 5, 6, 7, 5, // bottom / top
	0, 1, 4, 4, 1, 5, 2, 3, 6, 6, 3, 7, // front /back
	2, 4, 0, 2, 6, 4, 3, 5, 1, 3, 7, 5 // left / right
	};
	// Create a new mesh from the data
	return std::make_shared<Mesh>(verts, 8, indices, 36);
}

glm::vec4 testColor = glm::vec4(1.0f, 0.0f, 1.0f, 1.0f);


void Game::LoadContent() {

	//Active camera for viewport selection, while leaving the original camera where it was
	activeCamera = std::make_shared<Camera>();
	activeCamera->SetPosition(glm::vec3(5, 5, 5));
	activeCamera->LookAt(glm::vec3(0), glm::vec3(0, 0, 1));
	activeCamera->Projection = glm::perspective(glm::radians(60.0f), 1.0f, 0.01f, 1000.0f);

	//Top Left (Ortho Top)
	Camera2 = std::make_shared<Camera>();
	Camera2->SetPosition(glm::vec3(0, 0, 15));
	Camera2->LookAt(glm::vec3(0), glm::vec3(0, 1, 0));
	Camera2->drawMode = Camera::wireframe;
	Camera2->Projection = glm::ortho(-5.0f, 5.0f, -5.0f, 5.0f, 0.0f, 100.0f);

	//Bottom Left (Ortho Left)
	myCamera = std::make_shared<Camera>();
	myCamera->SetPosition(glm::vec3(0, -15, -0.5));
	myCamera->LookAt(glm::vec3(0), glm::vec3(0, 0, 1));
	myCamera->drawMode = Camera::wireframe;
	myCamera->Projection = glm::ortho(-5.0f, 5.0f, 5.0f, -5.0f, 0.0f, 100.0f);

	//Top Right (Ortho Front)
	Camera4 = std::make_shared<Camera>();
	Camera4->SetPosition(glm::vec3(15, 0, 0));
	Camera4->LookAt(glm::vec3(0), glm::vec3(0, 0, 1));
	Camera4->drawMode = Camera::wireframe;
	Camera4->Projection = glm::ortho(-5.0f, 5.0f, -5.0f, 5.0f, 0.0f, 100.0f);

	//Bottom Right (Perspective)
	Camera3 = std::make_shared<Camera>();
	Camera3->SetPosition(glm::vec3(-10, 16, 13));
	Camera3->LookAt(glm::vec3(0), glm::vec3(0, 0, 1));
	Camera3->Projection = glm::perspective(glm::radians(60.0f), 1.0f, 0.01f, 1000.0f);

	cameraMap[0] = activeCamera;
	cameraMap[1] = Camera2;
	cameraMap[2] = myCamera;
	cameraMap[3] = Camera4;
	cameraMap[4] = Camera3;

	// Create our 4 vertices
	Vertex vertices[4] = {
		//       Position                   Color                      Normal
		//    x      y     z         r    g     b     a            x      y     z      u     v
		{{ -10.0f, -10.0f, 0.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 1.0f }, { 1.0f, 0.0f }},
		{{  10.0f, -10.0f, 0.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f }},
		{{ -10.0f,  10.0f, 0.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 1.0f }, { 1.0f, 1.0f }},
		{{  10.0f,  10.0f, 0.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 1.0f }}
	};

	// Create our 6 indices
	uint32_t indices[6] = {
		0, 1, 2,
		2, 1, 3
	};

	// Create a new mesh from the data
	Mesh::Sptr myMesh = std::make_shared<Mesh>(vertices, 4, indices, 6);

	//Mesh::Sptr monkey = ObjLoader::LoadObjToMesh("monkey.obj");

	Shader::Sptr phong = std::make_shared<Shader>();
	phong->Load("lighting.vs.glsl", "textured-blinn-phong.fs.glsl");

	Texture2D::Sptr albedo = Texture2D::LoadFromFile("color-grid.png");

	SamplerDesc description = SamplerDesc();
	description.MinFilter = MinFilter::LinearMipNearest;
	description.MagFilter = MagFilter::Linear;
	description.WrapS = description.WrapT = WrapMode::Repeat;
	TextureSampler::Sptr Linear = std::make_shared<TextureSampler>(description);

	Material::Sptr testMat = std::make_shared<Material>(phong);
	testMat->Set("a_LightPos", { 2, 0, 4 });
	testMat->Set("a_LightColor", { 1.0f, 1.0f, 1.0f });
	testMat->Set("a_AmbientColor", { 1.0f, 1.0f, 1.0f });
	testMat->Set("a_AmbientPower", 0.1f);
	testMat->Set("a_LightSpecPower", 0.5f);
	testMat->Set("a_LightShininess", 256.0f);
	testMat->Set("a_LightAttenuation", 1.0f / 100.0f);
	// Previously testMat->Set("s_Albedo", albedo, Linear);
	testMat->Set("s_Albedos[0]", Texture2D::LoadFromFile("grass.jpg"), Linear);
	testMat->Set("s_Albedos[1]", Texture2D::LoadFromFile("moss.jpg"), Linear);
	testMat->Set("s_Albedos[2]", Texture2D::LoadFromFile("brick.jpg"), Linear);


	SceneManager::RegisterScene("Test");
	SceneManager::RegisterScene("Test2");
	SceneManager::SetCurrentScene("Test");

	auto scene = CurrentScene();

	scene->SkyboxShader = std::make_shared<Shader>();
	scene->SkyboxShader->Load("cubemap.vs.glsl", "cubemap.fs.glsl");
	scene->SkyboxMesh = MakeInvertedCube();

	std::string files[6] = {
		std::string("cubemap/graycloud_lf.jpg"),
		std::string("cubemap/graycloud_rt.jpg"),
		std::string("cubemap/graycloud_dn.jpg"),
		std::string("cubemap/graycloud_up.jpg"),
		std::string("cubemap/graycloud_ft.jpg"),
		std::string("cubemap/graycloud_bk.jpg")
	};
	scene->Skybox = TextureCube::LoadFromFiles(files);

	{ // Push a new scope so that we don't step on other names
		Shader::Sptr mountainShader = std::make_shared<Shader>();
		mountainShader->Load("mountainVertex.vs.glsl", "mountainFragment.fs.glsl");
		Material::Sptr testMat = std::make_shared<Material>(mountainShader);

		// Format is: [xDir, yDir, "steepness", wavelength] (note that the sum of steepness should be < 1 to avoid loops)

		testMat->Set("s_Environment", scene->Skybox);
		testMat->HasTransparency = true;
		auto& ecs = GetRegistry("Test"); // If you've changed the name of the scene, you'll need to modify this!
		entt::entity e1 = ecs.create();
		MeshRenderer& m1 = ecs.assign<MeshRenderer>(e1);
		m1.Material = testMat;
		m1.Mesh = MakeSubdividedPlane(20.0f, 100);
	}

	{ // Push a new scope so that we don't step on other names
		Shader::Sptr waterShader = std::make_shared<Shader>();
		waterShader->Load("water-shader.vs.glsl", "water-shader.fs.glsl");
		Material::Sptr testMat = std::make_shared<Material>(waterShader);
		testMat->Set("a_EnabledWaves", 3);
		testMat->Set("a_Gravity", 9.81f);
		// Format is: [xDir, yDir, "steepness", wavelength] (note that the sum of steepness should be < 1 to avoid loops)
		testMat->Set("a_Waves[0]", { 0.1f, 0.0f, 0.20f, 6.0f });
		testMat->Set("a_Waves[1]", { 0.0f, 0.1f, 0.15f, 3.1f });
		testMat->Set("a_Waves[2]", { 0.1f, 1.4f, 0.10f, 1.8f });
		testMat->Set("a_WaterAlpha", 0.75f);
		testMat->Set("a_WaterColor", { 0.8f, 1.0f, 0.95f });
		testMat->Set("a_WaterClarity", 0.9f);
		testMat->Set("a_FresnelPower", 0.5f);
		testMat->Set("a_RefractionIndex", 1.0f / 1.34f);
		testMat->Set("s_Environment", scene->Skybox);
		testMat->HasTransparency = true;
		auto& ecs = GetRegistry("Test"); // If you've changed the name of the scene, you'll need to modify this!
		entt::entity e1 = ecs.create();
		MeshRenderer& m1 = ecs.assign<MeshRenderer>(e1);
		m1.Material = testMat;
		m1.Mesh = MakeSubdividedPlane(20.0f, 100);
	}

	glfwSetMouseButtonCallback(myWindow, mouseClickCallback);
}


void Game::UnloadContent() {

}

void Game::InitImGui() {
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

void Game::ShutdownImGui() {
	// Cleanup the ImGui implementation
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	// Destroy our ImGui context
	ImGui::DestroyContext();
}

void Game::ImGuiNewFrame() {
	// Implementation new frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	// ImGui context new frame
	ImGui::NewFrame();
}

void Game::ImGuiEndFrame() {
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

void Game::Update(float deltaTime) {
	glm::vec3 movement = glm::vec3(0.0f);
	glm::vec3 rotation = glm::vec3(0.0f);

	float speed = 10.0f;
	float rotSpeed = 1.0f;

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
	for (int i = 0; i < cameraMap.size(); i++)
	{
		if (cameraMap[i]->isSelected)
		{
			cameraMap[i]->Rotate(rotation);
			cameraMap[i]->Move(movement);
		}
	}

	//Polling mouse position for window selection
	glfwGetCursorPos(myWindow, &mousePosX, &mousePosY);

	// Rotate our transformation matrix a little bit each frame
	myModelTransform = glm::rotate(myModelTransform, deltaTime, glm::vec3(0, 0, 1));

	auto view = CurrentRegistry().view<UpdateBehaviour>();
	for (const auto& e : view) {
		auto& func = CurrentRegistry().get<UpdateBehaviour>(e);
		if (func.Function) {
			func.Function(e, deltaTime);
		}
	}
}

void Game::Draw(float deltaTime) {

	glm::ivec4 viewport = {
	0,0,
	myWindowSize.x / 2, myWindowSize.y / 2 };

	glm::ivec4 viewport2 = {
	0,myWindowSize.y / 2,
	myWindowSize.x / 2, myWindowSize.y / 2 };

	glm::ivec4 viewport3 = {
	myWindowSize.x / 2 , 0,
	myWindowSize.x / 2, myWindowSize.y / 2 };

	glm::ivec4 viewport4 = {
	myWindowSize.x / 2 , myWindowSize.y / 2,
	myWindowSize.x / 2, myWindowSize.y / 2 };

	_RenderScene(viewport, myCamera);
	_RenderScene(viewport2, Camera2);
	_RenderScene(viewport3, Camera3);
	_RenderScene(viewport4, Camera4);
}

void Game::DrawGui(float deltaTime) {
	// Open a new ImGui window
	static bool testOpen = true;
	ImGui::Begin("Test", &testOpen, ImVec2(300, 200));
	// Draw a color editor
	ImGui::ColorEdit4("Clear Color", &myClearColor[0]);
	// Check if a textbox has changed, and update our window title if it has
	if (ImGui::InputText("Window Title", myWindowTitle, 32)) {
		glfwSetWindowTitle(myWindow, myWindowTitle);
	}
	// Our object's test color
	ImGui::ColorEdit4("Object Color", &testColor[0]);

	auto it = SceneManager::Each();
	for (auto& kvp : it) {
		if (ImGui::Button(kvp.first.c_str())) {
			SceneManager::SetCurrentScene(kvp.first);
		}
	}

	ImGui::End();

	// Open a second ImGui window
	ImGui::Begin("Debug");
	// Draw a formatted text line
	ImGui::Text("Time: %f", glfwGetTime());

	// Start a new ImGui header for our camera settings
	if (ImGui::CollapsingHeader("Camera Settings")) {
		// Draw our camera's normal
		glm::vec3 camNormal = activeCamera->GetForward();
		ImGui::DragFloat3("Normal", &camNormal[0]);

		// Get the camera's position so we can edit it
		glm::vec3 position = activeCamera->GetPosition();
		// Draw an editor control for the position, and update camera position
		if (ImGui::DragFloat3("Position", &position[0])) {
			activeCamera->SetPosition(position);
		}
		if (ImGui::Button("Look at center")) {
			activeCamera->LookAt(glm::vec3(0), glm::vec3(0, 0, 1));
		}
		// Get the camera pinning value
		static glm::vec3 camPin;

		// Get whether or not camera pinning is enabled
		bool camPlaneEnabled = activeCamera->GetPinnedUp().has_value();
		// Draw a checkbox for camera pinning
		if (ImGui::Checkbox("Pinning Enabled", &camPlaneEnabled)) {
			// If we've disabled pinning, cache our pinning vector and remove it
			if (!camPlaneEnabled) {
				camPin = activeCamera->GetPinnedUp().value();
				activeCamera->SetPinnedUp(std::optional<glm::vec3>());
			}
			// Set our camera's pinning vector to our cached value
			else {
				activeCamera->SetPinnedUp(camPin);
			}
		}
		// If we have enabled pinning
		if (camPlaneEnabled) {
			// Draw a slider for our camera pin direction
			if (ImGui::InputFloat3("Pin Direction", &camPin[0])) {
				activeCamera->SetPinnedUp(camPin);
			}
		}
	}
	ImGui::End();
}

void Game::_RenderScene(glm::ivec4 viewport, Camera::Sptr camera) 
{

	glViewport(viewport.x, viewport.y, viewport.z, viewport.w);
	glScissor(viewport.x, viewport.y, viewport.z, viewport.w);

	glm::vec4 borderColor = { 0.1f, 0.1f, 0.1f, 1.0f };

	if (camera->isSelected)
		borderColor = { 1.0f, 0.5f, 1.0f, 1.0f };

	int border = 4; // 4px border
	// Clear with the border color
	glClearColor(borderColor.x, borderColor.y, borderColor.z, borderColor.w);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// Set viewport to be inset slightly (the amount is the border width)
	glViewport(viewport.x + border, viewport.y + border, viewport.z - 2 * border, viewport.w - 2 * border);
	glScissor(viewport.x + border, viewport.y + border, viewport.z - 2 * border, viewport.w - 2 * border);

	switch (camera->drawMode)
	{
	case Camera::fill:
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		break;
	case Camera::point:
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
		break;
	case Camera::wireframe:
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		break;
	}

	// Clear our screen every frame
	glClearColor(myClearColor.x, myClearColor.y, myClearColor.z, myClearColor.w);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//myScene.Render(deltaTime);

	// We'll grab a reference to the ecs to make things easier
	auto& ecs = CurrentRegistry();

	// We sort our mesh renderers based on material properties
	// This will group all of our meshes based on shader first, then material second
	ecs.sort<MeshRenderer>([&](const MeshRenderer& lhs, const MeshRenderer& rhs) {
		if (rhs.Material == nullptr || rhs.Mesh == nullptr)
			return false;
		else if (lhs.Material == nullptr || lhs.Mesh == nullptr)
			return true;
		else if (lhs.Material->HasTransparency & !rhs.Material->HasTransparency) //
			return false; // This section is new
		else if (!lhs.Material->HasTransparency & rhs.Material->HasTransparency) // The order IS important
			return true; //
		else if (lhs.Material->GetShader() != rhs.Material->GetShader())
			return lhs.Material->GetShader() < rhs.Material->GetShader();
		else
			return lhs.Material < rhs.Material;
		});


	// These will keep track of the current shader and material that we have bound
	Material::Sptr mat = nullptr;
	Shader::Sptr boundShader = nullptr;

	// A view will let us iterate over all of our entities that have the given component types
	auto view = ecs.view<MeshRenderer>();

	for (const auto& entity : view) {

		// Get our shader
		const MeshRenderer& renderer = ecs.get<MeshRenderer>(entity);

		// Early bail if mesh is invalid
		if (renderer.Mesh == nullptr || renderer.Material == nullptr)
			continue;

		// If our shader has changed, we need to bind it and update our frame-level uniforms
		if (renderer.Material->GetShader() != boundShader) {
			boundShader = renderer.Material->GetShader();
			boundShader->Bind();
			boundShader->SetUniform("a_CameraPos", camera->GetPosition());
			boundShader->SetUniform("a_Time", static_cast<float>(glfwGetTime()));

		}

		// If our material has changed, we need to apply it to the shader
		if (renderer.Material != mat) {
			mat = renderer.Material;
			mat->Apply();
		}

		// We'll need some info about the entities position in the world
		const Transform& transform = ecs.get_or_assign<Transform>(entity);

		// Get the object's transformation
		glm::mat4 worldTransform = transform.GetWorldTransform();

		// Our normal matrix is the inverse-transpose of our object's world rotation
		glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(worldTransform)));

		// Update the MVP using the item's transform
		mat->GetShader()->SetUniform(
			"a_ModelViewProjection",
			camera->GetViewProjection() *
			worldTransform);

		// Update the model matrix to the item's world transform
		mat->GetShader()->SetUniform("a_Model", worldTransform);

		// Update the model matrix to the item's world transform
		mat->GetShader()->SetUniform("a_NormalMatrix", normalMatrix);

		// Draw the item
		renderer.Mesh->Draw();
	}

	auto scene = CurrentScene();
	// Draw the skybox after everything else, if the scene has one
	if (scene->Skybox)
	{
		// Disable culling
		glDisable(GL_CULL_FACE);
		// Set our depth test to less or equal (because we are at 1.0f)
		glDepthFunc(GL_LEQUAL);
		// Disable depth writing
		glDepthMask(GL_FALSE);

		// Make sure no samplers are bound to slot 0
		TextureSampler::Unbind(0);
		// Set up the shader
		scene->SkyboxShader->Bind();
		scene->SkyboxShader->SetUniform("a_View", glm::mat4(glm::mat3(
			camera->GetView()
		)));
		scene->SkyboxShader->SetUniform("a_Projection", camera->Projection);

		scene->Skybox->Bind(0);
		scene->SkyboxShader->SetUniform("s_Skybox", 0);
		scene->SkyboxMesh->Draw();

		// Restore our state
		glDepthMask(GL_TRUE);
		glEnable(GL_CULL_FACE);
		glDepthFunc(GL_LESS);
	}
}

void mouseClickCallback(GLFWwindow* window, int button, int action, int mods)
{
	int windowSizeX, windowSizeY;

	glfwGetWindowSize(window, &windowSizeX, &windowSizeY);

	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		for (int i = 0; i < cameraMap.size() ; i++)
		{
			cameraMap[i]->isSelected = false;
			cameraMap[i]->isFullScreen = false;
		}

		if (mousePosX < windowSizeX / 2 && mousePosY < windowSizeY / 2)
		{
			cameraMap[0] = cameraMap[1];
			cameraMap[1]->isSelected = true;
		}
		else if (mousePosX < windowSizeX / 2 && mousePosY > windowSizeY / 2)
		{
			cameraMap[0] = cameraMap[2];
			cameraMap[2]->isSelected = true;
		}
		else if (mousePosX > windowSizeX / 2 && mousePosY < windowSizeY / 2)
		{
			cameraMap[0] = cameraMap[3];
			cameraMap[3]->isSelected = true;
		}
		else if (mousePosX > windowSizeX / 2 && mousePosY > windowSizeY / 2)
		{
			cameraMap[0] = cameraMap[4];
			cameraMap[4]->isSelected = true;
		}
	}

	//TODO: Fullscreen on right click
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
	{
		if (mousePosX < windowSizeX / 2 && mousePosY < windowSizeY / 2)
		{
			cameraMap[0] = cameraMap[1];
			cameraMap[1]->isFullScreen = true;
		}
		else if (mousePosX > windowSizeX / 2 && mousePosY < windowSizeY / 2)
		{
			LOG_INFO("Top Right");
		}
		else if (mousePosX < windowSizeX / 2 && mousePosY > windowSizeY / 2)
		{
			LOG_INFO("Bottom Left");
		}
		else if (mousePosX > windowSizeX / 2 && mousePosY > windowSizeY / 2)
		{
			LOG_INFO("Bottom Right");
		}
	}
}