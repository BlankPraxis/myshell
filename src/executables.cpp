#include "executables.hpp"

#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <optional>
#include <string>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>

#include "status.hpp"
#include "parser.hpp"

namespace fs = std::filesystem;

namespace shell
{
    namespace
    {
        std::vector<char*> convert_to_argv(const std::string& command_name, std::vector<std::string>& command_args)
        {
            std::vector<std::string> arg_storage{};
            arg_storage.push_back(command_name);
            for (const auto& arg : command_args)
            {
                arg_storage.push_back(arg);
            }

            std::vector<char *> argv{};
            for (auto& arg : arg_storage)
            {
                argv.push_back(arg.data());
            }
            argv.push_back(nullptr);

            return argv;
        }
    }


    std::optional<fs::path> find_executable(const std::string& command_to_find)
    {
        const char *path_env { std::getenv("PATH") };
        if (!path_env) return std::nullopt;

        std::string path_var { path_env };
        std::vector<std::string> dirs { shell::parse_path(path_var) };

        fs::path candidate{};
        if (command_to_find.find('/') == std::string::npos)
        {    
            for (const auto& dir : dirs)
            {
                candidate = fs::path(dir) / command_to_find;
                if (fs::exists(candidate) && fs::is_regular_file(candidate) && access(candidate.c_str() , X_OK) == 0)
                {
                    return candidate;
                }
            }
        }
        else
        {
            candidate = command_to_find;
            if (fs::exists(candidate) && fs::is_regular_file(candidate) && access(candidate.c_str(), X_OK) == 0)
            {
                return candidate;
            }
        }

        return std::nullopt;
    }

    shell::Status excecute(const std::string& command_name, std::vector<std::string>& command_args, fs::path executable)
    {
        std::vector<std::string> arg_storage{};
        arg_storage.push_back(command_name);
        for (const auto& arg : command_args)
        {
            arg_storage.push_back(arg);
        }

        std::vector<char *> argv{};
        for (auto& arg : arg_storage)
        {
            argv.push_back(arg.data());
        }
        argv.push_back(nullptr);
        
        pid_t pid { fork() };

        if (pid < 0)
        {
            std::cout << "shell: fork failed\n";
            return shell::Status::CONTINUE;
        }
        if (pid == 0)
        {
            execv(executable.c_str(), argv.data());
            
            std::perror("shell: exec");
            _exit(1);
        }
        else
        {
            int status;
            waitpid(pid, &status, 0);
        }

        return shell::Status::CONTINUE;
    }
}
