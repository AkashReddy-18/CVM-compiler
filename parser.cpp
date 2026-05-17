#pragma once
#include <iostream>
#include <string>
#include <stdexcept>
#include <memory>
#include <vector>
#include "lexer.cpp"
#include "ast.h"

class parser
{
private:
    lexer& lex;
    token cur_token;
    
    void advance()
    {
        cur_token = lex.nexttoken();
    }

    std::unique_ptr<Expr> parse_factor()
    {
        if(cur_token.type == tokentype::number)
        {
            int val = std::stoi(cur_token.value);
            advance();
            return std::make_unique<literal_expr>(val);
        }
        else if (cur_token.type == tokentype::kw_input)
        {
            advance();
            if (cur_token.type == tokentype::punctuation && cur_token.value == "(") {
                advance();
                if (cur_token.type != tokentype::punctuation || cur_token.value != ")")
                    throw std::runtime_error("Expected ')' after 'input('");
                advance();
            }
            return std::make_unique<input_expr>();
        }
        else if (cur_token.type == tokentype::identifier) 
        {
            std::string name = cur_token.value;
            advance();
            return std::make_unique<var_read_expr>(name);
        }
        else if(cur_token.type == tokentype::punctuation && cur_token.value == "(")
        {
            advance();
            auto res = parse_expression();
            if(cur_token.type != tokentype::punctuation || cur_token.value != ")")
            {
                throw std::runtime_error("Syntax Error: Missing ')' at line " + std::to_string(cur_token.line));
            }
            advance();
            return res;
        }
        throw std::runtime_error("Syntax Error: Unexpected token '" + 
            cur_token.value + "' at line " + std::to_string(cur_token.line));
    }

    std::unique_ptr<Expr> parse_term()
    {
        auto left = parse_factor();
        while(cur_token.type == tokentype::op && 
        (cur_token.value == "*" || cur_token.value == "/"))
        {
            std::string op = cur_token.value;
            advance();
            auto right = parse_factor();
            left = std::make_unique<binary_expr>(op, std::move(left), std::move(right));
        }
        return left;
    }

    std::unique_ptr<Expr> parse_addition()
    {
        auto left = parse_term();
        while(cur_token.type == tokentype::op && 
        (cur_token.value == "+" || cur_token.value == "-"))
        {
            std::string op = cur_token.value;
            advance();
            auto right = parse_term();
            left = std::make_unique<binary_expr>(op, std::move(left), std::move(right));
        }
        return left;
    }

    std::unique_ptr<Expr> parse_comparison()
    {
        auto left = parse_addition();
        while(cur_token.type == tokentype::op && 
        (cur_token.value == "<" || cur_token.value == "=="))
        {
            std::string op = cur_token.value;
            advance();
            auto right = parse_addition();
            left = std::make_unique<binary_expr>(op, std::move(left), std::move(right));
        }
        return left;
    }

public:
    parser(lexer &l) : lex(l)
    {
        advance();
    }

    std::unique_ptr<Expr> parse_expression()
    {
        auto left = parse_comparison();
        while(cur_token.type == tokentype::op && 
        (cur_token.value == "|" || cur_token.value == "&"))
        {
            std::string op = cur_token.value;
            advance();
            auto right = parse_comparison();
            left = std::make_unique<binary_expr>(op, std::move(left), std::move(right));
        }
        return left;
    }

    std::unique_ptr<Expr> parse_block()
    {
        if (cur_token.type != tokentype::punctuation || cur_token.value != "{")
            throw std::runtime_error("Expected '{' to start a block at line " + std::to_string(cur_token.line));
        
        advance();
        std::vector<std::unique_ptr<Expr>> statements;
        while (cur_token.type != tokentype::punctuation || cur_token.value != "}")
        {
            if (cur_token.type == tokentype::endoffile)
                throw std::runtime_error("Unexpected end of file inside block");
            statements.push_back(parse_statement());
        }
        advance();
        return std::make_unique<block_stmt>(std::move(statements));
    }

    std::unique_ptr<Expr> parse_statement() 
    {
        if (cur_token.type == tokentype::kw_let || cur_token.type == tokentype::kw_int || cur_token.type == tokentype::kw_bool) 
        {
            advance();
            if (cur_token.type != tokentype::identifier) 
                throw std::runtime_error("Expected variable name after declaration keyword");
            std::string name = cur_token.value;
            advance();
            if (cur_token.type != tokentype::op || cur_token.value != "=") 
                throw std::runtime_error("Expected '=' after variable name");
            advance();
            auto expr = parse_expression();
            if (cur_token.type == tokentype::punctuation && cur_token.value == ";") advance();
            return std::make_unique<var_decl_expr>(name, std::move(expr));
        }
        else if (cur_token.type == tokentype::kw_print) 
        {
            advance();
            auto expr = parse_expression();
            if (cur_token.type == tokentype::punctuation && cur_token.value == ";") advance();
            return std::make_unique<print_stmt>(std::move(expr));
        }
        else if (cur_token.type == tokentype::kw_if)
        {
            advance();
            if (cur_token.type != tokentype::punctuation || cur_token.value != "(")
                throw std::runtime_error("Expected '(' after 'if'");
            advance();
            auto cond = parse_expression();
            if (cur_token.type != tokentype::punctuation || cur_token.value != ")")
                throw std::runtime_error("Expected ')' after if condition");
            advance();
            auto then_branch = parse_block();
            std::unique_ptr<Expr> else_branch = nullptr;
            if (cur_token.type == tokentype::kw_else)
            {
                advance();
                else_branch = parse_block();
            }
            return std::make_unique<if_stmt>(std::move(cond), std::move(then_branch), std::move(else_branch));
        }
        else if (cur_token.type == tokentype::kw_while)
        {
            advance();
            if (cur_token.type != tokentype::punctuation || cur_token.value != "(")
                throw std::runtime_error("Expected '(' after 'while'");
            advance();
            auto cond = parse_expression();
            if (cur_token.type != tokentype::punctuation || cur_token.value != ")")
                throw std::runtime_error("Expected ')' after while condition");
            advance();
            auto body = parse_block();
            return std::make_unique<while_stmt>(std::move(cond), std::move(body));
        }
        else if (cur_token.type == tokentype::identifier)
        {
            std::string name = cur_token.value;
            // peek next manually is hard here, so we do it by assuming '=' is assignment
            // In a better parser we would peek.
            // For now, we'll try to re-parse.
        }
        
        auto expr = parse_expression();
        if (cur_token.type == tokentype::punctuation && cur_token.value == ";") advance();
        return expr;
    }

    std::vector<std::unique_ptr<Expr>> parse_program()
    {
        std::vector<std::unique_ptr<Expr>> statements;
        while(cur_token.type != tokentype::endoffile)
        {
            statements.push_back(parse_statement());
        }
        return statements;
    }
};
