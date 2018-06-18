#ifndef H_STATEMENT
#define H_STATEMENT

#include <memory>
#include <list>

#include "expression.h"
#include "identifier.h"

enum class STATEMENT_TYPE {COMPOUND, CONDITIONAL, LOOP, RETURN, VAR_DEF, EXPRESSION, NOP};

struct Variable
{
    friend class DebugPrinter;

    const bool is_initialized;
    Identifier name;
    std::shared_ptr<Expression> expr;

    Variable(Identifier _name) : is_initialized(false), name(_name) {}
    Variable(Identifier _name, Expression _expr) : is_initialized(true), name(_name), expr(std::make_shared<Expression>(_expr)) {}
};

struct Statement
{
    friend class DebugPrinter;

public:
    STATEMENT_TYPE type;
    std::shared_ptr<std::list<Statement>> body;   // compound, conditional or loop statement
    std::shared_ptr<Expression> expr;   // conditional, loop, return or expression statement
    std::shared_ptr<std::list<Variable>> vars;

public:
    // Compound statement
	Statement(std::list<Statement> _statements);
    Statement(STATEMENT_TYPE _type, std::list<Statement> _statements);

    // Conditional or loop statement
    Statement(STATEMENT_TYPE _type, Expression _condition_expr, Statement _statement);
    // Return or expression statement
    Statement(STATEMENT_TYPE _type, Expression _expr);

    // Variable definitions
    Statement(std::list<Variable> _vars);
    Statement(STATEMENT_TYPE _type, std::list<Variable> _vars);

    // No-op statement
    Statement();
    Statement(STATEMENT_TYPE _type);

    void setbody(std::list<Statement> stmts);
    void setbody(Statement stmt);
    void setexpr(Expression expr);
    void setvars(std::list<Variable> vars);
};

#endif // H_STATEMENT
