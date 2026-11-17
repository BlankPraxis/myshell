#pragma once

#include <optional>
#include <string>
#include <vector>

namespace shell
{
    // one program or builtin, with redirections that apply only to this stage.
    struct Command
    {
        std::string name;
        std::vector<std::string> args;
        std::optional<std::string> input_redirect;
        std::optional<std::string> output_redirect;
        bool append_output{false};
    };

    // a command line is a series of commands connected by pipes.
    struct Pipeline
    {
        std::vector<Command> commands;
    };
}
