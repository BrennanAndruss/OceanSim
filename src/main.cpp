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

#include "stb_image.h"

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

	// Directional light
	glm::vec3 lightDir = glm::vec3(0.0f, -0.7f, 1.0f);

	// Models and geometry
	Water water;
	Mesh cube;

	// Shaders
	Shader simpleShader;
	Shader waterShader;
	Shader cubemapShader;

	// Textures
	unsigned int cubemapTexture;
	char* faces[6] = {
		"right",
		"left",
		"top",
		"bottom",
		"front",
		"back"
	};

	// Animation data
	float accumulatedTime = 0.0f;

	// Debug flags
	bool debugNormals = false;


	void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
	{
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		{
			glfwSetWindowShouldClose(window, GL_TRUE);
		}

		// Release the cursor to leave the window
		if (key == GLFW_KEY_TAB && action == GLFW_PRESS)
		{
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			firstMouse = true;
		}

		if (key == GLFW_KEY_W)
		{
			if (action == GLFW_PRESS) moveDirection.z += 1.0f;
			else if (action == GLFW_RELEASE) moveDirection.z -= 1.0f;
		}
		if (key == GLFW_KEY_A)
		{
			if (action == GLFW_PRESS) moveDirection.x -= 1.0f;
			else if (action == GLFW_RELEASE) moveDirection.x += 1.0f;
		}
		if (key == GLFW_KEY_S)
		{
			if (action == GLFW_PRESS) moveDirection.z -= 1.0f;
			else if (action == GLFW_RELEASE) moveDirection.z += 1.0f;
		}
		if (key == GLFW_KEY_D)
		{
			if (action == GLFW_PRESS) moveDirection.x += 1.0f;
			else if (action == GLFW_RELEASE) moveDirection.x -= 1.0f;
		}
		if (key == GLFW_KEY_E)
		{
			if (action == GLFW_PRESS) moveDirection.y += 1.0f;
			else if (action == GLFW_RELEASE) moveDirection.y -= 1.0f;
		}
		if (key == GLFW_KEY_Q)
		{
			if (action == GLFW_PRESS) moveDirection.y -= 1.0f;
			else if (action == GLFW_RELEASE) moveDirection.y += 1.0f;
		}

		if (key == GLFW_KEY_N)
		{
			if (action == GLFW_PRESS) debugNormals = true;
			else if (action == GLFW_RELEASE) debugNormals = false;
		}
		if (key == GLFW_KEY_Z)
		{
			if (action == GLFW_PRESS) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			else if (action == GLFW_RELEASE) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
	}

	void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods)
	{
		// Capture the cursor to enable free rotation
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

		// Update camera rotation and view matrix
		camera.updateRotation(xOffset, yOffset);
	}

	void resizeCallback(GLFWwindow *window, int width, int height)
	{
		glViewport(0, 0, width, height);
		screenWidth = width;
		screenHeight = height;

		// Update projection matrix
		camera.updatePerspective();
	}

	void scrollCallback(GLFWwindow* window, double xOffset, double yOffset)
	{
		// movement handled with mouse callback
		// camera.updateRotation(xOffset, yOffset);
	}

	void init()
	{
		GLSL::checkVersion();

		// Set background color
		glClearColor(.72f, .84f, 1.06f, 1.0f);
		
		// Enable z-buffer test
		glEnable(GL_DEPTH_TEST);

		// Initialize camera looking down the z-axis
		camera = Camera(glm::vec3(0.0f, 6.0f, 20.0f), &screenWidth, &screenHeight);
		camera.setupMatricesUbo();
		camera.updateRotation(0.0f, -15.0f);

		// Initialize shaders
		simpleShader.init(resourceDir + "/simple.vert", resourceDir + "/simple.frag");
		waterShader.init(resourceDir + "/water.vert", resourceDir + "/water.frag");
		cubemapShader.init(resourceDir + "/cubemap.vert", resourceDir + "/cubemap.frag");

		// Initialize the skybox
		cubemapTexture = createSky(resourceDir + "/skycube1/", ".bmp");
		
		// Initialize ocean
		water = Water(100, 20);
		water.generateMesh();
		water.generateWaves(1);

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

	unsigned int createSky(std::string dir, char* extension)
	{
		unsigned int textureID;
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

		int width, height, nrChannels;
		stbi_set_flip_vertically_on_load(false);
		for (GLuint i = 0; i < 6; i++)
		{
			unsigned char* data = stbi_load((dir + faces[i] + extension).c_str(), 
				&width, &height, &nrChannels, 0);
			if (data)
			{
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, 
					width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			}
			else
			{
				std::cout << "failed to load: " << (dir + faces[i] + extension) << "\n";
			}
		}

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		std::cout << "creating cube map any errors: " << glGetError() << "\n";
		return textureID;
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

		// Update camera position and view matrix
		camera.updatePosition(moveDirection, time->getDeltaTime());

		// Initialize the model matrix
		glm::mat4 model(1.0f);
		// model = glm::translate(model, glm::vec3(0.0f, 0.0f, -5.0f));

		// Configure simple shader
		model = glm::scale(model, glm::vec3(5.0f));
		model = glm::rotate(model, glm::radians(-15.0f), glm::vec3(1.0f, 0.0f, 0.0f));

		simpleShader.bind();
		simpleShader.setVec3("lightDir", lightDir);
		simpleShader.setVec3("cameraPos", camera.getPosition());
		simpleShader.setVec3("matAmb", glm::vec3(0.1f, 0.1f, 0.2f));
		simpleShader.setVec3("matDif", glm::vec3(1.0f, 1.0f, 1.0f));
		simpleShader.setVec3("matSpec", glm::vec3(0.8f, 0.9f, 1.0f));
		simpleShader.setFloat("matShine", 32.0f);
		simpleShader.setMat4("model", model);

		// Draw the cube
		// cube.draw();
		simpleShader.unbind();

		// Configure water shader and draw the water
		model = glm::mat4(1.0f);

		waterShader.bind();
		waterShader.setMat4("model", model);
		waterShader.setFloat("time", accumulatedTime);
		waterShader.setVec3("lightDir", lightDir);
		waterShader.setVec3("cameraPos", camera.getPosition());
		waterShader.setVec3("matAmb", glm::vec3(0.1f, 0.1f, 0.2f));
		waterShader.setVec3("matDif", glm::vec3(0.1f, 0.3f, 0.6f));
		waterShader.setVec3("matSpec", glm::vec3(0.7f, 0.8f, 0.9f));
		waterShader.setFloat("matShine", 100.0f);
		waterShader.setBool("debugNormals", debugNormals);
		
		water.draw();
		waterShader.unbind();

		// Configure cubemap shader
		model = glm::mat4(1.0f);
		model = glm::scale(model, glm::vec3(50.0f));

		cubemapShader.bind();
		cubemapShader.setMat4("model", model);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);

		// Set the depth function to always draw the skybox
		glDepthFunc(GL_LEQUAL);
		cube.draw();
		glDepthFunc(GL_LESS);

		cubemapShader.unbind();
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
