#include<iostream>
#include<string>
#include<stdexcept>
#include "lexer.cpp"

class parser
{
private:
    lexer& lex;
    token cur_token;
    
    void advance()
    {
        cur_token=lex.nexttoken();
    }

    double parse_factor()//handles numbers and parentheses
    {
        if(cur_token.type == tokentype::number)
        {
            double val=std::stod(cur_token.value);
            advance();
            return val;
        }
        else if(cur_token.type == tokentype::punctuation and cur_token.value=="(")
        {
            advance();
            double res=parse_exp();
            if(cur_token.type!=tokentype::punctuation or cur_token.value!=")")
            {
                throw std::runtime_error("Syntax Error: Missing ')' at line " + std::to_string(cur_token.line));
            }
            advance();
            return res;
        }
        else if(cur_token.type==tokentype::identifier)
        {
            throw std::runtime_error("Error: Variable evaluation not yet implemented! Found '" + 
                cur_token.value + "' at line " + std::to_string(cur_token.line));
        }
        throw std::runtime_error("Syntax Error: Unexpected token '" + 
            cur_token.value + "' at line " + std::to_string(cur_token.line));
    }

    double parse_term()//for mul and div
    {
        double res=parse_factor();
        while(cur_token.type==tokentype::op and 
        (cur_token.value=="*" or cur_token.value=="/"))
        {
            std::string op=cur_token.value;
            advance();
            double next_factor=parse_factor();
            
            if(op=="*")
            {
                res*=next_factor;
            }
            else if(op=="/")
            {
                if(next_factor==0)throw std::runtime_error("Math Error: Division by zero");
                res/=next_factor;
            }
        }
        return res;
    }
public:
    parser(lexer &l):lex(l)
    {
        advance();
    }

    double parse_exp()//for + and -
    {
        double res=parse_term();
        while(cur_token.type==tokentype::op and 
        (cur_token.value=="+" or cur_token.value=="-"))
        {
            std::string op=cur_token.value;
            advance();
            double  next_term=parse_term();
            if(op=="+")
            {
                res+=next_term;
            }
            else if(op=="-")
            {
                res-=next_term;
            }
        }
        return res;
    }

    double parse()//Entry point for dividing
    {
        double res=parse_exp();
        if(cur_token.type!=tokentype::endoffile)
        {
            throw std::runtime_error("Syntax Error: Unexpected token '" + 
                cur_token.value + "' at the end of expression.");
        }
        return res;
    }
};

int main()
{
    std:: string code=R"(
        10 + 20 * (3 - 1)
    )";
    std::cout << "Source Code: " << code << "\n";
    lexer lex(code);
    parser parser(lex);

    try
    {
        double res=parser.parse();
        std::cout << "Result: " << res << std::endl;
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
    return 0;
}