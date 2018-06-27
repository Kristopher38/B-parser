#include "expression.h"

std::map<EXPR_TYPE, int> op_prec = {{EXPR_TYPE::BIN_COMMA, 0},
                                {EXPR_TYPE::TERNARY, 1}, {EXPR_TYPE::BIN_EQUALS, 1},
                                {EXPR_TYPE::BIN_PLUSEQUALS, 1}, {EXPR_TYPE::BIN_MINUSEQUALS, 1},
                                {EXPR_TYPE::BIN_OR, 2},
                                {EXPR_TYPE::BIN_AND, 3},
                                {EXPR_TYPE::BIN_COMPARE, 4}, {EXPR_TYPE::BIN_NEGATEEQUALS, 4},
                                {EXPR_TYPE::BIN_PLUS, 5}, {EXPR_TYPE::BIN_MINUS, 5},
                                {EXPR_TYPE::UNARY_AMP, 6}, {EXPR_TYPE::UNARY_STAR, 6}, {EXPR_TYPE::UNARY_NEGATE, 6},
                                {EXPR_TYPE::UNARY_MINUS, 6}, {EXPR_TYPE::UNARY_PREINCR, 6}, {EXPR_TYPE::UNARY_PREDECR, 6},
                                {EXPR_TYPE::UNARY_POSTINCR, 7}, {EXPR_TYPE::UNARY_POSTDECR, 7},
                                    {EXPR_TYPE::INDEXING, 7}};

std::map<EXPR_TYPE, ASSOC> op_assoc = {{EXPR_TYPE::BIN_COMMA, ASSOC::LEFT},
                                       {EXPR_TYPE::TERNARY, ASSOC::RIGHT},
                                       {EXPR_TYPE::BIN_EQUALS, ASSOC::RIGHT},
                                       {EXPR_TYPE::BIN_PLUSEQUALS, ASSOC::RIGHT},
                                       {EXPR_TYPE::BIN_MINUSEQUALS, ASSOC::RIGHT},
                                       {EXPR_TYPE::BIN_OR, ASSOC::LEFT},
                                       {EXPR_TYPE::BIN_AND, ASSOC::LEFT},
                                       {EXPR_TYPE::BIN_COMPARE, ASSOC::LEFT},
                                       {EXPR_TYPE::BIN_NEGATEEQUALS, ASSOC::LEFT},
                                       {EXPR_TYPE::BIN_PLUS, ASSOC::LEFT},
                                       {EXPR_TYPE::BIN_MINUS, ASSOC::LEFT},
                                       {EXPR_TYPE::UNARY_AMP, ASSOC::RIGHT},
                                       {EXPR_TYPE::UNARY_STAR, ASSOC::RIGHT},
                                       {EXPR_TYPE::UNARY_NEGATE, ASSOC::RIGHT},
                                       {EXPR_TYPE::UNARY_MINUS, ASSOC::RIGHT},
                                       {EXPR_TYPE::UNARY_PREINCR, ASSOC::RIGHT},
                                       {EXPR_TYPE::UNARY_PREDECR, ASSOC::RIGHT},
                                       {EXPR_TYPE::UNARY_POSTINCR, ASSOC::LEFT},
                                       {EXPR_TYPE::UNARY_POSTDECR, ASSOC::LEFT},
                                       {EXPR_TYPE::INDEXING, ASSOC::LEFT}};

std::map<EXPR_TYPE, EXPR_OPCOUNT> op_opcount = {{EXPR_TYPE::BIN_COMMA, EXPR_OPCOUNT::BINARY},
                                                {EXPR_TYPE::TERNARY, EXPR_OPCOUNT::TERNARY},
                                                {EXPR_TYPE::BIN_EQUALS, EXPR_OPCOUNT::BINARY},
                                                {EXPR_TYPE::BIN_PLUSEQUALS, EXPR_OPCOUNT::BINARY},
                                                {EXPR_TYPE::BIN_MINUSEQUALS, EXPR_OPCOUNT::BINARY},
                                                {EXPR_TYPE::BIN_OR, EXPR_OPCOUNT::BINARY},
                                                {EXPR_TYPE::BIN_AND, EXPR_OPCOUNT::BINARY},
                                                {EXPR_TYPE::BIN_COMPARE, EXPR_OPCOUNT::BINARY},
                                                {EXPR_TYPE::BIN_NEGATEEQUALS, EXPR_OPCOUNT::BINARY},
                                                {EXPR_TYPE::BIN_PLUS, EXPR_OPCOUNT::BINARY},
                                                {EXPR_TYPE::BIN_MINUS, EXPR_OPCOUNT::BINARY},
                                                {EXPR_TYPE::UNARY_AMP, EXPR_OPCOUNT::UNARY},
                                                {EXPR_TYPE::UNARY_STAR, EXPR_OPCOUNT::UNARY},
                                                {EXPR_TYPE::UNARY_NEGATE, EXPR_OPCOUNT::UNARY},
                                                {EXPR_TYPE::UNARY_MINUS, EXPR_OPCOUNT::UNARY},
                                                {EXPR_TYPE::UNARY_PREINCR, EXPR_OPCOUNT::UNARY},
                                                {EXPR_TYPE::UNARY_PREDECR, EXPR_OPCOUNT::UNARY},
                                                {EXPR_TYPE::UNARY_POSTINCR, EXPR_OPCOUNT::UNARY},
                                                {EXPR_TYPE::UNARY_POSTDECR, EXPR_OPCOUNT::UNARY},
                                                {EXPR_TYPE::INT_LITERAL, EXPR_OPCOUNT::SINGLETOKEN},
                                                {EXPR_TYPE::STR_LITERAL, EXPR_OPCOUNT::SINGLETOKEN},
                                                {EXPR_TYPE::IDENTIFIER, EXPR_OPCOUNT::SINGLETOKEN},
                                                {EXPR_TYPE::PARENTHESIS, EXPR_OPCOUNT::GROUPING},
                                                {EXPR_TYPE::INDEXING, EXPR_OPCOUNT::GROUPING},
                                                {EXPR_TYPE::FUNC_CALL, EXPR_OPCOUNT::GROUPING},
                                                {EXPR_TYPE::NONE, EXPR_OPCOUNT::SINGLETOKEN}};

const std::map<EXPR_OPCOUNT, uint8_t> operand_count = {{EXPR_OPCOUNT::UNARY, 1}, {EXPR_OPCOUNT::BINARY, 2}, {EXPR_OPCOUNT::TERNARY, 3}};

Expression::Expression() : type(EXPR_TYPE::NONE), gentype(op_opcount[type]) {}

Expression::Expression(EXPR_TYPE _type, int _int_val)
{
    if (_type != EXPR_TYPE::INT_LITERAL)
        throw std::logic_error("Integer literal expressions must have INT_LITERAL expression type");
    else
    {
        type = EXPR_TYPE::INT_LITERAL;
        gentype = op_opcount[type];
        int_val = std::make_shared<int>(_int_val);
    }
}
Expression::Expression(int _int_val) : type(EXPR_TYPE::INT_LITERAL)
{
    int_val = std::make_shared<int>(_int_val);
    gentype = op_opcount[type];
}
Expression::Expression(EXPR_TYPE _type, std::string _str_val)
{
    if (_type != EXPR_TYPE::STR_LITERAL && _type != EXPR_TYPE::IDENTIFIER)
        throw std::logic_error("Wrong expression type supplied with string value");
    else
    {
        type = _type;
        gentype = op_opcount[type];
        str_val = std::make_shared<std::string>(_str_val);
    }
}

// Function calls
Expression::Expression(Expression _func_name, std::list<Expression> _args) : Expression(EXPR_TYPE::FUNC_CALL, _func_name, _args) {}
Expression::Expression(EXPR_TYPE _type, Expression _func_name, std::list<Expression> _args)
{
    if (_type != EXPR_TYPE::FUNC_CALL)
        throw std::logic_error("Wrong expression type supplied with arguments as for a function call expression");
    else
    {
        type = _type;
        gentype = op_opcount[type];
        expressions = std::make_shared<std::list<Expression>>();
        expressions->push_back(_func_name);
        expressions->insert(expressions->end(), _args.begin(), _args.end());
    }
}

// Parenthesis or unary expression
Expression::Expression(EXPR_TYPE _type, Expression _expr)
{
    init(_type, _expr);
}

void Expression::init(EXPR_TYPE _type, Expression _expr)
{
    if (op_opcount[_type] != EXPR_OPCOUNT::UNARY &&
        _type != EXPR_TYPE::PARENTHESIS)
            throw std::logic_error("Wrong expression type supplied with only one expression argument (parenthesis or unary expression)");
    else
    {
        type = _type;
        gentype = op_opcount[type];
        expressions = std::make_shared<std::list<Expression>>();
        expressions->push_back(_expr);
    }
}

// Indexing or binary expression
Expression::Expression(EXPR_TYPE _type, Expression _expr1, Expression _expr2)
{
    init(_type, _expr1, _expr2);
}

void Expression::init(EXPR_TYPE _type, Expression _expr1, Expression _expr2)
{
    if (op_opcount[_type] != EXPR_OPCOUNT::BINARY &&
        _type != EXPR_TYPE::INDEXING)
            throw std::logic_error("Wrong expression type supplied with two expression arguments (indexing or binary expression)");
    else
    {
        type = _type;
        gentype = op_opcount[type];
        expressions = std::make_shared<std::list<Expression>>();
        expressions->push_back(_expr1);
        expressions->push_back(_expr2);
    }
}

// Ternary expression
Expression::Expression(Expression _condition_expr, Expression _true_expr, Expression _false_expr) : Expression(EXPR_TYPE::TERNARY, _condition_expr, _true_expr, _false_expr) {}
Expression::Expression(EXPR_TYPE _type, Expression _condition_expr, Expression _true_expr, Expression _false_expr)
{
    init(_type, _condition_expr, _true_expr, _false_expr);
}

void Expression::init(EXPR_TYPE _type, Expression _condition_expr, Expression _true_expr, Expression _false_expr)
{
    if (_type != EXPR_TYPE::TERNARY)
        throw std::logic_error("Wrong expression type supplied with three expression arguments (ternary operator expression)");
    else
    {
        type = _type;
        gentype = op_opcount[type];
        expressions = std::make_shared<std::list<Expression>>();
        expressions->push_back(_condition_expr);
        expressions->push_back(_true_expr);
        expressions->push_back(_false_expr);
    }
}

// Only works for unary, binary and ternary expressions
Expression::Expression(EXPR_TYPE _type, std::vector<Expression> _operands)
{
    try
    {
        if (operand_count.at(op_opcount[_type]) != _operands.size())
            throw std::logic_error("Wrong number of expression arguments supplied to unary/binary/ternary expression constructor");
    }
    catch (const std::out_of_range& e)
    {
        throw std::logic_error("Wrong expression type supplied to unary/binary/ternary expression constructor");
    }

    switch (_operands.size())
    {
        case 1:
            init(_type, _operands[0]);
            break;
        case 2:
            init(_type, _operands[1], _operands[0]);
            break;
        case 3:
            init(_type, _operands[2], _operands[1], _operands[0]);
            break;
        default:
            throw std::logic_error("Wrong number of expression arguments supplied to unary/binary/ternary expression");
    }
}
