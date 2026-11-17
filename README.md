# C++ Shell

A lightweight Unix-style command-line shell written in **C++20**. It reads commands, parses arguments and shell operators, and runs built-in commands or external programs. The project focuses on core shell execution features such as **pipelines, input/output redirection, and process management**.

This is a small shell implementation, **not a full Bash or POSIX shell replacement**.

## Features

- **Command execution:** Run external executables found through `PATH` or specified by a path (for example, `./program` or `/usr/bin/ls`).
- **Built-in commands:** `cd`, `pwd`, `echo`, `type`, and `exit`.
- **Pipelines:** Connect multiple commands with `|`, such as `cat file.txt | grep error | wc -l`.
- **Redirection:** Read standard input from a file with `<`, overwrite a file with `>`, or append with `>>`.
- **Quoting and escaping:** Handle single quotes, double quotes, escaped characters, and empty quoted arguments.
- **Syntax errors:** Report malformed inputs, including incomplete pipelines, missing redirection targets, unfinished escapes, and unclosed quotes.
- **Interactive prompt:** Display the current working directory in blue, shortening the home-directory prefix to `~`.

## Requirements

- A Unix-like environment, such as **Linux or WSL** on Windows
- A **C++20-compatible compiler** (such as GCC or Clang)
- **CMake 3.10 or later**

The shell uses POSIX system calls such as `fork`, `execv`, `pipe`, `dup2`, and `waitpid`, so it is not designed for native Windows execution.

## Build and run

From the project root:

```bash
cmake -S . -B build
cmake --build build
./build/shell
```

Once launched, enter commands at the prompt:

```text
~/projects/shell$ pwd
/home/user/projects/shell
~/projects/shell$ echo "Hello, world!"
Hello, world!
```

The prompt above is illustrative; its path depends on your working directory. To leave the shell, run `exit` or press **Ctrl+D**.

## Usage examples

**Execute external programs**

```bash
ls -la
/usr/bin/printf "Hello\n"
./my-program
```

**Use built-in commands**

```bash
pwd
cd ..
cd ~
cd ~/projects
echo "Hello from my shell"
type ls
type cd
exit
```

**Pipe commands together**

```bash
ls | wc -l
cat notes.txt | grep TODO
cat notes.txt | grep TODO | wc -l
```

**Redirect input and output**

```bash
echo "first line" > output.txt
echo "second line" >> output.txt
cat < output.txt
cat < output.txt | wc -l
```

**Quote or escape arguments**

```bash
echo 'single quoted text'
echo "double quoted text"
echo hello\ world
echo "" "two words"
```

## Built-in commands

| Command | Description |
| --- | --- |
| `cd [directory]` | Change the shell's working directory. With no argument, `~`, or `~/...`, it uses `HOME`. |
| `pwd` | Print the current working directory. |
| `echo [arguments...]` | Print arguments separated by spaces, followed by a newline. |
| `type <command>` | Show whether a command is built into the shell or where its executable can be found. |
| `exit [code]` | Exit the shell; no argument or `0` exits successfully. A nonzero or unsupported argument causes a failure exit. |

Built-ins run in the parent process when executed alone, so commands such as `cd` can change the shell's state. In pipelines, each stage runs in a child process.

## How it works

1. **Read:** The main loop displays the prompt and reads a command line.
2. **Tokenize and parse:** The parser recognizes words, quotes, escapes, pipes, and redirection operators, producing a `Pipeline` of `Command` objects.
3. **Execute:** The execution layer either runs a standalone built-in directly or creates child processes with `fork()`.
4. **Connect and redirect:** The shell uses `pipe()` and `dup2()` to wire command stages together and `open()` to handle file redirection. Explicit file redirections take precedence over pipeline descriptors for their respective streams.
5. **Launch and wait:** External commands are located using `PATH` or an explicit path, launched with `execv()`, and reaped with `waitpid()`.

## Project structure

```text
.
├── CMakeLists.txt
├── include/
│   ├── builtins.hpp
│   ├── colors.hpp
│   ├── command.hpp
│   ├── executables.hpp
│   ├── parser.hpp
│   ├── service.hpp
│   ├── status.hpp
│   └── tokens.hpp
└── src/
    ├── builtins.cpp
    ├── executables.cpp
    ├── main.cpp
    ├── parser.cpp
    └── service.cpp
```

## Scope and limitations

The shell supports core command execution, but it does **not** currently implement many features found in larger shells, including:

- Environment-variable or command substitution (such as `$HOME` or `$(command)`)
- Wildcard expansion (such as `*.txt`) or general `~` expansion outside the `cd` built-in
- Command chaining and conditionals (`;`, `&&`, `||`)
- Background jobs and job control (`&`, `jobs`, `fg`, `bg`)
- Command history, tab completion, or interactive line editing
- Standard-error redirection operators such as `2>` and `2>&1`

This project is intended to demonstrate the fundamentals of parsing and running Unix-style commands rather than full compatibility with an existing shell.
