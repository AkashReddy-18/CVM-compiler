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

    char peek() const //to get current char
    {
        if(position >= src.length())return '\0';
        return src[position];
    }

    char peekNext() const //to look one ahead
    {
        if(position + 1 >= src.length())return '\0';
        return src[position + 1];
    }

    char advance() //consume and return current char ..
    {
        if(position>=src.length())return '\0';
        return src[position++];
    }

    void skipwhitespace() //ignore all kind of white spaces and comments!
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
                // Skip single-line comment
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

    //idnetifying numbers
    if(std::isdigit(c))
    {
        std::string num_str;
        while(std::isdigit(peek()))
        {
            num_str+=advance();
        }
        return token{tokentype::number, num_str, cur_line};
    }

    //identify identifiers and key words
    if(std::isalpha(c) || c=='_')
    {
        std::string id_str;
        while(std::isalnum(peek()) || peek()=='_')
        {
            id_str+=advance();
        }   
        if(id_str=="int" || id_str=="return" || id_str=="if")
        {
            return token{tokentype::keyword, id_str, cur_line};
        }
        return token{tokentype::identifier, id_str, cur_line};
    }

    //identifying operators
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

    //identifying punctuation
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
        case tokentype::keyword: return "keyword";
        case tokentype::op: return "operator";
        case tokentype::punctuation: return "punctuation";
        case tokentype::endoffile: return "EOF";
        default: return "Unkown";
    }
}

// int main()
// {
//     std::string code = R"(
//         int main() {
//             int magic_number = 42;
//             return magic_number;
//         }
//     )";

//     lexer lex(code);
//     token token;

//     std::cout << "Type\t\tValue\t\tLine\n";
//     std::cout << "--------------------------------------\n";

//     do
//     {
//         token = lex.nexttoken();
//         std::cout << typetostring(token.type) << "\t'" 
//                   << token.value << "'\t\t" 
//                   << token.line << "\n";
//     }while(token.type != tokentype::endoffile);

//     return 0;
// }