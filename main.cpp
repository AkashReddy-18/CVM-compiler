#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>

#include "lexer.cpp"
#include "parser.cpp"
#include "compiler.cpp"
#include "stack_VM.cpp"

/**
 * Runs the full CVM++ pipeline: Lexing -> Parsing -> Compiling -> Executing
 */
void run(const std::string& code)
{
    if(code.empty()) return;

    try
    {
        // 1. Lexing: Break code into tokens
        lexer lex(code);

        // 2. Parsing: Build an AST (List of Statements)
        parser parser_obj(lex);
        auto program_ast = parser_obj.parse_program();

        // 3. Compiling: Turn AST into VM Bytecode
        compiler comp;
        auto bytecode = comp.compile(program_ast);

        // 4. Executing: Run the bytecode on the Virtual Machine
        stackVM vm;
        vm.loadprogram(bytecode);
        vm.run();
    }
    catch(const std::exception& e)
    {
        std::cerr << "Runtime Error: " << e.what() << '\n';
    }
}

/**
 * Interactive REPL mode: Type code line-by-line
 */
void run_repl() {
    std::string line;
    std::cout << "========================================\n";
    std::cout << " Welcome to CVM++! Type 'exit' to quit. \n";
    std::cout << "========================================\n";

    while (true) {
        std::cout << ">> ";
        
        if (!std::getline(std::cin, line)) break;
        
        if (line == "exit" || line == "quit") {
            break;
        }

        run(line);
    }
}

/**
 * File Runner mode: Execute a script from a .cvm file
 */
void run_file(const std::string& filename) {
    std::ifstream file(filename);
    
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file '" << filename << "'" << std::endl;
        return;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    
    run(buffer.str());
}

/**
 * Main Entry Point: Decides between REPL and File mode based on arguments
 */
int main(int argc, char* argv[]) {
    if (argc == 1) {
        // No file provided -> Start REPL
        run_repl();
    } 
    else if (argc == 2) {
        // File provided -> Execute it
        run_file(argv[1]);
    } 
    else {
        std::cerr << "Usage: cvm++ [path_to_script]" << std::endl;
        return 1;
    }

    return 0;
}
