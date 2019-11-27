#include "statement.h"

// Compound statement
Statement::Statement(std::vector<Statement> _statements) : Statement(STATEMENT_TYPE::COMPOUND, _statements) {}
Statement::Statement(STATEMENT_TYPE _type, std::vector<Statement> _statements)
{
    if (_type != STATEMENT_TYPE::COMPOUND)
        throw std::logic_error("Wrong statement type supplied with statement vector");
    else
    {
        type = _type;
        body = std::make_shared<std::vector<Statement>>(_statements);
    }
}

// Conditional or loop statement
Statement::Statement(STATEMENT_TYPE _type, Expression _condition_expr, Statement _statement)
{
    if (_type != STATEMENT_TYPE::CONDITIONAL && _type != STATEMENT_TYPE::LOOP)
        throw std::logic_error("Wrong statement type supplied with expression and statement");
    else
    {
        type = _type;
        body = std::make_shared<std::vector<Statement>>();
        body->push_back(_statement);
        expr = std::make_shared<Expression>(_condition_expr);
    }
}

// Return or expression statement
Statement::Statement(STATEMENT_TYPE _type, Expression _expr)
{
    if (_type != STATEMENT_TYPE::RETURN && _type != STATEMENT_TYPE::EXPRESSION)
        throw std::logic_error("Wrong statement type supplied with expression");
    else
    {
        type = _type;
        expr = std::make_shared<Expression>(_expr);
    }
}

// Variable definitions

Statement::Statement(std::vector<Variable> _vars) : Statement(STATEMENT_TYPE::VAR_DEF, _vars) {}
Statement::Statement(STATEMENT_TYPE _type, std::vector<Variable> _vars)
{
    if (_type != STATEMENT_TYPE::VAR_DEF)
        throw std::logic_error("Wrong statement type supplied with variable vector");
    else
    {
        type = _type;
        vars = std::make_shared<std::vector<Variable>>(_vars);
    }
}

// No-op statement
Statement::Statement() : type(STATEMENT_TYPE::NOP) {}
Statement::Statement(STATEMENT_TYPE _type)
{
    if (_type != STATEMENT_TYPE::NOP)
        throw std::logic_error("No arguments supplied to statement which isn't no-op statement");
    else
        type = _type;
}

void Statement::setbody(std::vector<Statement> _stmts)
{
    body = std::make_shared<std::vector<Statement>>(_stmts);
}

void Statement::setbody(Statement _stmt)
{
    body->clear();
    body->push_back(_stmt);
}

void Statement::setexpr(Expression _expr)
{
    expr = std::make_shared<Expression>(_expr);
}

void Statement::setvars(std::vector<Variable> _vars)
{
    vars = std::make_shared<std::vector<Variable>>(_vars);
}
