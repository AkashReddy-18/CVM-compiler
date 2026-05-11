#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>

#include "lexer.cpp"
#include "parser.cpp"
#include "compiler.cpp"
#include "stack_VM.cpp"

// Global instances for REPL persistence
compiler global_comp;
stackVM global_vm;

/**
 * Runs the full CVM++ pipeline.
 * @param is_repl If true, uses global compiler/VM and preserves state.
 */
void run(const std::string& code, bool is_repl = false)
{
    if(code.empty()) return;

    try
    {
        lexer lex(code);
        parser parser_obj(lex);
        auto program_ast = parser_obj.parse_program();

        if (is_repl) {
            auto bytecode = global_comp.compile(program_ast);
            global_vm.loadprogram(bytecode, false); // false = don't reset globals
            global_vm.run();
        } else {
            // Fresh instances for file running
            compiler local_comp;
            stackVM local_vm;
            auto bytecode = local_comp.compile(program_ast);
            local_vm.loadprogram(bytecode, true);
            local_vm.run();
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << '\n';
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

        run(line, true);
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
    
    run(buffer.str(), false);
}

/**
 * Main Entry Point
 */
int main(int argc, char* argv[]) {
    if (argc == 1) {
        run_repl();
    } 
    else if (argc == 2) {
        run_file(argv[1]);
    } 
    else {
        std::cerr << "Usage: cvm++ [path_to_script]" << std::endl;
        return 1;
    }

    return 0;
}
