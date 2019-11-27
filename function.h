#ifndef H_FUNCTION
#define H_FUNCTION

#include "statement.h"
#include "identifier.h"

struct Function
{
    friend class DebugPrinter;

    Identifier name;
    std::vector<Identifier> params;
    Statement body;

    Function(Identifier _name, Statement _body) : name(_name), body(_body) {}
    Function(Identifier _name, std::vector<Identifier> _params, Statement _body) : name(_name), params(_params), body(_body) {}
};

#endif // H_FUNCTION
