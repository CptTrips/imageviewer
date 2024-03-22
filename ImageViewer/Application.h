#pragma once

#include <string>

#include "UI.h"
#include "GLFWWindow.h"
#include "BufferRenderer.h"
#include "Buffer.h"


struct ApplicationOptions
{

    std::string imagePath;

    ApplicationOptions(std::string imagePath)
        : imagePath(imagePath)
    {}
};

class Application
{

    UI ui;

    GLFWWindow window;

    BufferRenderer renderer;

    Buffer buffer;

    void processUIEvents();

    bool validateImagePath(std::string imagePath) const;

public:

    Application(ApplicationOptions);

    void run();

    bool loadImage(std::string imagePath);
};

