# CVM++: Custom Virtual Machine and Compiler

CVM++ is a modular scripting language implemented in C++. It follows a complete compilation pipeline, translating source code into proprietary bytecode that executes on a stack-based Virtual Machine.

## Features Implemented

- Lexical Analysis: A custom lexer that converts source code into tokens, handles whitespace, and ignores single-line comments.
- AST Generation: A Recursive Descent Parser that constructs an Abstract Syntax Tree (AST) using std::unique_ptr for memory safety.
- Mathematical Expressions: Full support for operator precedence and nested parentheses.
- Variables and State: Support for explicit data types (int, bool) and variable declarations using 'let'. 
- REPL Persistence: A stateful interactive shell where variables persist across multiple commands.
- Control Flow: Implementation of 'if-else' statements and 'while' loops using a jump-patching mechanism.
- Logical Operations: Full support for comparison (==, <) and boolean logic (&, |).
- Built-in I/O: Keywords for printing outputs (print) and taking user input (input).
- Bytecode Compiler: A visitor-pattern based compiler that transforms the AST into numeric opcodes.
- Stack-Based VM: An execution engine featuring a calculation stack and global variable memory.

## Language Syntax Example
int x = 10;

bool ok = 1;

int val = input();

while (x < 15) {
    if (ok & (x == 12)) {
        print 100;
    }
    x = x + 1;
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
To run a specific script file (like the provided demo), pass the file path as an argument:

./cvm_project demo_script.cvm

## Project Structure

- common.h: Shared enums for Opcodes and TokenTypes.
- ast.h: Definitions for AST nodes and the Visitor interface.
- lexer.cpp: Logic for tokenizing raw text.
- parser.cpp: Logic for turning tokens into an AST with token peeking.
- compiler.cpp: Logic for turning the AST into VM bytecode.
- stack_VM.cpp: The execution engine and stack logic.
- main.cpp: The command-line interface and stateful REPL runner.
