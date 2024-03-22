#include "OptionsParser.h"

OptionsParser::OptionsParser(int argc, char* argv[])
	: imagePath()
{

	if (argc != ARGUMENT_COUNT)
        throw std::runtime_error("Invalid number of options");

	const std::vector<std::string> optionTokens(argv + 1, argv + argc);

	imagePath = optionTokens[0];
}


ApplicationOptions OptionsParser::getApplicationOptions() const
{

	return ApplicationOptions(imagePath);
}
