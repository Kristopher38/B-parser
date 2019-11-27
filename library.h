#ifndef H_LIBRARY
#define H_LIBRARY

#include <vector>

struct Library
{
    friend class DebugPrinter;

    std::vector<Function> functions;

    Library(std::vector<Function> _functions) : functions(_functions) {}
    Library() : functions(std::vector<Function>()) {}
};

#endif // H_LIBRARY
