#ifndef COMMON_H
#define COMMON_H

#include <string>

// Shared between Lexer and Parser
enum class tokentype {
    identifier,
    number,
    keyword,
    op,
    punctuation,
    endoffile,
    unknown
};

struct token {
    tokentype type;
    std::string value;
    int line;
};

// Shared between Compiler and VM
enum class opcode : int {
    push,
    add,
    sub,
    mul,
    div,
    print,
    halt
};

#endif
