#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <cctype>
#include <string_view>
#include "common.h"

class lexer
{
private:
    std:: string_view src;
    size_t position;
    int cur_line;

    char peek() const
    {
        if(position >= src.length())return '\0';
        return src[position];
    }

    char peekNext() const
    {
        if(position + 1 >= src.length())return '\0';
        return src[position + 1];
    }

    char advance()
    {
        if(position>=src.length())return '\0';
        return src[position++];
    }

    void skipwhitespace()
    {
        while(true)
        {
            if(std::isspace(peek()))
            {
                if(peek() == '\n')
                {
                    cur_line++;
                }
                advance();
            }
            else if(peek() == '/' && peekNext() == '/')
            {
                while(peek() != '\n' && peek() != '\0')
                {
                    advance();
                }
            }
            else
            {
                break;
            }
        }
    }

public:
    lexer(std::string_view src) : src(src), position(0), cur_line(1){}

    token nexttoken();
};

token lexer::nexttoken()
{
    skipwhitespace();
    char c=peek();
    if(c=='\0')
    {
        return token{tokentype::endoffile,"",cur_line};
    }

    if(std::isdigit(c))
    {
        std::string num_str;
        while(std::isdigit(peek()))
        {
            num_str+=advance();
        }
        return token{tokentype::number, num_str, cur_line};
    }

    if(std::isalpha(c) || c=='_')
    {
        std::string id_str;
        while(std::isalnum(peek()) || peek()=='_')
        {
            id_str+=advance();
        }   
        if(id_str=="if") return token{tokentype::kw_if, id_str, cur_line};
        if(id_str=="else") return token{tokentype::kw_else, id_str, cur_line};
        if(id_str=="while") return token{tokentype::kw_while, id_str, cur_line};
        if(id_str=="let") return token{tokentype::kw_let, id_str, cur_line};
        if(id_str=="print") return token{tokentype::kw_print, id_str, cur_line};
        if(id_str=="int") return token{tokentype::kw_int, id_str, cur_line};
        if(id_str=="bool") return token{tokentype::kw_bool, id_str, cur_line};
        if(id_str=="input") return token{tokentype::kw_input, id_str, cur_line};
        
        return token{tokentype::identifier, id_str, cur_line};
    }

    if(c == '+' || c == '-' || c == '*' || c == '/' || c == '=' || c == '<' || c == '>')
    {
        std::string op_str;
        op_str+= advance();
        if(op_str=="=" and peek() == '=')
        {
            op_str+=advance();
        }
        return token{tokentype::op, op_str, cur_line};
    }

    if (c == '(' || c == ')' || c == '{' || c == '}' || c == ';')
    {
        std::string punc_str;
        punc_str+=advance();
        return  token{tokentype::punctuation, punc_str, cur_line};
    }

    std::string unknown_str;
    unknown_str+=advance();
    return token{tokentype::unknown, unknown_str, cur_line};
}

std::string typetostring(tokentype type)
{
    switch(type)
    {
        case tokentype::identifier: return "Identifier";
        case tokentype::number: return "Number";
        case tokentype::op: return "operator";
        case tokentype::punctuation: return "punctuation";
        case tokentype::endoffile: return "EOF";
        case tokentype::kw_if: return "if";
        case tokentype::kw_else: return "else";
        case tokentype::kw_while: return "while";
        case tokentype::kw_let: return "let";
        case tokentype::kw_print: return "print";
        case tokentype::kw_int: return "int";
        case tokentype::kw_bool: return "bool";
        case tokentype::kw_input: return "input";
        default: return "Unknown";
    }
}
