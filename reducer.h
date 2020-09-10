#ifndef REDUCER_H
#define REDUCER_H

#include <vector>
#include "parsertoken.h"
#include "library.h"
#include "function.h"
#include "statement.h"
#include "expression.h"
#include "identifier.h"

class Reducer
{
public:
    static Library library(std::vector<ParserToken>& ptokens)
    {
        std::vector<Function> func_vector;
        std::transform(ptokens.rbegin(), ptokens.rend(), std::back_inserter(func_vector), [](ParserToken ptoken) { return *ptoken.function; });
        return Library(func_vector);
    }

    static Function function(std::vector<ParserToken>& ptokens)
	{
        std::vector<Identifier> params;
        for (auto it = ptokens.rbegin() + 1; it != ptokens.rend() - 2; ++it)
            if (it->gettag() == PARSERTOKEN::TOKEN && it->token->type == TOKEN_IDENTIFIER)
                params.push_back(*it->token->str_val);
        return Function(*ptokens.back().token->str_val, params, *ptokens.front().statement);
    }

    static Statement statement_compound(std::vector<ParserToken>& ptokens)
    {
        std::vector<Statement> stmts;
        for (auto it = ptokens.rbegin(); it != ptokens.rend(); ++it)    // goes from left to right
            if (it->gettag() == PARSERTOKEN::STATEMENT)
                stmts.push_back(*it->statement);
        return Statement(stmts);
    }

    static Statement statement_if_while(std::vector<ParserToken>& ptokens)
    {
    }

    static Statement statement_return(std::vector<ParserToken>& ptokens)
    {
    }

    static ParserToken nop() {return ParserToken(Expression());}
};

#endif // REDUCER_H
