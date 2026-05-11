#pragma once
#include<iostream>
#include<vector>
#include "common.h"

class stackVM
{
private:
    std:: vector<int> stack;
    std:: vector<int> program;
    size_t ip=0;

public:
    void loadprogram(const std::vector<int> &prog)
    {
        program=prog;
        ip=0;
    }

    void run()
    {
        bool running = 1;
        while(running and ip<program.size())
        {
            int instr=program[ip];
            ip++;

            switch(static_cast<opcode>(instr))
            {
                case opcode::push:
                {
                    int val=program[ip];
                    stack.push_back(val);
                    ip++;
                    break;
                }
                case opcode::add:
                {
                    int a=stack.back();stack.pop_back();
                    int b=stack.back();stack.pop_back();
                    stack.push_back(a+b);
                    break;
                }
                case opcode::mul:
                {
                    int a=stack.back();stack.pop_back();
                    int b=stack.back();stack.pop_back();
                    stack.push_back(a*b);
                    break;
                }
                case opcode::sub:
                {
                    int b=stack.back();stack.pop_back();
                    int a=stack.back();stack.pop_back();
                    stack.push_back(a-b);
                    break;
                }
                case opcode::div:
                {
                    int b=stack.back();stack.pop_back();
                    int a=stack.back();stack.pop_back();
                    if(b == 0) throw std::runtime_error("Division by zero");
                    stack.push_back(a/b);
                    break;
                }
                case opcode::print:
                {
                    std::cout<<"output: "<<stack.back()<<std::endl;
                    stack.pop_back();   
                    break;
                }
                case opcode::halt:
                {
                    running=false;
                    break;
                }
            }
        }
    }
};

#if 0
int main()
{
    stackVM vm;
    /*for opcode other than push we just store the opcode but 
    for push we store opcode immediately followe by what to push*/
    std::vector<int> myProgram = {
        (int)opcode::push, 3,
        (int)opcode::push, 4,
        (int)opcode::add,
        (int)opcode::push, 2,
        (int)opcode::mul,
        (int)opcode::print,
        (int)opcode::halt
    };
    vm.loadprogram(myProgram);
    vm.run();
    return 0;
}
#endif