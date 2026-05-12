#pragma once
#include <iostream>
#include <vector>
#include <memory>
#include <map>
#include "common.h"
#include "ast.h"

class compiler : public ast_visitor
{
private:
    std::map<std::string, int> symbol_table; 
    int next_var_id = 0;

    int get_or_create_var(const std::string& name) 
    {
        if (symbol_table.find(name) == symbol_table.end()) 
        {
            symbol_table[name] = next_var_id++;
        }
        return symbol_table[name];
    }

    void patch(int index, int value)
    {
        byte_code[index] = value;
    }

public:
    std::vector<int> byte_code;

    void visit(literal_expr* expr) override
    {
        byte_code.push_back(static_cast<int>(opcode::push));
        byte_code.push_back(expr->val);
    }

    void visit(binary_expr* expr) override
    {
        expr->left->accept(this);
        expr->right->accept(this);

        if (expr->op == "+") byte_code.push_back(static_cast<int>(opcode::add));
        else if (expr->op == "-") byte_code.push_back(static_cast<int>(opcode::sub));
        else if (expr->op == "*") byte_code.push_back(static_cast<int>(opcode::mul));
        else if (expr->op == "/") byte_code.push_back(static_cast<int>(opcode::div));
        else if (expr->op == "<") byte_code.push_back(static_cast<int>(opcode::less));
        else if (expr->op == "==") byte_code.push_back(static_cast<int>(opcode::equal));
    }

    void visit(var_decl_expr* expr) override
    {
        expr->value->accept(this);
        int var_id = get_or_create_var(expr->var_name);
        byte_code.push_back(static_cast<int>(opcode::set_global));
        byte_code.push_back(var_id);
    }

    void visit(assign_stmt* expr) override
    {
        expr->value->accept(this);
        if (symbol_table.find(expr->var_name) == symbol_table.end())
            throw std::runtime_error("Cannot assign to undeclared variable: " + expr->var_name);
        int var_id = symbol_table[expr->var_name];
        byte_code.push_back(static_cast<int>(opcode::set_global));
        byte_code.push_back(var_id);
    }

    void visit(var_read_expr* expr) override
    {
        if (symbol_table.find(expr->var_name) == symbol_table.end()) {
            throw std::runtime_error("Undefined variable: " + expr->var_name);
        }
        int var_id = symbol_table[expr->var_name];
        byte_code.push_back(static_cast<int>(opcode::get_global));
        byte_code.push_back(var_id);
    }

    void visit(print_stmt* expr) override
    {
        expr->value->accept(this);
        byte_code.push_back(static_cast<int>(opcode::print));
    }

    void visit(block_stmt* expr) override
    {
        for (const auto& stmt : expr->statements) {
            stmt->accept(this);
        }
    }

    void visit(if_stmt* expr) override
    {
        expr->condition->accept(this);
        byte_code.push_back(static_cast<int>(opcode::jump_if_false));
        int jump_if_false_idx = byte_code.size();
        byte_code.push_back(0); 
        
        expr->then_branch->accept(this);
        
        if (expr->else_branch) 
        {
            byte_code.push_back(static_cast<int>(opcode::jump));
            int jump_idx = byte_code.size();
            byte_code.push_back(0); 
            
            patch(jump_if_false_idx, byte_code.size());
            expr->else_branch->accept(this);
            patch(jump_idx, byte_code.size());
        }
        else 
        {
            patch(jump_if_false_idx, byte_code.size());
        }
    }

    void visit(while_stmt* expr) override
    {
        int start_label = byte_code.size();
        expr->condition->accept(this);
        
        byte_code.push_back(static_cast<int>(opcode::jump_if_false));
        int jump_if_false_idx = byte_code.size();
        byte_code.push_back(0); 
        
        expr->body->accept(this);
        
        byte_code.push_back(static_cast<int>(opcode::jump));
        byte_code.push_back(start_label);
        
        patch(jump_if_false_idx, byte_code.size());
    }

    void visit(input_expr* expr) override
    {
        byte_code.push_back(static_cast<int>(opcode::input));
    }

    std::vector<int> compile(const std::vector<std::unique_ptr<Expr>>& program) {
        byte_code.clear();
        for (const auto& stmt : program) {
            stmt->accept(this);
        }
        byte_code.push_back(static_cast<int>(opcode::halt));
        return byte_code;
    }
};
