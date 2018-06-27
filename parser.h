#ifndef PARSER_H_INCLUDED
#define PARSER_H_INCLUDED

#include <stack>
#include <string>
#include <list>
#include <memory>

#include "lexer.h"
#include "token.h"
#include "lang_syntax.h"
#include "expression.h"
#include "statement.h"
#include "function.h"
#include "library.h"
#include "identifier.h"
#include "parsertoken.h"

#include "debugprinter.h"

class Parser
{
    std::stack<ParserToken> parser_stack;
    std::stack<int> return_stack;
    std::stack<int> reduce_stack;
    int current_state = 0;

    Action choose_action(Token lookahead_token)
    {
        return this->choose_action(current_state, lookahead_token.type);
    }

    Action choose_action(int _current_state, TOKEN lookahead_token_type)
    {
        Action a;
        try
        {
            a = grammar.at(Current(_current_state, lookahead_token_type));
        }
        catch (const std::exception& e)
        {
            try
            {
                a = grammar.at(Current(_current_state));
            }
            catch (const std::exception& e)
            {
                cout<<"STATE MACHINE ERROR"<<endl;
                cout<<_current_state<<" "<<token_debug_names.at(lookahead_token_type)<<endl;
                throw e;
            }
        }
        return a;
    }

    Expression reduce_expression(EXPR_TYPE rule, std::list<ParserToken>& ptokens)
    {
        switch(rule)
        {
            case EXPR_TYPE::INT_LITERAL:
                return Expression(rule, *ptokens.back().token->int_val);
            case EXPR_TYPE::STR_LITERAL:
            case EXPR_TYPE::IDENTIFIER:
                return Expression(rule, *ptokens.back().token->str_val);
            case EXPR_TYPE::PARENTHESIS:
            {
                ptokens.pop_back();
                return Expression(rule, *ptokens.back().expression);
            }
            case EXPR_TYPE::INDEXING:
            {
                Expression lhs = *ptokens.back().expression;
                ptokens.pop_back();
                ptokens.pop_back();
                return Expression(rule, lhs, *ptokens.back().expression);
            }
            case EXPR_TYPE::FUNC_CALL:
            {
                Expression func_name = *ptokens.back().expression;
                ptokens.pop_back();
                ptokens.pop_back();
                std::list<Expression> func_args;
                for (std::list<ParserToken>::reverse_iterator it = ptokens.rbegin(); it != ptokens.rend(); ++it)
                    if (it->gettag() == PARSERTOKEN::EXPRESSION)
                        func_args.push_back(*it->expression);

                return Expression(rule, func_name, func_args);
            }
            case EXPR_TYPE::BIN_EQUALS:
            case EXPR_TYPE::BIN_PLUS:
            case EXPR_TYPE::BIN_MINUS:
            case EXPR_TYPE::BIN_PLUSEQUALS:
            case EXPR_TYPE::BIN_MINUSEQUALS:
            case EXPR_TYPE::BIN_OR:
            case EXPR_TYPE::BIN_AND:
            case EXPR_TYPE::BIN_COMPARE:
            case EXPR_TYPE::BIN_NEGATEEQUALS:
            case EXPR_TYPE::BIN_COMMA:
            {
                Expression lhs = *ptokens.back().expression;
                ptokens.pop_back();
                ptokens.pop_back();
                return Expression(rule, lhs, *ptokens.back().expression);
            }
            case EXPR_TYPE::UNARY_AMP:
            case EXPR_TYPE::UNARY_STAR:
            case EXPR_TYPE::UNARY_MINUS:
            case EXPR_TYPE::UNARY_NEGATE:
            case EXPR_TYPE::UNARY_PREINCR:
            case EXPR_TYPE::UNARY_PREDECR:
            {
                ptokens.pop_back();
                return Expression(rule, *ptokens.back().expression);
            }
            case EXPR_TYPE::UNARY_POSTINCR:
            case EXPR_TYPE::UNARY_POSTDECR:
            {
                return Expression(rule, *ptokens.back().expression);
            }
            case EXPR_TYPE::TERNARY:
            {
                Expression cond = *ptokens.back().expression;
                ptokens.pop_back();
                ptokens.pop_back();
                Expression true_expr = *ptokens.back().expression;
                ptokens.pop_back();
                ptokens.pop_back();
                return Expression(cond, true_expr, *ptokens.back().expression);
            }
        }
    }

    Function reduce_function(std::list<ParserToken>& ptokens)
    {
        std::list<Identifier> params;
        for (std::list<ParserToken>::reverse_iterator it = ptokens.rbegin(); it != ptokens.rend(); ++it)
            if (it->gettag() == PARSERTOKEN::TOKEN && it->token->type == TOKEN_IDENTIFIER)
                params.push_back(*it->token->str_val);

        Identifier name = params.front();
        params.pop_front();
        Statement body(*ptokens.front().statement);

        return Function(name, params, body);
    }

    Library reduce_library(std::list<ParserToken>& ptokens)
    {
        ptokens.pop_front();
        std::list<Function> func_list;
        for (std::list<ParserToken>::reverse_iterator it = ptokens.rbegin(); it != ptokens.rend(); ++it)
            func_list.push_back(Function(*it->function));

        return Library(func_list);
    }

    Statement reduce_statement(STATEMENT_TYPE rule, std::list<ParserToken>& ptokens)
    {
        switch(rule)
        {
            case STATEMENT_TYPE::COMPOUND:
            {
                std::list<Statement> stmts;
                for (std::list<ParserToken>::reverse_iterator it = ptokens.rbegin(); it != ptokens.rend(); ++it)    // goes from left to right
                    if (it->gettag() == PARSERTOKEN::STATEMENT)
                        stmts.push_back(*it->statement);
                return stmts;
            }
            case STATEMENT_TYPE::CONDITIONAL:
            case STATEMENT_TYPE::LOOP:
            {
                ptokens.pop_back();
                ptokens.pop_back();
                Expression expr = *ptokens.back().expression;
                ptokens.pop_back();
                ptokens.pop_back();
                Statement body = *ptokens.back().statement;
                return Statement(rule, expr, body);
            }
            case STATEMENT_TYPE::RETURN:
            {
                ptokens.pop_back();
                return Statement(rule, *ptokens.back().expression);
            }
            case STATEMENT_TYPE::VAR_DEF:
            {
                ptokens.pop_back();
                std::list<Variable> vars;
                for (std::list<ParserToken>::reverse_iterator it = ptokens.rbegin(); it != ptokens.rend(); ++it)    // goes from left to right
                {
                    if (it->gettag() == PARSERTOKEN::TOKEN && it->token->type == TOKEN_IDENTIFIER)
                    {
                        Identifier ident = *it->token->str_val;
                        std::advance(it, 2);
                        if (it->gettag() == PARSERTOKEN::EXPRESSION)
                            vars.push_back(Variable(ident, *it->expression));
                        else
                        {
                            vars.push_back(Variable(ident));
                            --it;
                            continue;
                        }
                    }
                }
                return Statement(rule, vars);
            }
            case STATEMENT_TYPE::EXPRESSION:
            {
                return Statement(rule, *ptokens.back().expression);
            }
            case STATEMENT_TYPE::NOP:
                return Statement(rule);
        }
    }

    ParserToken reduce(Goal goal, std::list<ParserToken> ptokens)   // note: order of tokens is from left to right
    {
        switch(goal.goal)
        {
            case GOAL::STATEMENT:
                return ParserToken(reduce_statement(goal.statement, ptokens));
            case GOAL::EXPRESSION:
                return ParserToken(reduce_expression(goal.expr, ptokens));
            case GOAL::FUNCTION:
                return ParserToken(reduce_function(ptokens));
            case GOAL::LIBRARY:
                return ParserToken(reduce_library(ptokens));
            case GOAL::NONE:
                break;
        }
    }

    std::list<Expression> rpn_traverse_tree(Expression to_traverse)
    {
        std::list<Expression> parents_list;

        if (op_opcount[to_traverse.type] != EXPR_OPCOUNT::SINGLETOKEN &&
            op_opcount[to_traverse.type] != EXPR_OPCOUNT::GROUPING)
        {
            parents_list.push_back(to_traverse);
            for (auto it = to_traverse.expressions->begin(); it != to_traverse.expressions->end(); ++it)
            {
                if (op_opcount[to_traverse.type] == EXPR_OPCOUNT::TERNARY && it == ++to_traverse.expressions->begin())
                {
                    *it = rpn_expr(Expression(EXPR_TYPE::PARENTHESIS, *it));
                }
                std::list<Expression> traversed = rpn_traverse_tree(*it);
                parents_list.insert(parents_list.end(), traversed.begin(), traversed.end());
            }
        }
        return parents_list;
    }

    Expression rpn_expr(Expression to_transform)
    {
        // making operator and operand list phase
        std::list<Expression> parents_stack;

        // if we're operating on a single token just return it unchanged
        if (op_opcount[to_transform.type] == EXPR_OPCOUNT::SINGLETOKEN)
            return to_transform;

        // if we have a grouping expression we need to make sure expressions
        // inside it are properly ordered by applying rpn_expr on them
        else if (op_opcount[to_transform.type] == EXPR_OPCOUNT::GROUPING)
        {
            for (auto it = to_transform.expressions->begin(); it != to_transform.expressions->end(); ++it)
                *it = rpn_expr(*it);
            return to_transform;    // after applying precedence rules to expressions inside return grouping expression
        }

        // if it's not a grouping or single token expression, it's an
        // expression that needs to be ordered by applying precedence rules
        else
            parents_stack = rpn_traverse_tree(to_transform);

        // rpn-ordering phase
        std::stack<Expression> operator_stack;
        std::stack<Expression> output_stack;
        unsigned insert_offset = 0;

        operator_stack.push(parents_stack.front());    // push the operator at the top of the tree in an expression
        while (!parents_stack.empty())
        {
            Expression current_expr = parents_stack.front();
            parents_stack.pop_front();
            for (std::list<Expression>::iterator current_subexpr = current_expr.expressions->begin(); current_subexpr != current_expr.expressions->end(); ++current_subexpr)
            {
                //output_stack.push(*current_subexpr);
                if (op_opcount[current_subexpr->type] == EXPR_OPCOUNT::SINGLETOKEN ||
                    op_opcount[current_subexpr->type] == EXPR_OPCOUNT::GROUPING)
                {
                    std::stack<Expression> temp;
                    if (insert_offset)
                    {
                        while (insert_offset)
                        {
                            temp.push(output_stack.top());
                            output_stack.pop();
                            insert_offset--;
                        }
                        output_stack.push(*current_subexpr);
                        while (!temp.empty())
                        {
                            output_stack.push(temp.top());
                            temp.pop();
                        }
                    }
                    else
                        output_stack.push(*current_subexpr);
                }
                else
                {
                    while (
                           !operator_stack.empty() &&
                           ((op_prec[current_subexpr->type] < op_prec[operator_stack.top().type]) ||
                           ((op_prec[current_subexpr->type] == op_prec[operator_stack.top().type]) && (op_assoc[operator_stack.top().type] == ASSOC::LEFT)))
                          )
                    {
                        output_stack.push(operator_stack.top());
                        operator_stack.pop();
                        insert_offset++;
                    }
                    operator_stack.push(*current_subexpr);

                }
            }
        }

        while (!operator_stack.empty())
        {
            Expression test = operator_stack.top();
            output_stack.push(test);
            operator_stack.pop();
        }

        // transforming to AST phase
        std::stack<Expression> rpn_stack;
        while (!output_stack.empty())
        {
            rpn_stack.push(output_stack.top());
            output_stack.pop();
        }

        std::stack<Expression> result_stack;
        while (!rpn_stack.empty())
        {
            Expression token = rpn_stack.top();
            rpn_stack.pop();

            if (op_opcount[token.type] != EXPR_OPCOUNT::SINGLETOKEN &&
                op_opcount[token.type] != EXPR_OPCOUNT::GROUPING)
            {
                std::vector<Expression> exprs;
                for (uint8_t i = 0; i < operand_count.at(op_opcount[token.type]); ++i)
                {
                    exprs.push_back(result_stack.top());
                    result_stack.pop();
                }
                result_stack.push(Expression(token.type, exprs));
            }
            else
                result_stack.push(token);
        }

        return result_stack.top();
    }

public:

    Parser()
    {
        return_stack.push(-1);
        reduce_stack.push(-1);
        reduce_stack.push(0);
    }
    ~Parser()
    {}

    void feed(Token lookahead_token)
    {
        Action action;
        while (action.next_action != ACTION::SHIFT)
        {
            action = this->choose_action(lookahead_token);
            switch (action.next_action)
            {
                // shift is for situations where we want to return after shifting
                // that is: when we are NOT pushing the last terminal in a path
                // except if we are pushing last terminal in an expression
                // (since expressions can recursively call themselves)
                case ACTION::SHIFT:
                    parser_stack.push(ParserToken(lookahead_token));
                    reduce_stack.top()++;
                    current_state = action.next_state;
                    return;

                // jump is for situations where we don't want to return after shifting
                // that is: when we are pushing the last terminal in a path after
                // which a reduce occurs (i.e: semicolon in a return statement)
                // except if we are pushing last terminal in an expression
                // (since expressions can recursively call themselves)
                case ACTION::JUMP:
                    current_state = action.next_state;  // jump to state
                    parser_stack.push(ParserToken(lookahead_token));
                    reduce_stack.top()++;
                    break;

                case ACTION::CALL_NONTERM:
                    current_state = action.next_state;  // get nonterminal state
                    reduce_stack.push(0);
                    return_stack.push(action.return_state);    // push return pathstate
                    break;

                case ACTION::CALL_NONTERM_REC:
                    current_state = action.next_state;
                    reduce_stack.push(1);
                    return_stack.push(action.return_state);
                    break;

                case ACTION::REDUCE:
                    if (action.next_goal.goal != GOAL::NONE)
                    {
                        //DebugPrinter::print_stack(parser_stack, true);
                        std::vector<ParserToken> to_reduce;

                        for (int i = reduce_stack.top(); i > 0; --i)
                        {
                            to_reduce.push_back(parser_stack.top());
                            parser_stack.pop();
                        }


                        std::list<ParserToken> to_reduce2;
                        for (int i = 0; i < to_reduce.size(); ++i)
                            to_reduce2.push_back(to_reduce[i]);

                        ParserToken reduced_token = this->reduce(action.next_goal, to_reduce2);
                        parser_stack.push(reduced_token);

                        current_state = action.return_state;

                        if (action.next_goal.goal == GOAL::STATEMENT && parser_stack.top().statement->expr)
                        {
                            Expression temp = *parser_stack.top().statement->expr;
                            *parser_stack.top().statement->expr = rpn_expr(temp);
                        }
                        if (action.next_goal.goal == GOAL::EXPRESSION && op_opcount[action.next_goal.expr] == EXPR_OPCOUNT::GROUPING)
                        {
                            Expression temp = *parser_stack.top().expression;
                            *parser_stack.top().expression = rpn_expr(temp);
                        }

                        if (action.return_state < 0)
                        {
                            current_state = return_stack.top();
                            return_stack.pop();
                            reduce_stack.pop();
                            reduce_stack.top()++;
                            return;
                        }
                    }
                    else
                    {
                        current_state = return_stack.top();
                        return_stack.pop();
                        reduce_stack.pop();
                        reduce_stack.top()++;
                    }
                break;
            }
        }
    }

    Library finish()
    {
        if (!parser_stack.empty() &&
            parser_stack.top().gettag() == PARSERTOKEN::LIBRARY)
            return *parser_stack.top().library;
        else return Library();
    }
};

#endif // PARSER_H_INCLUDED
