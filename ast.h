#ifndef AST_H
#define AST_H

#include <memory>
#include <vector>
#include <string>

class literal_expr;
class binary_expr;
class var_decl_expr;
class var_read_expr;
class assign_stmt;
class print_stmt;
class block_stmt;
class if_stmt;
class while_stmt;
class input_expr; // NEW

class ast_visitor
{
public:
    virtual void visit(literal_expr *expr) = 0;
    virtual void visit(binary_expr *expr) = 0;
    virtual void visit(var_decl_expr* expr) = 0;
    virtual void visit(var_read_expr* expr) = 0;
    virtual void visit(assign_stmt* expr) = 0;
    virtual void visit(print_stmt* expr) = 0;
    virtual void visit(block_stmt* expr) = 0;
    virtual void visit(if_stmt* expr) = 0;
    virtual void visit(while_stmt* expr) = 0;
    virtual void visit(input_expr* expr) = 0; // NEW
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
    void accept(ast_visitor *visitor) override { visitor->visit(this); }
};

class binary_expr : public Expr
{
public:
    std::string op;
    std::unique_ptr<Expr> left;
    std::unique_ptr<Expr> right;
    binary_expr(std::string op, std::unique_ptr<Expr> l, std::unique_ptr<Expr> r)
        : op(op), left(std::move(l)), right(std::move(r)) {}
    void accept(ast_visitor *visitor) override { visitor->visit(this); }
};

class var_decl_expr : public Expr {
public:
    std::string var_name;
    std::unique_ptr<Expr> value;
    var_decl_expr(std::string name, std::unique_ptr<Expr> val) 
        : var_name(name), value(std::move(val)) {}
    void accept(ast_visitor* visitor) override { visitor->visit(this); }
};

class var_read_expr : public Expr {
public:
    std::string var_name;
    var_read_expr(std::string name) : var_name(name) {}
    void accept(ast_visitor* visitor) override { visitor->visit(this); }
};

class assign_stmt : public Expr {
public:
    std::string var_name;
    std::unique_ptr<Expr> value;
    assign_stmt(std::string name, std::unique_ptr<Expr> val) 
        : var_name(name), value(std::move(val)) {}
    void accept(ast_visitor* visitor) override { visitor->visit(this); }
};

class print_stmt : public Expr {
public:
    std::unique_ptr<Expr> value;
    print_stmt(std::unique_ptr<Expr> val) : value(std::move(val)) {}
    void accept(ast_visitor* visitor) override { visitor->visit(this); }
};

class block_stmt : public Expr {
public:
    std::vector<std::unique_ptr<Expr>> statements;
    block_stmt(std::vector<std::unique_ptr<Expr>> stmts) : statements(std::move(stmts)) {}
    void accept(ast_visitor* visitor) override { visitor->visit(this); }
};

class if_stmt : public Expr {
public:
    std::unique_ptr<Expr> condition;
    std::unique_ptr<Expr> then_branch;
    std::unique_ptr<Expr> else_branch;
    if_stmt(std::unique_ptr<Expr> cond, std::unique_ptr<Expr> then_b, std::unique_ptr<Expr> else_b)
        : condition(std::move(cond)), then_branch(std::move(then_b)), else_branch(std::move(else_b)) {}
    void accept(ast_visitor* visitor) override { visitor->visit(this); }
};

class while_stmt : public Expr {
public:
    std::unique_ptr<Expr> condition;
    std::unique_ptr<Expr> body;
    while_stmt(std::unique_ptr<Expr> cond, std::unique_ptr<Expr> b)
        : condition(std::move(cond)), body(std::move(b)) {}
    void accept(ast_visitor* visitor) override { visitor->visit(this); }
};

// NEW: Represents 'input()'
class input_expr : public Expr {
public:
    void accept(ast_visitor* visitor) override { visitor->visit(this); }
};

#endif
