#ifndef H_DEBUGPRINT
#define H_DEBUGPRINT

#include <iostream>
#include <map>
#include <string>
#include <windows.h>
#include <stack>

#include "token.h"
#include "expression.h"
#include "statement.h"
#include "function.h"
#include "library.h"
#include "parsertoken.h"

using namespace std;

std::map<std::string, unsigned char> l = {{"vend", static_cast<unsigned char>(192)},
                                          {"vbranch", static_cast<unsigned char>(195)},
                                          {"hline", static_cast<unsigned char>(196)},
                                          {"vline", static_cast<unsigned char>(179)}};

std::map<EXPR_TYPE, std::string> expr_debug_names =
{{EXPR_TYPE::BIN_COMMA, ","},
{EXPR_TYPE::TERNARY, "?:"},
{EXPR_TYPE::BIN_EQUALS, "="},
{EXPR_TYPE::BIN_PLUSEQUALS, "+="},
{EXPR_TYPE::BIN_MINUSEQUALS, "-="},
{EXPR_TYPE::BIN_OR, "||"},
{EXPR_TYPE::BIN_AND, "&&"},
{EXPR_TYPE::BIN_COMPARE, "=="},
{EXPR_TYPE::BIN_NEGATEEQUALS, "!="},
{EXPR_TYPE::BIN_PLUS, "+"},
{EXPR_TYPE::BIN_MINUS, "-"},
{EXPR_TYPE::UNARY_AMP, "&"},
{EXPR_TYPE::UNARY_STAR, "*"},
{EXPR_TYPE::UNARY_NEGATE, "!"},
{EXPR_TYPE::UNARY_MINUS, "-u"},
{EXPR_TYPE::UNARY_PREINCR, "++pre"},
{EXPR_TYPE::UNARY_PREDECR, "--pre"},
{EXPR_TYPE::UNARY_POSTINCR, "post++"},
{EXPR_TYPE::UNARY_POSTDECR, "post--"},
{EXPR_TYPE::INT_LITERAL, "INT LITERAL"},
{EXPR_TYPE::STR_LITERAL, "STR LITERAL"},
{EXPR_TYPE::IDENTIFIER, "IDENTIFIER"},
{EXPR_TYPE::PARENTHESIS, "()"},
{EXPR_TYPE::INDEXING, "[]"},
{EXPR_TYPE::FUNC_CALL, "FCALL"},
{EXPR_TYPE::NONE, "NONE"}};

std::map<STATEMENT_TYPE, std::string> stmt_debug_names =
{{STATEMENT_TYPE::COMPOUND, "COMPOUND"},
 {STATEMENT_TYPE::CONDITIONAL, "CONDITIONAL"},
 {STATEMENT_TYPE::LOOP, "LOOP"},
 {STATEMENT_TYPE::RETURN, "RETURN"},
 {STATEMENT_TYPE::VAR_DEF, "VARIABLE DEFINITIONS"},
 {STATEMENT_TYPE::EXPRESSION, "EXPRESSION"},
 {STATEMENT_TYPE::NOP, "NOP"}};

class DebugPrinter
{
public:
    static void print_stack(std::stack<ParserToken> stack)
    {
        std::stack<ParserToken> rstack;
        while (!stack.empty())
        {
            rstack.push(stack.top());
            stack.pop();
        }

        int i = 0;
        while (!rstack.empty())
        {
            cout<<"["<<i<<"]:"<<endl;
            i++;
            ParserToken item = rstack.top();
            rstack.pop();
            switch (item.gettag())
            {
                case PARSERTOKEN::LIBRARY:
                    print_debug_library(*item.library);
                    break;
                case PARSERTOKEN::FUNCTION:
                    print_debug_function(*item.function);
                    break;
                case PARSERTOKEN::STATEMENT:
                    print_debug_statement(*item.statement);
                    break;
                case PARSERTOKEN::EXPRESSION:
                    print_debug_expr(*item.expression);
                    break;
                case PARSERTOKEN::TOKEN:
                    print_debug_token(*item.token);
                    break;
            }
        }
    }

    static void print_debug_library(Library library, int ident = 0)
    {
        cls();
        gotoxy(0, 0);
        cout<<tabs(ident)<<"LIBRARY"<<endl;

        for (auto it = library.functions.begin(); it != library.functions.end(); ++it)
        {
            if (it != --library.functions.end())
            {
                SHORT last_y = getxy().Y;
                print_debug_function(*it, ident+1);
                print_vline(ident, last_y+1);
            }
            else print_debug_function(*it, ident+1, true);

        }
    }

    static void print_debug_function(Function function, int ident = 0, bool last = false)
    {
        cout<<tabs(ident, last)<<"FUNCTION "<<"\""<<function.name<<"\""<<endl;
        for (auto it = function.params.begin(); it != function.params.end(); ++it)
            cout<<tabs(ident+1)<<"param: "<<*it<<endl;

        cout<<tabs(ident+1, true)<<"body "<<endl;
        print_debug_statement(function.body, ident+2, true);
    }

    static void print_debug_statement(Statement stmt, int ident = 0, bool last_stmt = false)
    {
        if (stmt.body && stmt.body->size() > 1)
            last_stmt = true;

        cout<<tabs(ident, last_stmt)<<"STATEMENT - "<<stmt_debug_names[stmt.type]<<endl;;

        switch (stmt.type)
        {
            case STATEMENT_TYPE::COMPOUND:
                for (auto it = stmt.body->begin(); it != stmt.body->end(); ++it)
                {
                    if (it != --stmt.body->end())
                    {
                        SHORT last_y = getxy().Y;
                        print_debug_statement(*it, ident+1);
                        print_vline(ident, last_y+1);
                    } else print_debug_statement(*it, ident+1, true);
                }
                break;
            case STATEMENT_TYPE::CONDITIONAL:
            case STATEMENT_TYPE::LOOP:
            {
                cout<<tabs(ident+1)<<"condition: "<<endl;

                SHORT last_y = getxy().Y;
                print_debug_expr(*stmt.expr, ident+2, true);
                print_vline(ident, last_y);

                cout<<tabs(ident+1, true)<<"body: "<<endl;
                print_debug_statement(stmt.body->back(), ident+2, true);
                break;
            }
            case STATEMENT_TYPE::RETURN:
                cout<<tabs(ident+1, true)<<"expression: "<<endl;
                print_debug_expr(*stmt.expr, ident+2, true);
                break;
            case STATEMENT_TYPE::VAR_DEF:
                cout<<tabs(ident+1, true)<<"variables: "<<endl;
                for (auto it = stmt.vars->begin(); it != stmt.vars->end(); ++it)
                    print_debug_vars(*it, ident+2);
                break;
            case STATEMENT_TYPE::EXPRESSION:
                cout<<tabs(ident+1, true)<<"expression: "<<endl;
                print_debug_expr(*stmt.expr, ident+2, true);
                break;
            case STATEMENT_TYPE::NOP:
                break;
        }

    }

    static void print_debug_vars(Variable var, int ident = 0)
    {}

    static void print_debug_expr(Expression expr, int ident = 0, bool last = false)
    {
        if (expr.expressions && expr.expressions->size() > 1)
            last = true;

        cout<<tabs(ident, last)<<"EXPRESSION - "<<expr_debug_names[expr.type];
        switch(expr.type)
        {
            case EXPR_TYPE::INT_LITERAL:
                cout<<", value: "<<*expr.int_val<<endl;
                break;
            case EXPR_TYPE::STR_LITERAL:
                cout<<", string: "<<"\""<<*expr.str_val<<"\""<<endl;
                break;
            case EXPR_TYPE::IDENTIFIER:
                cout<<", name: "<<*expr.str_val<<endl;
                break;
            case EXPR_TYPE::PARENTHESIS:
                cout<<endl;
                print_debug_expr(expr.expressions->back(), ident+1);
                break;
            case EXPR_TYPE::INDEXING:
            {
                cout<<endl;
                cout<<tabs(ident+1)<<"indexed: "<<endl;

                SHORT last_y = getxy().Y;
                print_debug_expr(expr.expressions->back(), ident+2, true);
                print_vline(ident, last_y);

                cout<<tabs(ident+1, true)<<"index: "<<endl;
                print_debug_expr(expr.expressions->front(), ident+2, true);
                break;
            }
            case EXPR_TYPE::FUNC_CALL:
            {
                cout<<endl;
                cout<<tabs(ident+1)<<"name: "<<endl;

                SHORT last_y = getxy().Y;
                print_debug_expr(expr.expressions->front(), ident+2, true);
                print_vline(ident, last_y);

                cout<<tabs(ident+1, true)<<"params: "<<endl;
                for (auto it = ++expr.expressions->begin(); it != expr.expressions->end(); ++it)
                {
                    if (it != --expr.expressions->end())
                        print_debug_expr(*it, ident+2);
                    else print_debug_expr(*it, ident+2, true);
                }
                break;
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
                cout<<endl;
                cout<<tabs(ident+1)<<"operand 1: "<<endl;

                SHORT last_y = getxy().Y;
                print_debug_expr(expr.expressions->back(), ident+2, true);
                print_vline(ident, last_y);

                cout<<tabs(ident+1, true)<<"operand 2: "<<endl;
                print_debug_expr(expr.expressions->front(), ident+2, true);
                break;
            }
            case EXPR_TYPE::UNARY_AMP:
            case EXPR_TYPE::UNARY_STAR:
            case EXPR_TYPE::UNARY_MINUS:
            case EXPR_TYPE::UNARY_NEGATE:
            case EXPR_TYPE::UNARY_POSTINCR:
            case EXPR_TYPE::UNARY_POSTDECR:
            case EXPR_TYPE::UNARY_PREINCR:
            case EXPR_TYPE::UNARY_PREDECR:
                cout<<endl;
                cout<<tabs(ident+1, true)<<"operand: "<<endl;
                print_debug_expr(expr.expressions->back(), ident+2, true);
                break;
            case EXPR_TYPE::TERNARY:
                cout<<endl;
                cout<<tabs(ident+1)<<"condition: "<<endl;

                SHORT last_y = getxy().Y;
                print_debug_expr(*++++expr.expressions->begin(), ident+2, true);
                print_vline(ident, last_y);

                cout<<tabs(ident+1)<<"true expr: "<<endl;

                last_y = getxy().Y;
                print_debug_expr(*++expr.expressions->begin(), ident+2, true);
                print_vline(ident, last_y);

                cout<<tabs(ident+1, true)<<"false expr: "<<endl;
                print_debug_expr(expr.expressions->front(), ident+2, true);
                break;
        }

    }

    static void print_debug(std::vector<ParserToken> ptokens)
    {
        for (std::vector<ParserToken>::iterator it = ptokens.begin(); it != ptokens.end(); ++it)
        {
            switch(it->gettag())
            {
                case PARSERTOKEN::TOKEN:
                    DebugPrinter::print_debug_token(*it->token);
                case PARSERTOKEN::EXPRESSION:
                    break;
            }
        }
    }

    static std::string tabs(int tabs, bool lastbranch = false)
    {
        std::string t;
        if (tabs)
        {
            for (int i = 0; i < tabs-1; ++i)
                t += "        ";

            if (lastbranch)
                t+= l["vend"];
            else t += l["vbranch"];

            for (int i = 0; i < 7; ++i)
                t += l["hline"];
        }
        return t;
    }

    static std::string vlines(int tabs, int i)
    {
        std:string t;
        gotoxy(static_cast<SHORT>(tabs*8), i);
        t += l["vline"];
        return t;
    }

    static void print_vline(int tabs, SHORT last_y)
    {
        COORD savedpos = getxy();
        for (SHORT i = last_y; i < savedpos.Y; ++i)
        {
            gotoxy(static_cast<SHORT>(tabs*8), i);
            cout<<l["vline"];
        }
        gotoxy(savedpos);
        return;
    }

    static void gotoxy(int x, int y)
    {
        COORD coord;
        coord.X = x;
        coord.Y = y;
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
    }

    static void gotoxy(COORD coord)
    {
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
    }

    static COORD getxy()
    {
        CONSOLE_SCREEN_BUFFER_INFO info;
        GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info);
        return info.dwCursorPosition;
    }

    static COORD getsize()
    {
        CONSOLE_SCREEN_BUFFER_INFO info;
        GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info);
        return info.dwSize;
    }

    static void cls()
    {
       COORD coordScreen = { 0, 0 };    // home for the cursor
       DWORD cCharsWritten;
       CONSOLE_SCREEN_BUFFER_INFO csbi;
       DWORD dwConSize;
       HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    // Get the number of character cells in the current buffer.

       if( !GetConsoleScreenBufferInfo( hConsole, &csbi ))
       {
          return;
       }

       dwConSize = csbi.dwSize.X * csbi.dwSize.Y;

       // Fill the entire screen with blanks.

       if( !FillConsoleOutputCharacter( hConsole,        // Handle to console screen buffer
                                        (TCHAR) ' ',     // Character to write to the buffer
                                        dwConSize,       // Number of cells to write
                                        coordScreen,     // Coordinates of first cell
                                        &cCharsWritten ))// Receive number of characters written
       {
          return;
       }

       // Get the current text attribute.

       if( !GetConsoleScreenBufferInfo( hConsole, &csbi ))
       {
          return;
       }

       // Set the buffer's attributes accordingly.

       if( !FillConsoleOutputAttribute( hConsole,         // Handle to console screen buffer
                                        csbi.wAttributes, // Character attributes to use
                                        dwConSize,        // Number of cells to set attribute
                                        coordScreen,      // Coordinates of first cell
                                        &cCharsWritten )) // Receive number of characters written
       {
          return;
       }

       // Put the cursor at its home coordinates.

       SetConsoleCursorPosition( hConsole, coordScreen );
    }

    static void print_debug_token(Token token, int ident = 0)
    {
        std::string tname = token_debug_names.at(token.type);
        cout<<"Token - "<<tname;
        switch (token.type)
        {
            case TOKEN_IDENTIFIER:
                cout<<*token.str_val;
                break;
            case TOKEN_STR_LITERAL:
                cout<<"\""<<*token.str_val<<"\"";
                break;
            case TOKEN_INT_LITERAL:
                cout<<*token.int_val;
                break;
            default:
                break;
        }
        cout<<endl;
    }
};

#endif // H_DEBUGPRINT
