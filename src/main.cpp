/*
 * Built upon provided base code for window creation and event handling
 * CPE 471 Cal Poly Z. Wood + S. Sueda + I. Dunn
 */

#include <iostream>
#include <glad/glad.h>

#include "GLSL.h"
#include "Shader.h"
#include "Texture.h"
#include "Mesh.h"
#include "WindowManager.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader/tiny_obj_loader.h>

// value_ptr for glm
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>


class Application : public EventCallbacks 
{
public:

	WindowManager* windowManager = nullptr;

	void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
	{
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		{
			glfwSetWindowShouldClose(window, GL_TRUE);
		}
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		{

		}
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) 
		{
			
		}
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) 
		{
			
		}
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		{

		}
		if (key == GLFW_KEY_Z && action == GLFW_PRESS) {
			glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
		}
		if (key == GLFW_KEY_Z && action == GLFW_RELEASE) {
			glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
		}
	}

	void mouseCallback(GLFWwindow *window, int button, int action, int mods)
	{
		double posX, posY;

		if (action == GLFW_PRESS)
		{
			 glfwGetCursorPos(window, &posX, &posY);
			 std::cout << "Pos X " << posX << " Pos Y " << posY << std::endl;
		}
	}

	void resizeCallback(GLFWwindow *window, int width, int height)
	{
		glViewport(0, 0, width, height);
	}

	void init(const std::string& resourceDirectory)
	{
		GLSL::checkVersion();

		// Enable z-buffer test.
		glEnable(GL_DEPTH_TEST);

		
	}

	void initObjs(const std::string& resourceDirectory)
	{

	}

	void run()
	{
		// Get current frame buffer size
		int width, height;
		glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
		glViewport(0, 0, width, height);

		// Clear framebuffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Projection matrix
		float aspect = width/(float)height;
		glm::mat4 projection = glm::perspective(45.0f, aspect, 0.01f, 100.0f);
		// shader.setMat4("projection", projection);

		// View matrix
		glm::mat4 view(1.0f);
		view = glm::translate(view, glm::vec3(0.0f, 0.0f, -6.0f));
		// shader.setMat4("view", view);

		// Initialize the model matrix
		glm::mat4 model(1.0f);
	}
};

int main(int argc, char *argv[])
{
	// Where the resources are loaded from
	std::string resourceDir = "../../../resources";

	if (argc >= 2)
	{
		resourceDir = argv[1];
	}

	Application application;

	// Your main will always include a similar set up to establish your window
	// and GL context, etc.
	WindowManager* windowManager = WindowManager::getInstance();
	windowManager->init(640, 480);
	windowManager->setEventCallbacks(&application);
	application.windowManager = windowManager;

	// This is the code that will likely change program to program as you
	// may need to initialize or set up different data and state
	application.init(resourceDir);
	application.initObjs(resourceDir);

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
