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
    kw_if,    // NEW
    kw_else,  // NEW
    kw_while  // NEW
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
    jump,           // NEW: Unconditional jump
    jump_if_false,  // NEW: Jump if top of stack is 0
    less,           // NEW: a < b
    equal           // NEW: a == b
};

#endif
