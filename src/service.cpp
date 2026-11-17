#include "service.hpp"

#include <algorithm>
#include <cerrno>
#include <cstdio>
#include <fcntl.h>
#include <iostream>
#include <string>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>

#include "builtins.hpp"
#include "executables.hpp"

namespace shell
{
    namespace
    {
        bool is_builtin(const std::string& name)
        {
            const auto names = builtin_commands();
            return std::find(names.begin(), names.end(), name) != names.end();
        }

        Status run_builtin(const Command& command)
        {
            if (command.name == "exit") return shell::exit(command.args);
            if (command.name == "echo") return shell::echo(command.args);
            if (command.name == "pwd") return shell::pwd();
            if (command.name == "cd") return shell::cd(command.args);
            if (command.name == "type") return shell::type(command.args);
            return Status::CONTINUE;
        }

        bool redirect_file(const std::string& file, int target, int flags)
        {
            const int fd = open(file.c_str(), flags, 0666);
            if (fd < 0)
            {
                std::perror(("shell: " + file).c_str());
                return false;
            }
            if (fd != target)
            {
                if (dup2(fd, target) == -1)
                {
                    std::perror("shell: dup2");
                    close(fd);
                    return false;
                }
                close(fd);
            }
            return true;
        }

        bool apply_redirections(const Command& command)
        {
            if (command.input_redirect &&
                !redirect_file(*command.input_redirect, STDIN_FILENO, O_RDONLY))
                return false;
            if (command.output_redirect &&
                !redirect_file(*command.output_redirect, STDOUT_FILENO,
                               O_WRONLY | O_CREAT |
                               (command.append_output ? O_APPEND : O_TRUNC)))
                return false;
            return true;
        }

        // Standalone builtins run in the parent so `cd`/`exit` affect the shell.
        // Save and restore its descriptors around any redirections.
        class ParentRedirections
        {
        public:
            bool apply(const Command& command)
            {
                std::cout.flush();
                if (command.input_redirect)
                {
                    saved_stdin_ = dup(STDIN_FILENO);
                    if (saved_stdin_ == -1)
                    {
                        std::perror("shell: dup stdin");
                        return false;
                    }
                }
                if (command.output_redirect)
                {
                    saved_stdout_ = dup(STDOUT_FILENO);
                    if (saved_stdout_ == -1)
                    {
                        std::perror("shell: dup stdout");
                        return false;
                    }
                }
                return apply_redirections(command);
            }

            ~ParentRedirections()
            {
                std::cout.flush();
                if (saved_stdin_ != -1)
                {
                    dup2(saved_stdin_, STDIN_FILENO);
                    close(saved_stdin_);
                }
                if (saved_stdout_ != -1)
                {
                    dup2(saved_stdout_, STDOUT_FILENO);
                    close(saved_stdout_);
                }
            }

            ParentRedirections(const ParentRedirections&) = delete;
            ParentRedirections& operator=(const ParentRedirections&) = delete;
            ParentRedirections() = default;

        private:
            int saved_stdin_{-1};
            int saved_stdout_{-1};
        };

        void wait_for_children(const std::vector<pid_t>& children)
        {
            for (const pid_t child : children)
            {
                int status = 0;
                while (waitpid(child, &status, 0) == -1 && errno == EINTR) {}
            }
        }

        [[noreturn]] void run_child(const Command& command, int incoming, int outgoing)
        {
            if (incoming != -1 && dup2(incoming, STDIN_FILENO) == -1)
            {
                std::perror("shell: dup2 stdin");
                _exit(1);
            }
            if (outgoing != -1 && dup2(outgoing, STDOUT_FILENO) == -1)
            {
                std::perror("shell: dup2 stdout");
                _exit(1);
            }
            if (incoming != -1) close(incoming);
            if (outgoing != -1) close(outgoing);

            // Explicit redirections take precedence over pipeline descriptors.
            if (!apply_redirections(command)) _exit(1);

            if (is_builtin(command.name))
            {
                const Status status = run_builtin(command);
                std::cout.flush(); // _exit does not flush C++ output streams.
                std::cerr.flush();
                _exit(status == Status::ERROR ? 1 : 0);
            }
            execute_external(command);
        }
    }

    Status execute_pipeline(const Pipeline& pipeline)
    {
        if (pipeline.commands.empty()) return Status::CONTINUE;

        if (pipeline.commands.size() == 1 && is_builtin(pipeline.commands[0].name))
        {
            ParentRedirections redirections;
            if (!redirections.apply(pipeline.commands[0])) return Status::CONTINUE;
            return run_builtin(pipeline.commands[0]);
        }

        std::vector<pid_t> children;
        int incoming = -1;
        const std::size_t count = pipeline.commands.size();

        for (std::size_t i = 0; i < count; ++i)
        {
            int outgoing[2] = {-1, -1};
            if (i + 1 < count && pipe(outgoing) == -1)
            {
                std::perror("shell: pipe");
                if (incoming != -1) close(incoming);
                wait_for_children(children);
                return Status::CONTINUE;
            }

            // Prevent buffered prompt/output from appearing twice after fork.
            std::cout.flush();
            std::cerr.flush();
            const pid_t pid = fork();
            if (pid == -1)
            {
                std::perror("shell: fork");
                if (incoming != -1) close(incoming);
                if (outgoing[0] != -1) close(outgoing[0]);
                if (outgoing[1] != -1) close(outgoing[1]);
                wait_for_children(children);
                return Status::CONTINUE;
            }
            if (pid == 0)
            {
                if (outgoing[0] != -1) close(outgoing[0]);
                run_child(pipeline.commands[i], incoming, outgoing[1]);
            }

            children.push_back(pid);
            if (incoming != -1) close(incoming);
            if (outgoing[1] != -1) close(outgoing[1]);
            incoming = outgoing[0];
        }
        if (incoming != -1) close(incoming);
        wait_for_children(children);
        return Status::CONTINUE;
    }
}
