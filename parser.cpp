#pragma once
#include <iostream>
#include <string>
#include <stdexcept>
#include <memory>
#include <vector>
#include "lexer.cpp"
#include "ast.h"

/**
 * The parser takes tokens from the lexer and builds an Abstract Syntax Tree (AST).
 */
class parser
{
private:
    lexer& lex;
    token cur_token;
    
    // Move to the next token in the stream
    void advance()
    {
        cur_token = lex.nexttoken();
    }

    // Handles numbers, variables, and parenthesized expressions
    std::unique_ptr<Expr> parse_factor()
    {
        if(cur_token.type == tokentype::number)
        {
            int val = std::stoi(cur_token.value);
            advance();
            return std::make_unique<literal_expr>(val);
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

    // Handles multiplication and division (higher precedence)
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

    // Handles addition and subtraction
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

public:
    parser(lexer &l) : lex(l)
    {
        advance();
    }

    // Handles comparisons: <, == (lowest expression precedence)
    std::unique_ptr<Expr> parse_expression()
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

    // Parses { stmt1; stmt2; ... }
    std::unique_ptr<Expr> parse_block()
    {
        if (cur_token.type != tokentype::punctuation || cur_token.value != "{")
            throw std::runtime_error("Expected '{' to start a block at line " + std::to_string(cur_token.line));
        
        advance(); // consume '{'
        std::vector<std::unique_ptr<Expr>> statements;
        
        while (cur_token.type != tokentype::punctuation || cur_token.value != "}")
        {
            if (cur_token.type == tokentype::endoffile)
                throw std::runtime_error("Unexpected end of file inside block");
            statements.push_back(parse_statement());
        }
        
        advance(); // consume '}'
        return std::make_unique<block_stmt>(std::move(statements));
    }

    // Parses a single statement
    std::unique_ptr<Expr> parse_statement() 
    {
        // 1. Variable Declaration: let x = 10;
        if (cur_token.type == tokentype::keyword && cur_token.value == "let") 
        {
            advance(); // consume 'let'
            if (cur_token.type != tokentype::identifier) 
                throw std::runtime_error("Expected variable name after 'let'");
            std::string name = cur_token.value;
            advance();
            if (cur_token.type != tokentype::op || cur_token.value != "=") 
                throw std::runtime_error("Expected '=' after variable name");
            advance();
            auto expr = parse_expression();
            if (cur_token.type != tokentype::punctuation || cur_token.value != ";") 
                throw std::runtime_error("Expected ';' at end of let statement");
            advance();
            return std::make_unique<var_decl_expr>(name, std::move(expr));
        }
        // 2. Print: print 10 + 2;
        else if (cur_token.type == tokentype::keyword && cur_token.value == "print") 
        {
            advance(); // consume 'print'
            auto expr = parse_expression();
            if (cur_token.type != tokentype::punctuation || cur_token.value != ";") 
                throw std::runtime_error("Expected ';' after print statement");
            advance();
            return std::make_unique<print_stmt>(std::move(expr));
        }
        // 3. If Statement: if (cond) { ... } else { ... }
        else if (cur_token.type == tokentype::kw_if)
        {
            advance(); // consume 'if'
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
                advance(); // consume 'else'
                else_branch = parse_block();
            }
            return std::make_unique<if_stmt>(std::move(cond), std::move(then_branch), std::move(else_branch));
        }
        // 4. While Statement: while (cond) { ... }
        else if (cur_token.type == tokentype::kw_while)
        {
            advance(); // consume 'while'
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
        // 5. Assignment: z = x + y; (Identifier followed by '=')
        else if (cur_token.type == tokentype::identifier)
        {
            std::string name = cur_token.value;
            advance();
            if (cur_token.type == tokentype::op && cur_token.value == "=")
            {
                advance(); // consume '='
                auto expr = parse_expression();
                if (cur_token.type != tokentype::punctuation || cur_token.value != ";") 
                    throw std::runtime_error("Expected ';' after assignment");
                advance();
                return std::make_unique<assign_stmt>(name, std::move(expr));
            }
            throw std::runtime_error("Unexpected identifier '" + name + "' as a statement");
        }
        
        throw std::runtime_error("Unknown statement starting with: " + cur_token.value + " at line " + std::to_string(cur_token.line));
    }

    // Entry point: parses multiple statements until end of file
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
