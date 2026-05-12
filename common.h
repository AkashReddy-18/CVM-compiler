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
    unknown,
    kw_if,
    kw_else,
    kw_while,
    kw_let,
    kw_print,
    kw_int,    // Support int
    kw_bool,   // Support bool
    kw_input   // Support input
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
    halt,
    set_global,
    get_global,
    jump,
    jump_if_false,
    less,
    equal,
    input      // Takes input from user
};

#endif
