#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream> //03
#include <string> //03

#include <GLM/glm.hpp> //04
#include <glm/gtc/matrix_transform.hpp> //04

GLFWwindow* window;

bool initGLFW() {
	if (glfwInit() == GLFW_FALSE) {
		std::cout << "Failed to Initialize GLFW" << std::endl;
		return false;
	}

	//Create a new GLFW window
	window = glfwCreateWindow(800, 800, "Window", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	return true;
}

bool initGLAD() {
	if (gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) == 0) {
		std::cout << "Failed to initialize Glad" << std::endl;
		return false;
	}
}


GLuint shader_program;

bool loadShaders() {
	// Read Shaders from file
	std::string vert_shader_str;
	std::ifstream vs_stream("vertex_shader.glsl", std::ios::in);
	if (vs_stream.is_open()) {
		std::string Line = "";
		while (getline(vs_stream, Line))
			vert_shader_str += "\n" + Line;
		vs_stream.close();
	}
	else {
		printf("Could not open vertex shader!!\n");
		return false;
	}
	const char* vs_str = vert_shader_str.c_str();

	std::string frag_shader_str;
	std::ifstream fs_stream("frag_shader.glsl", std::ios::in);
	if (fs_stream.is_open()) {
		std::string Line = "";
		while (getline(fs_stream, Line))
			frag_shader_str += "\n" + Line;
		fs_stream.close();
	}
	else {
		printf("Could not open fragment shader!!\n");
		return false;
	}
	const char* fs_str = frag_shader_str.c_str();

	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, &vs_str, NULL);
	glCompileShader(vs);
	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, &fs_str, NULL);
	glCompileShader(fs);

	shader_program = glCreateProgram();
	glAttachShader(shader_program, fs);
	glAttachShader(shader_program, vs);
	glLinkProgram(shader_program);

	return true;
}


int main() {
	//Initialize GLFW
	if (!initGLFW())
		return 1;

	//Initialize GLAD
	if (!initGLAD())
		return 1;

	// Triangle data
	static const GLfloat points[] = {
		-1.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		0.0f, 1.0f, 0.0f
	};

	// Color data
	static const GLfloat colors[] = {
		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 1.0f
	};

	// Color data
	static const GLfloat normals[] = {
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f
	};

	//Lecture 5
	GLfloat	cameraPos[] = {0.0f, 0.0f, 3.0f}; //get position of camera, from camera class when you implement this for real
	GLfloat lightPos[] = {-1.0f, 0.0f, 3.0f}; //probably want to make a light class for point lights
	


	
	//VBO
	GLuint pos_vbo = 0;
	glGenBuffers(1, &pos_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, pos_vbo);
	glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(float), points, GL_STATIC_DRAW);

	GLuint color_vbo = 1;
	glGenBuffers(1, &color_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, color_vbo);
	glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(float), colors, GL_STATIC_DRAW);

	GLuint normal_vbo = 2;
	glGenBuffers(1, &normal_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, normal_vbo);
	glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(float), normals, GL_STATIC_DRAW);
	
	
	// VAO
	GLuint vao = 0;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, pos_vbo);

	//(index, size, type, normalized, stride, pointer)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	//(stride: byte offset between consecutive values)
	//(pointer: offset of the first component of the 
	//first attribute in the array - initial value is 0)

	glBindBuffer(GL_ARRAY_BUFFER, color_vbo);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glBindBuffer(GL_ARRAY_BUFFER, normal_vbo);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	// Load your shaders
	if (!loadShaders())
		return 1;

	// Lecture 04
	// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	int width, height;
	glfwGetWindowSize(window, &width, &height);
	glm::mat4 Projection = 
		glm::perspective(glm::radians(45.0f), 
		(float)width / (float)height, 0.1f, 100.0f);

	// Camera matrix
	glm::mat4 View = glm::lookAt(
		glm::vec3(0, 0, 3), // Camera is at (2,3,2), in World Space
		glm::vec3(0, 0, 0), // and looks at the origin
		glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
	);

	// Model matrix : an identity matrix (model will be at the origin)
	glm::mat4 Model = glm::mat4(1.0f);
	// create individual matrices glm::mat4 T R and S, then multiply them
	//	Model = glm::translate(Model, glm::vec3(0.0f, 0.0f, 0.0f));
	//	Model = glm::scale(Model, glm::vec3(2.0f, 1.0f, 1.0f));


	// Our ModelViewProjection : multiplication of our 3 matrices
	glm::mat4 mvp = Projection * View * Model; // Remember, matrix multiplication is the other way around

	// T * R * S usual order
	// Or if you want to rotate the object around a certain point
	// T * R * point T * S

	// Get a handle for our "MVP" uniform
	// Only during the initialisation
	
	GLuint MatrixID = glGetUniformLocation(shader_program, "MVP");
	GLuint ViewID = glGetUniformLocation(shader_program, "View");
	GLuint ModelID = glGetUniformLocation(shader_program, "Model");
	GLuint LightPosID = glGetUniformLocation(shader_program, "LightWorldPos");


	// Face culling
	//glEnable(GL_CULL_FACE);
	//glFrontFace(GL_CW); 
	//glCullFace(GL_FRONT); //GL_BACK, GL_FRONT_AND_BACK


	///// Game loop ///// 
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		 
		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shader_program);

		Model = glm::rotate(Model, glm::radians(1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		mvp = Projection * View * Model;

		//glBindVertexArray(vao);

		//Lecture 05
		glUniformMatrix4fv(ViewID, 1, GL_FALSE, &View[0][0]);
		glUniformMatrix4fv(ModelID, 1, GL_FALSE, &Model[0][0]);
		glUniformMatrix3fv(LightPosID, 1, GL_FALSE, &lightPos[0]);
		
		//Lecture 04
		//Send our transformation to the currently bound shader, in the "MVP" uniform
		//This is done in the main loop since each model will have a different MVP matrix (At least for the M part)
		
		
		
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &mvp[0][0]);
		
		//Draw points 0-3 from the currently bound VAO with current in-use shader
		glDrawArrays(GL_TRIANGLES, 0, 3);


		glfwSwapBuffers(window);
	}
	return 0;

}