#ifndef H_LIBRARY
#define H_LIBRARY

struct Library
{
    friend class DebugPrinter;

    std::list<Function> functions;

    Library(std::list<Function> _functions) : functions(_functions) {}
    Library() : functions(std::list<Function>()) {}
};

#endif // H_LIBRARY
