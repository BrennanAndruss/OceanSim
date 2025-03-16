/*
 * Built upon provided base code for window creation and event handling
 * CPE 471 Cal Poly Z. Wood + S. Sueda + I. Dunn
 */

#include <iostream>
#include <glad/glad.h>

#define DISABLE_OPENGL_ERROR_CHECKS

#include "GLSL.h"
#include "Camera.h"
#include "Shader.h"
#include "Texture.h"
#include "Mesh.h"
#include "Water.h"
#include "WindowManager.h"
#include "Time.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader/tiny_obj_loader.h>

// value_ptr for glm
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Application : public EventCallbacks 
{
public:

	std::string resourceDir;

	// Singleton instances
	WindowManager* windowManager = nullptr;
	Time* time = nullptr;

	// Camera and window
	Camera camera;
	glm::vec3 moveDirection = glm::vec3(0.0f);

	int screenWidth;
	int screenHeight;

	bool mouseCaptured = false;
	bool firstMouse = true;
	double xPrev, yPrev;

	// Models and geometry
	Water water;
	Mesh cube;

	// Shaders
	Shader simpleShader;
	Shader waterShader;

	// Textures


	// Animation data
	float accumulatedTime = 0.0f;

	void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
	{
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		{
			glfwSetWindowShouldClose(window, GL_TRUE);
		}
		if (key == GLFW_KEY_TAB && action == GLFW_PRESS)
		{
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			firstMouse = true;
		}

		if (key == GLFW_KEY_W)
		{
			if (action == GLFW_PRESS) moveDirection.z += 1.0f;
			if (action == GLFW_RELEASE) moveDirection.z -= 1.0f;
		}
		if (key == GLFW_KEY_A)
		{
			if (action == GLFW_PRESS) moveDirection.x -= 1.0f;
			if (action == GLFW_RELEASE) moveDirection.x += 1.0f;
		}
		if (key == GLFW_KEY_S)
		{
			if (action == GLFW_PRESS) moveDirection.z -= 1.0f;
			if (action == GLFW_RELEASE) moveDirection.z += 1.0f;
		}
		if (key == GLFW_KEY_D)
		{
			if (action == GLFW_PRESS) moveDirection.x += 1.0f;
			if (action == GLFW_RELEASE) moveDirection.x -= 1.0f;
		}
		if (key == GLFW_KEY_E)
		{
			if (action == GLFW_PRESS) moveDirection.y += 1.0f;
			if (action == GLFW_RELEASE) moveDirection.y -= 1.0f;
		}
		if (key == GLFW_KEY_Q)
		{
			if (action == GLFW_PRESS) moveDirection.y -= 1.0f;
			if (action == GLFW_RELEASE) moveDirection.y += 1.0f;
		}

		if (key == GLFW_KEY_Z && action == GLFW_PRESS) {
			glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
		}
		if (key == GLFW_KEY_Z && action == GLFW_RELEASE) {
			glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
		}
	}

	void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods)
	{
		if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
		{
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
	}

	void mouseCallback(GLFWwindow* window, double xPos, double yPos)
	{
		if (firstMouse)
		{
			xPrev = xPos;
			yPrev = yPos;
			firstMouse = false;
		}

		// Calculate deltas
		double xOffset = xPos - xPrev;
		double yOffset = yPos - yPrev;
		xPrev = xPos;
		yPrev = yPos;

		xOffset *= camera.sensitivity;
		yOffset *= camera.sensitivity;
		// Flip up/down rotation
		yOffset *= -1;

		// Update camera rotation
		camera.updateRotation(xOffset, yOffset);
	}

	void resizeCallback(GLFWwindow *window, int width, int height)
	{
		glViewport(0, 0, width, height);
		screenWidth = width;
		screenHeight = height;
		camera.updatePerspective();
	}

	void scrollCallback(GLFWwindow* window, double xOffset, double yOffset)
	{
		camera.updateRotation(xOffset, yOffset);
	}

	void init()
	{
		GLSL::checkVersion();

		// Set background color
		glClearColor(.72f, .84f, 1.06f, 1.0f);
		
		// Enable z-buffer test
		glEnable(GL_DEPTH_TEST);

		// Initialize camera
		camera = Camera(glm::vec3(0.0f, 8.0f, 16.0f), &screenWidth, &screenHeight);
		camera.setupMatricesUbo();
		camera.updateRotation(0.0f, -30.0f);

		// Initialize shaders
		simpleShader.init(resourceDir + "/simple.vert", resourceDir + "/simple.frag");
		waterShader.init(resourceDir + "/water.vert", resourceDir + "/water.frag");
		
		// Initialize ocean
		water = Water(50, 10);
		water.generateMesh();
		water.generateWaves();

		// Initialize models
		loadObj(cube);
	}

	void loadObj(Mesh& mesh)
	{
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> objMaterials;
		std::string errStr;
		bool rc = tinyobj::LoadObj(shapes, objMaterials, errStr,
			(resourceDir + "/cube.obj").c_str());

		if (!rc)
		{
			std::cerr << errStr << std::endl;
		}
		else
		{
			mesh.setupBuffers(shapes[0]);
		}
	}

	void loadMultishapeObj(std::vector<Mesh>& model)
	{
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> objMaterials;
		std::string errStr;
		bool rc = tinyobj::LoadObj(shapes, objMaterials, errStr,
			(resourceDir + "/cube.obj").c_str());

		if (!rc)
		{
			std::cerr << errStr << std::endl;
		}
		else
		{
			model.resize(shapes.size());
			for (size_t i = 0; i < shapes.size(); i++)
			{
				model[i].setupBuffers(shapes[i]);
			}
		}
	}

	void run()
	{
		// Update time
		time->updateTime();
		accumulatedTime += time->getDeltaTime();
		accumulatedTime = fmod(accumulatedTime, 1000.0f);
		
		// Get current frame buffer size
		glfwGetFramebufferSize(windowManager->getHandle(), &screenWidth, &screenHeight);
		glViewport(0, 0, screenWidth, screenHeight);

		// Clear framebuffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//// Projection matrix
		float aspect = screenWidth/(float)screenHeight;
		glm::mat4 projection = glm::perspective(45.0f, aspect, 0.01f, 100.0f);

		//// View matrix
		glm::mat4 view(1.0f);
		view = glm::translate(view, glm::vec3(0.0f, -1.0f, -5.0f));
		// view = glm::rotate(view, glm::radians(30.0f), glm::vec3(1.0f, 0.0f, 0.0f));

		// Update camera position
		camera.updatePosition(moveDirection, time->getDeltaTime());

		// Initialize the model matrix
		glm::mat4 model(1.0f);
		// model = glm::translate(model, glm::vec3(0.0f, 0.0f, -5.0f));
		
		// Configure simple shader
		model = glm::scale(model, glm::vec3(5.0f));

		simpleShader.bind();
		simpleShader.setMat4("P", projection);
		// simpleShader.setMat4("V", view);
		simpleShader.setMat4("V", camera.getViewMatrix());
		simpleShader.setMat4("model", model);

		// Draw the cube
		// cube.draw();
		simpleShader.unbind();

		// Configure water shader and draw the water
		model = glm::mat4(1.0f);

		waterShader.bind();
		//waterShader.setMat4("P", projection);
		//waterShader.setMat4("V", view);
		waterShader.setMat4("model", model);
		waterShader.setFloat("time", accumulatedTime);
		water.draw();

		waterShader.unbind();
	}
};

int main(int argc, char *argv[])
{
	Application application;

	// Where the resources are loaded from
	application.resourceDir = "../../../resources";

	if (argc >= 2)
	{
		application.resourceDir = argv[1];
	}

	// Your main will always include a similar set up to establish your window
	// and GL context, etc.
	application.screenWidth = 640;
	application.screenHeight = 480;

	WindowManager* windowManager = WindowManager::getInstance();
	windowManager->init(application.screenWidth, application.screenHeight);
	windowManager->setEventCallbacks(&application);
	application.windowManager = windowManager;

	// This is the code that will likely change program to program as you
	// may need to initialize or set up different data and state
	application.init();

	// Set up time
	application.time = Time::getInstance();

	// Loop until the user closes the window
	while (!glfwWindowShouldClose(windowManager->getHandle()))
	{
		// Render scene
		application.run();

		// Swap front and back buffers
		glfwSwapBuffers(windowManager->getHandle());
		// Poll for and process events
		glfwPollEvents();
	}

	// Quit program
	windowManager->shutdown();
	return 0;
}
