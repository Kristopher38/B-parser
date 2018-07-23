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

enum class PARSERTOKEN {LIBRARY, FUNCTION, STATEMENT, EXPRESSION, TOKEN, ERR};

struct ParserToken
{
    //friend class DebugPrinter;
private:
    PARSERTOKEN tag;
    bool ill_formed;
public:
    std::shared_ptr<Library> library;
    std::shared_ptr<Function> function;
    std::shared_ptr<Statement> statement;
    std::shared_ptr<Expression> expression;
    std::shared_ptr<Token> token;

    ParserToken(bool _ill_formed = false) : tag(PARSERTOKEN::ERR), ill_formed(_ill_formed) {}
    ParserToken(Token _token, bool _ill_formed = false) : tag(PARSERTOKEN::TOKEN), ill_formed(_ill_formed), token(std::make_shared<Token>(_token)) {}
    ParserToken(Statement _statement, bool _ill_formed = false) : tag(PARSERTOKEN::STATEMENT), ill_formed(_ill_formed), statement(std::make_shared<Statement>(_statement)) {}
    ParserToken(Expression _expression, bool _ill_formed = false) : tag(PARSERTOKEN::EXPRESSION), ill_formed(_ill_formed), expression(std::make_shared<Expression>(_expression)) {}
    ParserToken(Function _function, bool _ill_formed = false) : tag(PARSERTOKEN::FUNCTION), ill_formed(_ill_formed), function(std::make_shared<Function>(_function)) {}
    ParserToken(Library _library, bool _ill_formed = false) : tag(PARSERTOKEN::LIBRARY), ill_formed(_ill_formed), library(std::make_shared<Library>(_library)) {}

    PARSERTOKEN gettag() {return tag;}
    bool is_ill_formed() {return ill_formed;}
};

#endif // H_PARSERTOKEN
