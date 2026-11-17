#pragma once

#include <string>

namespace shell
{
    enum class TokenKind
    {
        WORD,
        PIPE,
        REDIRECT_IN,
        REDIRECT_OUT,
        REDIRECT_APPEND
    };

    struct Token
    {
        TokenKind kind;
        std::string value;
    };
}
