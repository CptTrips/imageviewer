#pragma once

#include <string>

#include "Application.h"

class OptionsParser
{

    static constexpr int ARGUMENT_COUNT{ 2 };

    std::string imagePath;

public:

    OptionsParser(int argc, char* argv[]);

    std::string getImagePath() const;

    ApplicationOptions getApplicationOptions() const;
    
};

