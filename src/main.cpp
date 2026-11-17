#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string>

#include "colors.hpp"
#include "parser.hpp"
#include "service.hpp"

namespace fs = std::filesystem;

int main()
{
    while (true)
    {
        std::string path = fs::current_path().string();
        if (const char* home_env = std::getenv("HOME"))
        {
            const std::string home = home_env;
            if (!home.empty() &&
                (path == home || path.rfind(home + "/", 0) == 0))
                path.replace(0, home.size(), "~");
        }

        std::cout << color::blue << path << color::reset << "$ " << std::flush;
        std::string input;
        if (!std::getline(std::cin, input))
        {
            std::cout << '\n';
            break;
        }

        try
        {
            const shell::Pipeline pipeline = shell::parse_command(input);
            const shell::Status state = shell::execute_pipeline(pipeline);
            if (state == shell::Status::EXIT) return EXIT_SUCCESS;
            if (state == shell::Status::ERROR) return EXIT_FAILURE;
        }
        catch (const std::invalid_argument& error)
        {
            std::cerr << "shell: syntax error: " << error.what() << '\n';
        }
    }
    return EXIT_SUCCESS;
}
