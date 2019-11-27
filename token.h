#ifndef TOKENS_H_INCLUDED
#define TOKENS_H_INCLUDED

#include <map>
#include <memory>

enum TOKEN {TOKEN_RESERVED = -1, TOKEN_IDENTIFIER = 0, TOKEN_INT_LITERAL, TOKEN_STR_LITERAL, TOKEN_EOF, TOKEN_IF, TOKEN_WHILE,
               TOKEN_RETURN, TOKEN_VAR, TOKEN_PARENTHESIS_OPEN, TOKEN_PARENTHESIS_CLOSE, TOKEN_SQBRACKET_OPEN, TOKEN_SQBRACKET_CLOSE,
               TOKEN_CURLYBRACE_OPEN, TOKEN_CURLYBRACE_CLOSE, TOKEN_COMMA, TOKEN_SEMICOLON, TOKEN_COLON, TOKEN_QUESTIONMARK, TOKEN_PLUS,
               TOKEN_MINUS, TOKEN_EQUALS, TOKEN_PLUSEQUALS, TOKEN_MINUSEQUALS, TOKEN_NEGATE, TOKEN_NEGATEEQUALS, TOKEN_INCREMENT,
               TOKEN_DECREMENT, TOKEN_COMPARE, TOKEN_STAR, TOKEN_AMP, TOKEN_AND, TOKEN_OR};

const std::map<TOKEN,std::string> token_debug_names = {{TOKEN_IDENTIFIER, "Identifier"}, {TOKEN_INT_LITERAL, "Integer literal"},
               {TOKEN_STR_LITERAL, "String literal"}, {TOKEN_EOF, "End of file"}, {TOKEN_IF, "IF"}, {TOKEN_WHILE, "WHILE"},
               {TOKEN_RETURN, "RETURN"}, {TOKEN_VAR, "VAR"}, {TOKEN_PARENTHESIS_OPEN, "("}, {TOKEN_PARENTHESIS_CLOSE, ")"},
               {TOKEN_SQBRACKET_OPEN, "["}, {TOKEN_SQBRACKET_CLOSE, "]"}, {TOKEN_CURLYBRACE_OPEN, "{"}, {TOKEN_CURLYBRACE_CLOSE, "}"},
               {TOKEN_COMMA, ","}, {TOKEN_SEMICOLON, ";"}, {TOKEN_COLON, ":"}, {TOKEN_QUESTIONMARK, "?"}, {TOKEN_PLUS, "+"},
               {TOKEN_MINUS, "-"}, {TOKEN_EQUALS, "="}, {TOKEN_PLUSEQUALS, "+="}, {TOKEN_MINUSEQUALS, "-="}, {TOKEN_NEGATE, "!"},
               {TOKEN_NEGATEEQUALS, "!="}, {TOKEN_INCREMENT, "++"}, {TOKEN_DECREMENT, "--"}, {TOKEN_COMPARE, "=="}, {TOKEN_STAR, "*"},
               {TOKEN_AMP, "&"}, {TOKEN_AND, "&&"}, {TOKEN_OR, "||"}};

struct Token
{
    friend class DebugPrinter;

    TOKEN type;
    std::shared_ptr<std::string> str_val;
    std::shared_ptr<int> int_val;
    unsigned line_num;

    Token() : type(TOKEN_EOF), line_num(0) {}
    Token(TOKEN _type, unsigned _line_num) : type(_type), line_num(_line_num)
    {
        if (_type == TOKEN_IDENTIFIER || _type == TOKEN_STR_LITERAL)
            throw std::logic_error("Identifier or string literal token must be supplied with a string value");
        if (_type == TOKEN_INT_LITERAL)
            throw std::logic_error("Integer literal token must be supplied with an integer value");
    }
    Token(TOKEN _type, std::string _str_val, unsigned _line_num) : type(_type), line_num(_line_num)
    {
        if (_type != TOKEN_IDENTIFIER && _type != TOKEN_STR_LITERAL)
            throw std::logic_error("Wrong token type supplied with string value");
        else
            str_val = std::make_shared<std::string>(_str_val);
    }
    Token(TOKEN _type, int _int_val, unsigned _line_num) : line_num(_line_num)
    {
        if (_type != TOKEN_INT_LITERAL)
            throw std::logic_error("Integer literal tokens must have a TOKEN_INT_LITERAL token type");
        else
        {
            type = TOKEN_INT_LITERAL;
            int_val = std::make_shared<int>(_int_val);
        }
    }
    Token(int _int_val, unsigned _line_num) : type(TOKEN_INT_LITERAL), line_num(_line_num)
    {
        int_val = std::make_shared<int>(_int_val);
    }

    ~Token()
    {}

};


#endif // TOKENS_H_INCLUDED
