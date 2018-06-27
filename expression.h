#ifndef H_EXPRESSION
#define H_EXPRESSION

#include <string>
#include <list>
#include <memory>
#include <map>
#include <vector>

enum class EXPR_TYPE {NONE, INT_LITERAL, STR_LITERAL, IDENTIFIER, PARENTHESIS, INDEXING, FUNC_CALL,
                    BIN_EQUALS, BIN_PLUS, BIN_MINUS, BIN_PLUSEQUALS, BIN_MINUSEQUALS,
                    BIN_OR, BIN_AND, BIN_COMPARE, BIN_NEGATEEQUALS, BIN_COMMA,
                    UNARY_AMP, UNARY_STAR, UNARY_MINUS, UNARY_NEGATE, UNARY_PREINCR,
                    UNARY_PREDECR, UNARY_POSTINCR, UNARY_POSTDECR,
                    TERNARY};

enum class EXPR_OPCOUNT {UNARY, BINARY, TERNARY, GROUPING, SINGLETOKEN};

enum class ASSOC {LEFT, RIGHT};

extern std::map<EXPR_TYPE, int> op_prec;
extern std::map<EXPR_TYPE, ASSOC> op_assoc;
extern std::map<EXPR_TYPE, EXPR_OPCOUNT> op_opcount;
extern const std::map<EXPR_OPCOUNT, uint8_t> operand_count;

struct Expression
{
    friend class DebugPrinter;

    EXPR_TYPE type;
    EXPR_OPCOUNT gentype;
    std::shared_ptr<int> int_val;
    std::shared_ptr<std::string> str_val;
    std::shared_ptr<std::list<Expression>> expressions;

    Expression();

    Expression(EXPR_TYPE _type, int _int_val);
    Expression(int _int_val);
    Expression(EXPR_TYPE _type, std::string _str_val);

    // Function calls
    Expression(Expression _func_name, std::list<Expression> _args);
    Expression(EXPR_TYPE _type, Expression _func_name, std::list<Expression> _args);

    // Parenthesis or unary expression
    Expression(EXPR_TYPE _type, Expression _expr);
    void init(EXPR_TYPE _type, Expression _expr);

    // Indexing or binary expression
    Expression(EXPR_TYPE _type, Expression _expr1, Expression _expr2);
    void init(EXPR_TYPE _type, Expression _expr1, Expression _expr2);

    // Ternary expression
    Expression(Expression _condition_expr, Expression _true_expr, Expression _false_expr);
    Expression(EXPR_TYPE _type, Expression _condition_expr, Expression _true_expr, Expression _false_expr);
    void init(EXPR_TYPE _type, Expression _condition_expr, Expression _true_expr, Expression _false_expr);

    // all-in-one for unary, binary and ternary expressions
    Expression(EXPR_TYPE _type, std::vector<Expression> _operands);
};

#endif // H_EXPRESSION
