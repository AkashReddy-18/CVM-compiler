#include <iostream>
#include <string>
#include <vector>
#include<fstream>
#include<sstream>

#include "lexer.cpp"
#include "parser.cpp"
#include "compiler.cpp"
#include "stack_VM.cpp"

void run(const std::string& code)
{
    if(code.empty())return;

    try
    {
        lexer lex(code);

        // 2. Parsing
        parser parser_obj(lex);
        auto ast = parser_obj.parse();

        // 3. Compiling
        compiler comp;
        auto bytecode = comp.compile(ast);

        // 4. Executing
        stackVM vm;
        vm.loadprogram(bytecode);
        vm.run();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
}

void run_repl() {
    std::string line;
    std::cout << "========================================\n";
    std::cout << " Welcome to CVM++! Type 'exit' to quit. \n";
    std::cout << "========================================\n";

    while (true) {
        std::cout << ">> ";
        
        // Read a full line from the user
        if (!std::getline(std::cin, line)) break;
        
        // Exit condition
        if (line == "exit" || line == "quit") {
            break;
        }

        // Execute the line
        run(line);
    }
}

void run_file(const std::string& filename) {
    // Open the file using an input file stream
    std::ifstream file(filename);
    
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file '" << filename << "'" << std::endl;
        return;
    }

    // Read the entire file into a string using a stringstream buffer
    std::stringstream buffer;
    buffer << file.rdbuf();
    
    // Execute the file's contents
    run(buffer.str());
}

int main(int argc, char* argv[]) {
    // argc is the number of arguments passed to the terminal.
    // argv[0] is always the name of the program itself.

    if (argc == 1) {
        // No extra arguments passed -> Run the REPL
        run_repl();
    } 
    else if (argc == 2) {
        // One extra argument passed -> Treat it as a file path
        run_file(argv[1]);
    } 
    else {
        // Too many arguments passed
        std::cerr << "Usage: cvm++ [path_to_script]" << std::endl;
        return 1;
    }

    return 0;
}