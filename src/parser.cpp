#include "parser.hpp"

#include <cstddef>
#include <iostream>
#include <string>
#include <vector>

#include "tokens.hpp"

namespace shell
{
    namespace
    {
        enum class TokenState
        {
            NORMAL,
            DOUBLE_QUOTED,
        };

        std::vector<Token> lex(const std::string& str)
        {
            std::vector<Token> tokens{};

            std::string current_phrase{};
            TokenState state { TokenState::NORMAL };

            for (std::size_t i{}; i < str.length(); ++i)
            {
                char ch { str[i] };
                
                if (ch == ' ')
                {
                    if (i - 1 > 0 && str[i - 1] == '"' && state == TokenState::NORMAL && current_phrase.empty())
                    {
                        tokens.push_back({ .kind = TokenKind::WORD, .value = current_phrase });
                        continue;
                    }
                    if (state != TokenState::NORMAL || current_phrase.empty())
                    {
                        if (state == TokenState::DOUBLE_QUOTED)
                        {
                            current_phrase.push_back(ch);
                        }
                        
                        continue;
                    }

                    tokens.push_back({ .kind = TokenKind::WORD, .value = current_phrase });
                    current_phrase.clear();
                }
                else if (ch == '"')
                {
                    if (state == TokenState::DOUBLE_QUOTED)
                    {
                        state = TokenState::NORMAL;
                    }
                    else
                    {
                        state = TokenState::DOUBLE_QUOTED;
                    }
                }
                else if (ch == '|' || ch == '>' || ch == '<')
                {
                    if (state != TokenState::NORMAL)
                    {
                        current_phrase.push_back(ch);
                        continue;
                    }

                    if (!current_phrase.empty() ||  i - 1 > 0 && (current_phrase.empty() && str[i - 1] == '"'))
                    {
                        tokens.push_back({ .kind = TokenKind::WORD, .value = current_phrase });
                        current_phrase.clear();
                    }

                    switch (ch)
                    {
                        case '|':
                            tokens.push_back({ .kind = TokenKind::PIPE, .value = "|"});
                            break; 
                        case '>':
                            if (i + 1 < str.length() && str[i + 1] == '>')
                            {
                                tokens.push_back({ .kind = TokenKind::REDIRECT_APPEND, .value = ">>" });
                                if (i + 1 < str.length())
                                {
                                    ++i;
                                }
                                break;
                            }
                            
                            tokens.push_back({ .kind = TokenKind::REDIRECT_OUT, .value = ">"});
                            break; 
                        case '<':
                            tokens.push_back({ .kind = TokenKind::REDIRECT_IN, .value = "<" });
                            break;
                    }
                }
                else
                {
                    current_phrase.push_back(ch);
                }
            }
            if (current_phrase.empty() && str[str.length() - 1] == '"')
            {
                tokens.push_back({ .kind = TokenKind::WORD, .value = current_phrase });
                current_phrase.clear();
            }
            else if (!current_phrase.empty())
            {
                tokens.push_back({ .kind = TokenKind::WORD, .value = current_phrase });
                current_phrase.clear();
            }

            // HANDLES PRINTING FOR DEBUGGING
            if (!tokens.empty())
            {
                std::cout << "[\n";
                for (const auto& token : tokens)
                {
                    switch (token.kind)
                    {
                        case TokenKind::WORD:
                            std::cout << "WORD(\"";
                            break;
                        case TokenKind::PIPE:
                            std::cout << "PIPE(\"";
                            break;
                        case TokenKind::REDIRECT_IN:
                            std::cout << "REDIRECT_IN(\"";
                            break;
                        case TokenKind::REDIRECT_OUT:
                            std::cout << "REDIRECT_OUT(\"";
                            break;
                        case TokenKind::REDIRECT_APPEND:
                            std::cout << "REDIRECT_APPEND(\"";
                            break;
                    }
                    std::cout << token.value << "\")\n";
                }
                std::cout << "]\n";
            }
            else
            {
                std::cout << "[]\n";
            }
            //-----------------------------------

            return tokens;
        }
    }

    std::vector<std::string> parse_command(const std::string& str, const char delimeter)
    {
        std::vector<std::string> tokens{};

        // ALMOST WORKS: handle empty token at end when > than 2 spaces are left at the end. and "" (example: echo "" hi) getting registered as empty tokens

        lex(str);

        // bool is_quoted = { false };
        // std::string temp_token{};
        // for (std::size_t i{}; i < str.length(); ++i)
        // {
        //     if (i == str.length() - 1)
        //     {
        //         if (str[i] != delimeter && str[i] != '"')
        //         {
        //             temp_token.push_back(str[i]);
        //         }

        //         tokens.push_back(temp_token);
        //         temp_token.clear();
        //         continue;
        //     }

        //     if (str[i] != delimeter)
        //     {
        //         if (str[i] == '"')
        //         {
        //             is_quoted = not is_quoted;
        //             continue;
        //         }

        //         temp_token.push_back(str[i]);
        //         continue;
        //     }

        //     if (!is_quoted)
        //     {
        //         if (temp_token.empty())
        //         {
        //             continue;
        //         }
        //         tokens.push_back(temp_token);
        //         temp_token.clear();
        //     }
        // }

        // printing the args in a table format: //
        // std::cout << '[' << tokens[0];
        // for (size_t i { 1 }; i < tokens.size(); ++i)
        // {
        //     std::cout << ", " << tokens[i];
        // }
        // std::cout << "]\n";

        return tokens;
    }

    std::vector<std::string> parse_path(const std::string& str)
    {
        const char delimeter { ':' };
        std::vector<std::string> dirs{};

        std::size_t i{};
        for (std::size_t j{}; j < str.length(); ++j)
        {
            if (str[j] == delimeter)
            {
                dirs.push_back(str.substr(i, j - i));
                i = j + 1;
                continue;   
            }
            if (str[i] == delimeter)
            {
                i = j;
            }
        }
        dirs.push_back(str.substr(i));

        return dirs;
    }
}

