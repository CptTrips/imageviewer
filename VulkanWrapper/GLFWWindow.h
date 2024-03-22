#pragma once
#include <iostream>
#include <cstdint>
#include <algorithm>
#include <vector>
#include <GLFW/glfw3.h>

class GLFWWindow
{

public:
	GLFWwindow* window;

	bool frameBufferResized = false;

	GLFWWindow();

	GLFWWindow(const uint32_t height, const uint32_t width, const bool resizable);

	GLFWWindow(const GLFWWindow& other) = delete;

	GLFWWindow& operator=(GLFWWindow other);

	GLFWWindow(GLFWWindow&& other) noexcept;

	~GLFWWindow();

	bool windowShouldClose();

	void pollEvents();

	std::vector<const char*> extensions();

	void getFramebufferSize(int* const width, int* const height);

	static void framebufferResizeCallback(GLFWwindow* window, int width, int height);

	void resize(int width, int height);

private:

	void swap(GLFWWindow& other);
};
