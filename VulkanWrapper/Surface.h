#pragma once

#include <vulkan\vulkan.h>

#include <GLFW/glfw3.h>

class Surface
{

	VkSurfaceKHR surface;

	const VkInstance instance;

public:
	Surface(const VkInstance instance, GLFWwindow* window);

	Surface(Surface&) = delete;

	Surface& operator=(Surface) = delete;

	~Surface();

	VkSurfaceKHR getVkSurface() const;
};

