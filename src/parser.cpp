#include "parser.hpp"

#include <cctype>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "tokens.hpp"

namespace shell
{
    namespace
    {
        enum class QuoteState
        {
            NORMAL,
            SINGLE_QUOTED,
            DOUBLE_QUOTED
        };

        std::vector<Token> lex(const std::string& input)
        {
            std::vector<Token> tokens;
            std::string word;
            QuoteState state{QuoteState::NORMAL};
            bool word_started{false}; // also preserves empty quoted arguments: "".

            auto finish_word = [&]() {
                if (word_started)
                {
                    tokens.push_back({TokenKind::WORD, std::move(word)});
                    word.clear();
                    word_started = false;
                }
            };

            for (std::size_t i = 0; i < input.size(); ++i)
            {
                const char ch = input[i];

                if (ch == '\\' && state != QuoteState::SINGLE_QUOTED)
                {
                    if (i + 1 == input.size())
                    {
                        throw std::invalid_argument("unfinished escape sequence");
                    }
                    const char next = input[i + 1];
                    // within double quotes, a backslash is only special before
                    // $, `, ", \, or a newline (as in a POSIX shell).
                    if (state == QuoteState::DOUBLE_QUOTED &&
                        next != '$' && next != '`' && next != '"' &&
                        next != '\\' && next != '\n')
                    {
                        word.push_back('\\');
                    }
                    else
                    {
                        if (next != '\n') word.push_back(next);
                        ++i;
                    }
                    word_started = true;
                }
                else if (state == QuoteState::SINGLE_QUOTED)
                {
                    if (ch == '\'') state = QuoteState::NORMAL;
                    else word.push_back(ch);
                }
                else if (state == QuoteState::DOUBLE_QUOTED)
                {
                    if (ch == '"') state = QuoteState::NORMAL;
                    else word.push_back(ch);
                }
                else if (ch == '\'' || ch == '"')
                {
                    state = ch == '\'' ? QuoteState::SINGLE_QUOTED : QuoteState::DOUBLE_QUOTED;
                    word_started = true;
                }
                else if (std::isspace(static_cast<unsigned char>(ch)))
                {
                    finish_word();
                }
                else if (ch == '|' || ch == '>' || ch == '<')
                {
                    finish_word();
                    if (ch == '|') tokens.push_back({TokenKind::PIPE, "|"});
                    else if (ch == '<') tokens.push_back({TokenKind::REDIRECT_IN, "<"});
                    else if (i + 1 < input.size() && input[i + 1] == '>')
                    {
                        tokens.push_back({TokenKind::REDIRECT_APPEND, ">>"});
                        ++i;
                    }
                    else tokens.push_back({TokenKind::REDIRECT_OUT, ">"});
                }
                else
                {
                    word.push_back(ch);
                    word_started = true;
                }
            }

            if (state != QuoteState::NORMAL)
            {
                throw std::invalid_argument("unterminated quoted string");
            }
            finish_word();
            return tokens;
        }
    }

    Pipeline parse_command(const std::string& input)
    {
        const std::vector<Token> tokens = lex(input);
        Pipeline pipeline;
        Command current;
        bool has_command{false};

        for (std::size_t i = 0; i < tokens.size(); ++i)
        {
            const Token& token = tokens[i];
            switch (token.kind)
            {
                case TokenKind::WORD:
                    if (!has_command)
                    {
                        current.name = token.value;
                        has_command = true;
                    }
                    else current.args.push_back(token.value);
                    break;

                case TokenKind::PIPE:
                    if (!has_command || current.name.empty())
                        throw std::invalid_argument("missing command before pipe");
                    pipeline.commands.push_back(std::move(current));
                    current = Command{};
                    has_command = false;
                    break;

                case TokenKind::REDIRECT_IN:
                case TokenKind::REDIRECT_OUT:
                case TokenKind::REDIRECT_APPEND:
                    if (++i >= tokens.size() || tokens[i].kind != TokenKind::WORD)
                        throw std::invalid_argument("expected filename after '" + token.value + "'");
                    if (token.kind == TokenKind::REDIRECT_IN)
                        current.input_redirect = tokens[i].value;
                    else
                    {
                        current.output_redirect = tokens[i].value;
                        current.append_output = token.kind == TokenKind::REDIRECT_APPEND;
                    }
                    break;
            }
        }

        if (has_command && !current.name.empty())
        {
            pipeline.commands.push_back(std::move(current));
        }
        else if (!tokens.empty())
        {
            throw std::invalid_argument("missing command at end of input");
        }
        return pipeline;
    }

    std::vector<std::string> parse_path(const std::string& path)
    {
        std::vector<std::string> dirs;
        std::size_t start = 0;
        while (true)
        {
            const std::size_t end = path.find(':', start);
            if (end == std::string::npos)
            {
                dirs.push_back(path.substr(start));
                break;
            }
            dirs.push_back(path.substr(start, end - start));
            start = end + 1;
        }
        return dirs;
    }
}
