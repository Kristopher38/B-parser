#ifndef LEXER_H_INCLUDED
#define LEXER_H_INCLUDED

#include <list>
#include <string>
#include <map>
#include <iostream>
#include <memory>
#include <vector>
#include <algorithm>

#include "token.h"

class Lexer
{
    const std::map<std::string,TOKEN> tokens = {{"if", TOKEN_IF}, {"while", TOKEN_WHILE}, {"return", TOKEN_RETURN}, {"var", TOKEN_VAR},
                                                {"(", TOKEN_PARENTHESIS_OPEN}, {")", TOKEN_PARENTHESIS_CLOSE},
                                                {"[", TOKEN_SQBRACKET_OPEN}, {"]", TOKEN_SQBRACKET_CLOSE},
                                                {"{", TOKEN_CURLYBRACE_OPEN}, {"}", TOKEN_CURLYBRACE_CLOSE},
                                                {",", TOKEN_COMMA}, {";", TOKEN_SEMICOLON}, {":", TOKEN_COLON},
                                                {"?", TOKEN_QUESTIONMARK}, {"+", TOKEN_PLUS}, {"-", TOKEN_MINUS},
                                                {"=", TOKEN_EQUALS}, {"+=", TOKEN_PLUSEQUALS}, {"-=", TOKEN_MINUSEQUALS},
                                                {"!", TOKEN_NEGATE}, {"!=", TOKEN_NEGATEEQUALS},
                                                {"++", TOKEN_INCREMENT}, {"--", TOKEN_DECREMENT}, {"==", TOKEN_COMPARE},
                                                {"*", TOKEN_STAR}, {"&", TOKEN_AMP}, {"&&", TOKEN_AND}, {"||", TOKEN_OR}};

    const std::vector<std::string> reserved_words = {"if", "while", "return", "var"};

    std::string* const stream;
    std::string::iterator stream_it;
    unsigned curr_line;

    inline bool istoken(std::string symbol) const
    {
        return tokens.count(symbol);
    }
    inline bool istoken(char c) const
    {
        return tokens.count(std::string(1, c));
    }
    inline bool isreserved(std::string symbol) const
    {
        if (std::find(reserved_words.begin(), reserved_words.end(), symbol) != reserved_words.end())
            return true;
        else return false;
    }
    inline bool isempty(std::string symbol) const
    {
        return !symbol.length();
    }
    inline bool isnum(std::string symbol) const
    {
        for (std::string::iterator it = symbol.begin(); it != symbol.end(); ++it)
        {
            if (!isdigit(*it))
                return false;
        }
        return true;
    }
    inline bool isnum(char c) const
    {
        return isdigit(c);
    }
    inline bool isstrliteralstart(const std::string& symbol) const
    {
        return symbol[0] == '"';
    }
    inline bool isstrliteralstart(char c) const
    {
        return c == '"';
    }
    inline bool iscommentstart(const std::string& symbol) const
    {
        return symbol.find("//") == 0 || symbol.find("/*") == 0;
    }
    inline bool isnewline(char c)
    {
        return c == '\n';
    }
    inline int stoi(std::string symbol)
    {
        std::stringstream ss(symbol);
        int value;
        ss>>value;
        return value;
    }
public:

    Lexer(std::string* const _stream) : stream(_stream),
                                        stream_it(stream->begin()),
                                        curr_line(1) {}

    Token next()
    {
        std::string symbol("");

        for (; stream_it != stream->end(); ++stream_it)
        {
            char c = *stream_it;

            if (!isempty(symbol))
            {
                // skip comments optimization
                std::string concat_symbol = symbol+std::string(1, c);
                if (iscommentstart(concat_symbol))
                {
                    std::size_t original_pos = std::distance(stream->begin(), stream_it);   // current stream_it position
                    std::size_t end_pos = std::string::npos;
                    if (concat_symbol.find("//") == 0)                  // for single-line comments
                        end_pos = stream->find("\n", original_pos);          // single-line comment end
                    if (concat_symbol.find("/*") == 0)                  // for multiline comments
                        end_pos = stream->find("*/", original_pos) + 1;      // multiline comment end
                    if (end_pos != std::string::npos)
                    {
                        std::string::iterator comment_start = stream_it;
                        std::advance(stream_it, end_pos - original_pos);     // advance to the first character past newline or comment end
                        curr_line += std::count(comment_start, stream_it+1, '\n'); // increment newlines
                        symbol = "";                                    // reset symbol
                        continue;
                    } else break;
                }

                // advance string literals
                if (isstrliteralstart(symbol))
                {
                    std::size_t original_pos = std::distance(stream->begin(), stream_it);
                    std::size_t str_end = stream->find("\"", original_pos);
                    std::string::iterator str_start = stream_it;
                    std::advance(stream_it, str_end - original_pos + 1);
                    curr_line += std::count(str_start, stream_it+1, '\n');
                    return Token(TOKEN_STR_LITERAL, stream->substr(original_pos, str_end - original_pos), curr_line);
                }

                if (istoken(symbol) && (!isreserved(symbol) || istoken(c) || isspace(c)))   // tokens
                {
                    if (istoken(concat_symbol))     // look one character ahead to determine if it's double-character operator
                    {
                        ++stream_it;
                        return Token(tokens.at(concat_symbol), curr_line);
                    }
                    else return Token(tokens.at(symbol), curr_line);
                }

                if (isspace(c) || istoken(c))       // separated by space or single-char tokens
                {
                    if (!isnum(symbol))             // identifiers
                        return Token(TOKEN_IDENTIFIER, symbol, curr_line);
                    else                            // integer literals
                        //return Token(TOKEN_INT_LITERAL, std::stoi(symbol), curr_line);
                        return Token(TOKEN_INT_LITERAL, stoi(symbol), curr_line);
                }

                /*if (((isspace(c) || istoken(c)) && istoken(symbol)) || (!isspace(c) && istoken(symbol) && !isreserved(symbol)))  // tokens separated by space or single-char tokens
                {
                    if (istoken(concat_symbol))     // look one character ahead to determine if it's double-character operator
                    {
                        ++stream_it;
                        return Token(tokens.at(concat_symbol));
                    }
                    else return Token(tokens.at(symbol));
                }
                if ((isspace(c) || istoken(c)) && !istoken(symbol) && !isnum(symbol))   // identifiers separated by space or single-char tokens
                {
                    return Token(TOKEN_IDENTIFIER, symbol);
                }
                if ((isspace(c) || istoken(c)) && isnum(symbol))    // integer literals separated by space or single-char tokens
                {
                    return Token(TOKEN_INT_LITERAL, std::stoi(symbol));
                }*/
            }
            if (!isspace(c))
                symbol += c;
            if (isnewline(c))
                curr_line++;
        }
        return Token(TOKEN_EOF, curr_line);
    }

    static std::list<Token> tokenize(std::string* const _stream)
    {
        std::list<Token> token_stream;
        Lexer l(_stream);
        Token t = l.next();
        while (t.type != TOKEN_EOF)
        {
            t = l.next();
            token_stream.push_back(t);
        }
        return token_stream;
    }

};

#endif // LEXER_H_INCLUDED
