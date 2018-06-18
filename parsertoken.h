#ifndef H_PARSERTOKEN
#define H_PARSERTOKEN

#include <memory>

#include "token.h"
#include "lexer.h"
#include "expression.h"
#include "statement.h"
#include "function.h"
#include "library.h"

//#include "debugprinter.h"

//class DebugPrinter;

enum class PARSERTOKEN {LIBRARY, FUNCTION, STATEMENT, EXPRESSION, TOKEN};

struct ParserToken
{
    //friend class DebugPrinter;
private:
    PARSERTOKEN tag;
public:
    std::shared_ptr<Library> library;
    std::shared_ptr<Function> function;
    std::shared_ptr<Statement> statement;
    std::shared_ptr<Expression> expression;
    std::shared_ptr<Token> token;

    ParserToken(Token _token) : tag(PARSERTOKEN::TOKEN), token(std::make_shared<Token>(_token)) {}
    ParserToken(Statement _statement) : tag(PARSERTOKEN::STATEMENT), statement(std::make_shared<Statement>(_statement)) {}
    ParserToken(Expression _expression) : tag(PARSERTOKEN::EXPRESSION), expression(std::make_shared<Expression>(_expression)) {}
    ParserToken(Function _function) : tag(PARSERTOKEN::FUNCTION), function(std::make_shared<Function>(_function)) {}
    ParserToken(Library _library) : tag(PARSERTOKEN::LIBRARY), library(std::make_shared<Library>(_library)) {}

    PARSERTOKEN gettag() {return tag;}
};

#endif // H_PARSERTOKEN
