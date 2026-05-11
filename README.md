# CVM++: Custom Virtual Machine and Compiler

CVM++ is a modular scripting language implemented in C++. It follows a complete compilation pipeline, translating source code into proprietary bytecode that executes on a stack-based Virtual Machine.

## Features Implemented

- Lexical Analysis: A custom lexer that converts source code into tokens, handles whitespace, and ignores single-line comments.
- AST Generation: A Recursive Descent Parser that constructs an Abstract Syntax Tree (AST) using std::unique_ptr for memory safety.
- Mathematical Expressions: Full support for operator precedence and nested parentheses.
- Variables and State: Support for variable declarations using 'let' and subsequent re-assignments.
- Control Flow: Implementation of 'if-else' statements and 'while' loops using a jump-patching mechanism.
- Bytecode Compiler: A visitor-pattern based compiler that transforms the AST into numeric opcodes.
- Stack-Based VM: An execution engine featuring a calculation stack and global variable memory.

## Language Syntax Example

// Declare variables
let count = 0;

// Use loops and comparisons
while (count < 3) {
    print count;
    count = count + 1;
}

// Conditional logic
if (count == 3) {
    print 100;
} else {
    print 0;
}

## How to Run

### Compilation
Use a C++ compiler with C++17 support to build the project:

g++ -std=c++17 main.cpp -o cvm_project

### Execution Modes

#### 1. Interactive Mode (REPL)
Run the executable without any arguments to enter the interactive shell:

./cvm_project

#### 2. Script Mode
To run a specific script file, pass the file path as an argument:

./cvm_project test_script.cvm

## Project Structure

- common.h: Shared enums for Opcodes and TokenTypes.
- ast.h: Definitions for AST nodes and the Visitor interface.
- lexer.cpp: Logic for tokenizing raw text.
- parser.cpp: Logic for turning tokens into an AST.
- compiler.cpp: Logic for turning the AST into VM bytecode.
- stack_VM.cpp: The execution engine and stack logic.
- main.cpp: The command-line interface and runner.
