#include "GLFWWindow.h"

GLFWWindow::GLFWWindow()
{
	window = nullptr;
}

GLFWWindow::GLFWWindow(const uint32_t height, const uint32_t width, const bool resizable)
{
	glfwInit();

	// GLFW will assume we are using OpenGL unless told otherwise
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	// We will not handle resizable windows right away
	glfwWindowHint(GLFW_RESIZABLE, resizable);

	window = glfwCreateWindow(width, height, "Vulkan", nullptr, nullptr);

	// So that framebuffer callback has access to frameBufferResized member
	// (why not only pass pointer to member?)
	glfwSetWindowUserPointer(window, this);

	glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);

	std::cout << "Window creation" << std::endl;

}

GLFWWindow& GLFWWindow::operator=(GLFWWindow other)
{

	this->swap(other);

	return *this;
}

GLFWWindow::GLFWWindow(GLFWWindow&& other) noexcept : GLFWWindow()
{
	this->swap(other);
}

GLFWWindow::~GLFWWindow()
{
	if (window)
	{
		glfwDestroyWindow(window);

		std::cout << "Window destruction " << window << std::endl;

		glfwTerminate();
		std::cout << "glfwTerminate" << std::endl;
	}

}

bool GLFWWindow::windowShouldClose()
{
	return glfwWindowShouldClose(window);
}

void GLFWWindow::pollEvents()
{
	glfwPollEvents();
}

std::vector<const char*> GLFWWindow::extensions()
{
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;

	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	std::cout << glfwExtensionCount << " GLFW required extensions." << std::endl;

	return std::vector<const char*>(glfwExtensions, glfwExtensions + glfwExtensionCount);
}

void GLFWWindow::getFramebufferSize(int* const p_width, int* const p_height)
{

	glfwGetFramebufferSize(window, p_width, p_height);
}

void GLFWWindow::framebufferResizeCallback(GLFWwindow* window, int width, int height)
{

	auto self = reinterpret_cast<GLFWWindow*>(glfwGetWindowUserPointer(window));

	self->frameBufferResized = true;
}

void GLFWWindow::resize(int width, int height)
{

	glfwSetWindowSize(window, width, height);
}


void GLFWWindow::swap(GLFWWindow& other)
{
	std::swap(this->window, other.window);
}

