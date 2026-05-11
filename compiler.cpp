#pragma once
#include <iostream>
#include <vector>
#include <memory>
#include "common.h"
#include "ast.h"

class compiler : public ast_visitor
{
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
    }

    std::vector<int> compile(std::unique_ptr<Expr>& root) {
        byte_code.clear();
        root->accept(this);
        byte_code.push_back(static_cast<int>(opcode::print));
        byte_code.push_back(static_cast<int>(opcode::halt));
        return byte_code;
    }
};
