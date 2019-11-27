#ifndef CONTEXT_H
#define CONTEXT_H

#include <map>
#include <vector>
#include <string>

#include "identifier.h"
#include "function.h"

class Context
{
    private:
        std::vector<std::map<std::string,Identifier>> scopes;
        std::vector<Function> func_list;


};

#endif // CONTEXT_H
