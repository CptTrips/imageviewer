#include "Surface.h"

#include <stdexcept>

Surface::Surface(const VkInstance instance, GLFWwindow* window)
	: instance(instance)
{

	if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS)
		throw std::runtime_error("Failed to create window surface");
}

Surface::~Surface()
{

	vkDestroySurfaceKHR(instance, surface, nullptr);
}

VkSurfaceKHR Surface::getVkSurface() const
{

	return surface;
}
