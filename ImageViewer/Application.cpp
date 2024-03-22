#include "Application.h"

#include <chrono>
#include <thread>

void Application::processUIEvents()
{

    if (ui.buttonPressed)
        loadImage(ui.onScreenImagePath);
}

bool Application::validateImagePath(std::string imagePath) const
{

    if (imagePath.empty())
        return false;

    return true;
}

Application::Application(ApplicationOptions options)
    : ui({options.imagePath, false})
    , window(100, 100, false)
    , renderer(BufferRendererOptions(window))
    , buffer()
{

    loadImage(options.imagePath);
}

void Application::run()
{

    while (!window.windowShouldClose())
    {

		window.pollEvents();

        processUIEvents();

		renderer.render(ui);

		std::this_thread::sleep_for(std::chrono::milliseconds(2));
    }
}

bool Application::loadImage(std::string imagePath)
{


    if (!validateImagePath(imagePath))
        return false;

    buffer = Buffer(imagePath);

    renderer.submitBuffer(buffer);

    return true;
}
