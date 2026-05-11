#ifndef AST_H
#define AST_H

#include <memory>
#include <vector>

class literal_expr;
class binary_expr;

class ast_visitor
{
public:
    virtual void visit(literal_expr *expr) = 0;
    virtual void visit(binary_expr *expr) = 0;
    virtual ~ast_visitor() = default;
};

class Expr
{
public:
    virtual void accept(ast_visitor *visitor) = 0;
    virtual ~Expr() = default;
};

class literal_expr : public Expr
{
public:
    int val;
    literal_expr(int x) : val(x) {}
    void accept(ast_visitor *visitor) override
    {
        visitor->visit(this);
    }
};

class binary_expr : public Expr
{
public:
    std::string op;
    std::unique_ptr<Expr> left;
    std::unique_ptr<Expr> right;
    binary_expr(std::string op, std::unique_ptr<Expr> l, std::unique_ptr<Expr> r)
        : op(op), left(std::move(l)), right(std::move(r)) {}
    void accept(ast_visitor *visitor) override
    {
        visitor->visit(this);
    }
};

#endif
