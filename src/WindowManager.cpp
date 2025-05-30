
#include "WindowManager.h"
#include "GLSL.h"

#include <iostream>


void error_callback(int error, const char *description)
{
	std::cerr << description << std::endl;
}

WindowManager* WindowManager::getInstance()
{
	static WindowManager instance;
	return &instance;
}

WindowManager::WindowManager() {}

WindowManager::~WindowManager() {}

bool WindowManager::init(int const width, int const height)
{
	glfwSetErrorCallback(error_callback);

	// Initialize glfw library
	if (!glfwInit())
	{
		return false;
	}

	//request the highest possible version of OGL - important for mac
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);

	// Create a windowed mode window and its OpenGL context.
	windowHandle = glfwCreateWindow(width, height, "Ocean Simulator", nullptr, nullptr);
	if (! windowHandle)
	{
		glfwTerminate();
		return false;
	}

	glfwMakeContextCurrent(windowHandle);

	// Initialize GLAD
	if (!gladLoadGL())
	{
		std::cerr << "Failed to initialize GLAD" << std::endl;
		return false;
	}

	std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;
	std::cout << "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

	// Set vsync
	glfwSwapInterval(1);

	glfwSetKeyCallback(windowHandle, key_callback);
	glfwSetMouseButtonCallback(windowHandle, mouse_button_callback);
	glfwSetFramebufferSizeCallback(windowHandle, resize_callback);
	glfwSetCursorPosCallback(windowHandle, mouse_callback);
	glfwSetScrollCallback(windowHandle, scroll_callback);

	return true;
}

void WindowManager::shutdown()
{
	glfwDestroyWindow(windowHandle);
	glfwTerminate();
}

void WindowManager::setEventCallbacks(EventCallbacks * callbacks_in)
{
	callbacks = callbacks_in;
}

GLFWwindow * WindowManager::getHandle()
{
	return windowHandle;
}

void WindowManager::key_callback(GLFWwindow * window, int key, int scancode, int action, int mods)
{
	WindowManager* instance = WindowManager::getInstance();
	if (instance && instance->callbacks)
	{
		instance->callbacks->keyCallback(window, key, scancode, action, mods);
	}
}

void WindowManager::mouse_button_callback(GLFWwindow * window, int button, int action, int mods)
{
	WindowManager* instance = WindowManager::getInstance();
	if (instance && instance->callbacks)
	{
		instance->callbacks->mouseButtonCallback(window, button, action, mods);
	}
}

void WindowManager::resize_callback(GLFWwindow * window, int in_width, int in_height)
{
	WindowManager* instance = WindowManager::getInstance();
	if (instance && instance->callbacks)
	{
		instance->callbacks->resizeCallback(window, in_width, in_height);
	}
}

void WindowManager::mouse_callback(GLFWwindow* window, double xPos, double yPos)
{
	WindowManager* instance = WindowManager::getInstance();
	if (instance && instance->callbacks)
	{
		instance->callbacks->mouseCallback(window, xPos, yPos);
	}
}

void WindowManager::scroll_callback(GLFWwindow* window, double xOffset, double yOffset)
{
	WindowManager* instance = WindowManager::getInstance();
	if (instance && instance->callbacks)
	{
		instance->callbacks->scrollCallback(window, xOffset, yOffset);
	}
}
