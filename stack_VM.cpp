#pragma once
#include <iostream>
#include <vector>
#include <stdexcept>
#include "common.h"

class stackVM
{
private:
    std::vector<int> stack;
    std::vector<int> program;
    std::vector<int> globals;
    size_t ip = 0;

public:
    void loadprogram(const std::vector<int> &prog, bool reset_globals = true)
    {
        program = prog;
        ip = 0;
        if (globals.empty()) {
            globals.resize(100, 0);
        }
        if (reset_globals) {
            std::fill(globals.begin(), globals.end(), 0);
        }
        stack.clear();
    }

    void run()
    {
        bool running = true;
        while(running && ip < program.size())
        {
            int instr = program[ip++];

            switch(static_cast<opcode>(instr))
            {
                case opcode::push:
                {
                    stack.push_back(program[ip++]);
                    break;
                }
                case opcode::add:
                {
                    int b = stack.back(); stack.pop_back();
                    int a = stack.back(); stack.pop_back();
                    stack.push_back(a + b);
                    break;
                }
                case opcode::sub:
                {
                    int b = stack.back(); stack.pop_back();
                    int a = stack.back(); stack.pop_back();
                    stack.push_back(a - b);
                    break;
                }
                case opcode::mul:
                {
                    int b = stack.back(); stack.pop_back();
                    int a = stack.back(); stack.pop_back();
                    stack.push_back(a * b);
                    break;
                }
                case opcode::div:
                {
                    int b = stack.back(); stack.pop_back();
                    int a = stack.back(); stack.pop_back();
                    if(b == 0) throw std::runtime_error("Division by zero");
                    stack.push_back(a / b);
                    break;
                }
                case opcode::less:
                {
                    int b = stack.back(); stack.pop_back();
                    int a = stack.back(); stack.pop_back();
                    stack.push_back(a < b ? 1 : 0);
                    break;
                }
                case opcode::equal:
                {
                    int b = stack.back(); stack.pop_back();
                    int a = stack.back(); stack.pop_back();
                    stack.push_back(a == b ? 1 : 0);
                    break;
                }
                case opcode::logical_or:
                {
                    int b = stack.back(); stack.pop_back();
                    int a = stack.back(); stack.pop_back();
                    stack.push_back((a != 0 || b != 0) ? 1 : 0);
                    break;
                }
                case opcode::logical_and:
                {
                    int b = stack.back(); stack.pop_back();
                    int a = stack.back(); stack.pop_back();
                    stack.push_back((a != 0 && b != 0) ? 1 : 0);
                    break;
                }
                case opcode::jump:
                {
                    ip = program[ip];
                    break;
                }
                case opcode::jump_if_false:
                {
                    int target = program[ip++];
                    int cond = stack.back(); stack.pop_back();
                    if (cond == 0) ip = target;
                    break;
                }
                case opcode::set_global:
                {
                    int var_id = program[ip++]; 
                    int val = stack.back(); stack.pop_back();           
                    globals[var_id] = val;    
                    break;
                }
                case opcode::get_global:
                {
                    int var_id = program[ip++]; 
                    stack.push_back(globals[var_id]);      
                    break;
                }
                case opcode::print:
                {
                    std::cout << "output: " << stack.back() << std::endl;
                    stack.pop_back();   
                    break;
                }
                case opcode::input:
                {
                    int val;
                    std::cout << "input >> ";
                    std::cin >> val;
                    stack.push_back(val);
                    break;
                }
                case opcode::halt:
                {
                    running = false;
                    break;
                }
            }
        }
    }
};
