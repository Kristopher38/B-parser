#ifndef LANG_SYNTAX_H_INCLUDED
#define LANG_SYNTAX_H_INCLUDED

#include <unordered_map>
#include "token.h"
#include "statement.h"
#include "expression.h"

enum class ACTION {JUMP, CALL_NONTERM, SHIFT, REDUCE, CALL_NONTERM_REC};

enum class GOAL {LIBRARY, FUNCTION, STATEMENT, EXPRESSION, NONE};

struct Goal
{
    GOAL goal;
    union
    {
        STATEMENT_TYPE statement;
        EXPR_TYPE expr;
    };

    Goal(STATEMENT_TYPE _statement) : goal(GOAL::STATEMENT), statement(_statement) {}
    Goal(EXPR_TYPE _expr) : goal(GOAL::EXPRESSION), expr(_expr) {}
    Goal(GOAL _goal)
    {
        if (_goal == GOAL::EXPRESSION || _goal == GOAL::STATEMENT)
            throw std::logic_error("No statement or expression type supplied for goal");
        else
            goal = _goal;
    }
    Goal() : goal(GOAL::NONE) {}


};

struct Action
{
    ACTION next_action;
    int next_state;
    int return_state;
    Goal next_goal;

    Action() : next_action(ACTION::JUMP), next_state(-1), return_state(-1), next_goal(Goal()) {}

    Action(ACTION _next_action, int _next_state, int _return_state = -1)
        : next_action(_next_action), next_state(_next_state), return_state(_return_state) {}

    Action(ACTION _next_action, Goal _next_goal = Goal(), int _return_state = -1)
        : next_action(_next_action), next_goal(_next_goal), return_state(_return_state) {}

};

struct Current
{
    int current_state;
    TOKEN lookahead_token;

    Current(int _current_state) : current_state(_current_state), lookahead_token(TOKEN_RESERVED) {}
    Current(int _current_state, TOKEN _lookahead_token) : current_state(_current_state), lookahead_token(_lookahead_token) {}

    bool operator==(const Current& other) const
    {
        return (this->current_state == other.current_state &&
                this->lookahead_token == other.lookahead_token);
    }
};

struct CurrentHasher
{
    std::size_t operator()(const Current& k) const
    {
        using std::size_t;
        using std::hash;
        using std::string;

        return (hash<int>()(k.current_state)) ^ (hash<int>()(k.lookahead_token) << 1);
    }
};

/* Note on recursive expression parsing:
 * When we're in state 78 (end of expression parsing), we're checking
 * for tokens as if we were in state 42, to allow for recursive
 * expressions. State 78 checks for every possible token available
 * also in state 42 and if it founds one it calls action
 * CALL_NONTERM_REC, which pushes additional '1' value onto
 * the reduce stack (not '0' because we've already parsed one expression
 * so we will need to reduce 1 more additional token), pushes one
 * additional return address onto the return stack (which is itself 78
 * to recursively loop around) and goes to state 42, in which then
 * continues normal operation of shifting/calling nonterminals.
 * If in state 78 it hasn't found any of tokens available in state 42,
 * it calls action REDUCE with goal NONE, which simply means "return"
 * (pop value from return stack to current state, pop reduce stack
 * (since we've already reduced nonterminal in action before) and
 * increment reduce stack (since we've just reduced, there's newly
 * reduced element on top which wasn't counted in reduce stack before)*/

const std::unordered_map<Current, Action, CurrentHasher> grammar =
{
    {
        Current(0),
        Action(ACTION::CALL_NONTERM, 1, 93)
    },
    {
        Current(93),
        Action(ACTION::CALL_NONTERM, 1, 93)
    },
    {
        Current(0, TOKEN_EOF),
        Action(ACTION::SHIFT, 2)
    },
    {
        Current(1, TOKEN_IDENTIFIER),
        Action(ACTION::SHIFT, 4)
    },
    {
        Current(93, TOKEN_EOF),
        Action(ACTION::JUMP, 2)
    },
    {
        Current(2),
        Action(ACTION::REDUCE, Goal(GOAL::LIBRARY))
    },
    {
        Current(4, TOKEN_IDENTIFIER),
        Action(ACTION::SHIFT, 5)
    },
    {
        Current(4, TOKEN_COLON),
        Action(ACTION::SHIFT, 7)
    },
    {
        Current(5, TOKEN_COMMA),
        Action(ACTION::SHIFT, 6)
    },
    {
        Current(5, TOKEN_COLON),
        Action(ACTION::SHIFT, 7)
    },
    {
        Current(6, TOKEN_IDENTIFIER),
        Action(ACTION::SHIFT, 5)
    },
    {
        Current(7),
        Action(ACTION::CALL_NONTERM, 8, 94)
    },
    {
        Current(94),
        Action(ACTION::REDUCE, Goal(GOAL::FUNCTION), 9)
    },
    {
        Current(9),
        Action(ACTION::REDUCE, Goal(GOAL::NONE))
    },
    {
        Current(8, TOKEN_CURLYBRACE_OPEN),
        Action(ACTION::SHIFT, 10)
    },
    {
        Current(8, TOKEN_IF),
        Action(ACTION::SHIFT, 13)
    },
    {
        Current(8, TOKEN_WHILE),
        Action(ACTION::SHIFT, 18)
    },
    {
        Current(8, TOKEN_RETURN),
        Action(ACTION::SHIFT, 23)
    },
    {
        Current(8, TOKEN_VAR),
        Action(ACTION::SHIFT, 26)
    },
    {
        Current(8),
        Action(ACTION::CALL_NONTERM, 11, 32)
    },
    {
        Current(8, TOKEN_SEMICOLON),
        Action(ACTION::SHIFT, 34)
    },
    {
        Current(10),
        Action(ACTION::CALL_NONTERM, 8, 95)
    },
    {
        Current(95),
        Action(ACTION::CALL_NONTERM, 8, 95)
    },
    {
        Current(95, TOKEN_CURLYBRACE_CLOSE),
        Action(ACTION::JUMP, 12)
    },
    {
        Current(10, TOKEN_CURLYBRACE_CLOSE),
        Action(ACTION::SHIFT, 12)
    },
    {
        Current(12),
        Action(ACTION::REDUCE, Goal(STATEMENT_TYPE::COMPOUND))
    },
    {
        Current(13, TOKEN_PARENTHESIS_OPEN),
        Action(ACTION::SHIFT, 14)
    },
    {
        Current(14),
        Action(ACTION::CALL_NONTERM, 11, 15)
    },
    {
        Current(15, TOKEN_PARENTHESIS_CLOSE),
        Action(ACTION::SHIFT, 16)
    },
    {
        Current(16),
        Action(ACTION::CALL_NONTERM, 8, 17)
    },
    {
        Current(17),
        Action(ACTION::REDUCE, Goal(STATEMENT_TYPE::CONDITIONAL), 35)
    },
    {
        Current(18, TOKEN_PARENTHESIS_OPEN),
        Action(ACTION::SHIFT, 19)
    },
    {
        Current(19),
        Action(ACTION::CALL_NONTERM, 11, 20)
    },
    {
        Current(20, TOKEN_PARENTHESIS_CLOSE),
        Action(ACTION::SHIFT, 21)
    },
    {
        Current(21),
        Action(ACTION::CALL_NONTERM, 8, 22)
    },
    {
        Current(22),
        Action(ACTION::REDUCE, Goal(STATEMENT_TYPE::LOOP), 35)
    },
    {
        Current(23),
        Action(ACTION::CALL_NONTERM, 11, 24)
    },
    {
        Current(24, TOKEN_SEMICOLON),
        Action(ACTION::JUMP, 25)
    },
    {
        Current(25),
        Action(ACTION::REDUCE, Goal(STATEMENT_TYPE::RETURN))
    },
    {
        Current(26, TOKEN_IDENTIFIER),
        Action(ACTION::SHIFT, 27)
    },
    {
        Current(27, TOKEN_SEMICOLON),
        Action(ACTION::SHIFT, 31)
    },
    {
        Current(27, TOKEN_COMMA),
        Action(ACTION::SHIFT, 28)
    },
    {
        Current(27, TOKEN_EQUALS),
        Action(ACTION::SHIFT, 29)
    },
    {
        Current(28, TOKEN_IDENTIFIER),
        Action(ACTION::SHIFT, 27)
    },
    {
        Current(29),
        Action(ACTION::CALL_NONTERM, 11, 30)
    },
    {
        Current(30, TOKEN_COMMA),
        Action(ACTION::SHIFT, 28)
    },
    {
        Current(30, TOKEN_SEMICOLON),
        Action(ACTION::JUMP, 31)
    },
    {
        Current(31),
        Action(ACTION::REDUCE, Goal(STATEMENT_TYPE::VAR_DEF))
    },
    {
        Current(32, TOKEN_SEMICOLON),
        Action(ACTION::JUMP, 33)
    },
    {
        Current(33),
        Action(ACTION::REDUCE, Goal(STATEMENT_TYPE::EXPRESSION))
    },
    {
        Current(34),
        Action(ACTION::REDUCE, Goal(STATEMENT_TYPE::NOP))
    },
    {
        Current(35),
        Action(ACTION::REDUCE, Goal(GOAL::NONE))
    },
    {
        Current(11, TOKEN_INT_LITERAL),
        Action(ACTION::SHIFT, 36)
    },
    {
        Current(11, TOKEN_STR_LITERAL),
        Action(ACTION::SHIFT, 37)
    },
    {
        Current(11, TOKEN_IDENTIFIER),
        Action(ACTION::SHIFT, 38)
    },
    {
        Current(11, TOKEN_PARENTHESIS_OPEN),
        Action(ACTION::SHIFT, 39)
    },
    {
        Current(11),
        Action(ACTION::CALL_NONTERM, 11, 42)
    },
    {
        Current(11, TOKEN_AMP),
        Action(ACTION::SHIFT, 63)
    },
    {
        Current(11, TOKEN_STAR),
        Action(ACTION::SHIFT, 64)
    },
    {
        Current(11, TOKEN_MINUS),
        Action(ACTION::SHIFT, 65)
    },
    {
        Current(11, TOKEN_NEGATE),
        Action(ACTION::SHIFT, 66)
    },
    {
        Current(11, TOKEN_INCREMENT),
        Action(ACTION::SHIFT, 67)
    },
    {
        Current(11, TOKEN_DECREMENT),
        Action(ACTION::SHIFT, 68)
    },
    {
        Current(36),
        Action(ACTION::REDUCE, Goal(EXPR_TYPE::INT_LITERAL), 78)
    },
    {
        Current(37),
        Action(ACTION::REDUCE, Goal(EXPR_TYPE::STR_LITERAL), 78)
    },
    {
        Current(38),
        Action(ACTION::REDUCE, Goal(EXPR_TYPE::IDENTIFIER), 78)
    },
    {
        Current(39),
        Action(ACTION::CALL_NONTERM, 11, 96)
    },
    {
        Current(41),
        Action(ACTION::REDUCE, Goal(EXPR_TYPE::PARENTHESIS), 78)
    },
    {
        Current(42, TOKEN_SQBRACKET_OPEN),
        Action(ACTION::SHIFT, 43)
    },
    {
        Current(42, TOKEN_PARENTHESIS_OPEN),
        Action(ACTION::SHIFT, 47)
    },
    {
        Current(42, TOKEN_EQUALS),
        Action(ACTION::SHIFT, 52)
    },
    {
        Current(42, TOKEN_PLUS),
        Action(ACTION::SHIFT, 53)
    },
    {
        Current(42, TOKEN_MINUS),
        Action(ACTION::SHIFT, 54)
    },
    {
        Current(42, TOKEN_PLUSEQUALS),
        Action(ACTION::SHIFT, 55)
    },
    {
        Current(42, TOKEN_MINUSEQUALS),
        Action(ACTION::SHIFT, 56)
    },
    {
        Current(42, TOKEN_OR),
        Action(ACTION::SHIFT, 57)
    },
    {
        Current(42, TOKEN_AND),
        Action(ACTION::SHIFT, 58)
    },
    {
        Current(42, TOKEN_COMPARE),
        Action(ACTION::SHIFT, 59)
    },
    {
        Current(42, TOKEN_NEGATEEQUALS),
        Action(ACTION::SHIFT, 60)
    },
    {
        Current(42, TOKEN_COMMA),
        Action(ACTION::SHIFT, 61)
    },
    {
        Current(42, TOKEN_INCREMENT),
        Action(ACTION::SHIFT, 71)
    },
    {
        Current(42, TOKEN_DECREMENT),
        Action(ACTION::SHIFT, 72)
    },
    {
        Current(42, TOKEN_QUESTIONMARK),
        Action(ACTION::SHIFT, 74)
    },
    {
        Current(42),
        Action(ACTION::REDUCE, Goal(GOAL::NONE))
    },
    {
        Current(43),
        Action(ACTION::CALL_NONTERM, 11, 44)
    },
    {
        Current(44, TOKEN_SQBRACKET_CLOSE),
        Action(ACTION::SHIFT, 45)
    },
    {
        Current(45),
        Action(ACTION::REDUCE, Goal(EXPR_TYPE::INDEXING), 78)
    },
    {
        Current(47, TOKEN_PARENTHESIS_CLOSE),
        Action(ACTION::SHIFT, 50)
    },
    {
        Current(47),
        Action(ACTION::CALL_NONTERM, 11, 48)
    },
    {
        Current(48, TOKEN_COMMA),
        Action(ACTION::SHIFT, 49)
    },
    {
        Current(48, TOKEN_PARENTHESIS_CLOSE),
        Action(ACTION::SHIFT, 50)
    },
    {
        Current(49),
        Action(ACTION::CALL_NONTERM, 11, 48)
    },
    {
        Current(50),
        Action(ACTION::REDUCE, Goal(EXPR_TYPE::FUNC_CALL), 78)
    },
    {
        Current(52),
        Action(ACTION::CALL_NONTERM, 11, 62)
    },
    {
        Current(53),
        Action(ACTION::CALL_NONTERM, 11, 79)
    },
    {
        Current(54),
        Action(ACTION::CALL_NONTERM, 11, 80)
    },
    {
        Current(55),
        Action(ACTION::CALL_NONTERM, 11, 81)
    },
    {
        Current(56),
        Action(ACTION::CALL_NONTERM, 11, 82)
    },
    {
        Current(57),
        Action(ACTION::CALL_NONTERM, 11, 83)
    },
    {
        Current(58),
        Action(ACTION::CALL_NONTERM, 11, 84)
    },
    {
        Current(59),
        Action(ACTION::CALL_NONTERM, 11, 85)
    },
    {
        Current(60),
        Action(ACTION::CALL_NONTERM, 11, 86)
    },
    {
        Current(61),
        Action(ACTION::CALL_NONTERM, 11, 87)
    },
    {
        Current(62),
        Action(ACTION::REDUCE, Goal(EXPR_TYPE::BIN_EQUALS), 78)
    },
    {
        Current(79),
        Action(ACTION::REDUCE, Goal(EXPR_TYPE::BIN_PLUS), 78)
    },
    {
        Current(80),
        Action(ACTION::REDUCE, Goal(EXPR_TYPE::BIN_MINUS), 78)
    },
    {
        Current(81),
        Action(ACTION::REDUCE, Goal(EXPR_TYPE::BIN_PLUSEQUALS), 78)
    },
    {
        Current(82),
        Action(ACTION::REDUCE, Goal(EXPR_TYPE::BIN_MINUSEQUALS), 78)
    },
    {
        Current(83),
        Action(ACTION::REDUCE, Goal(EXPR_TYPE::BIN_OR), 78)
    },
    {
        Current(84),
        Action(ACTION::REDUCE, Goal(EXPR_TYPE::BIN_AND), 78)
    },
    {
        Current(85),
        Action(ACTION::REDUCE, Goal(EXPR_TYPE::BIN_COMPARE), 78)
    },
    {
        Current(86),
        Action(ACTION::REDUCE, Goal(EXPR_TYPE::BIN_NEGATEEQUALS), 78)
    },
    {
        Current(87),
        Action(ACTION::REDUCE, Goal(EXPR_TYPE::BIN_COMMA), 78)
    },
    {
        Current(63),
        Action(ACTION::CALL_NONTERM, 11, 69)
    },
    {
        Current(64),
        Action(ACTION::CALL_NONTERM, 11, 88)
    },
    {
        Current(65),
        Action(ACTION::CALL_NONTERM, 11, 89)
    },
    {
        Current(66),
        Action(ACTION::CALL_NONTERM, 11, 90)
    },
    {
        Current(67),
        Action(ACTION::CALL_NONTERM, 11, 91)
    },
    {
        Current(68),
        Action(ACTION::CALL_NONTERM, 11, 92)
    },
    {
        Current(69),
        Action(ACTION::REDUCE, Goal(EXPR_TYPE::UNARY_AMP), 78)
    },
    {
        Current(88),
        Action(ACTION::REDUCE, Goal(EXPR_TYPE::UNARY_STAR), 78)
    },
    {
        Current(89),
        Action(ACTION::REDUCE, Goal(EXPR_TYPE::UNARY_MINUS), 78)
    },
    {
        Current(90),
        Action(ACTION::REDUCE, Goal(EXPR_TYPE::UNARY_NEGATE), 78)
    },
    {
        Current(91),
        Action(ACTION::REDUCE, Goal(EXPR_TYPE::UNARY_PREINCR), 78)
    },
    {
        Current(92),
        Action(ACTION::REDUCE, Goal(EXPR_TYPE::UNARY_PREDECR), 78)
    },
    {
        Current(71),
        Action(ACTION::REDUCE, Goal(EXPR_TYPE::UNARY_POSTINCR), 78)
    },
    {
        Current(72),
        Action(ACTION::REDUCE, Goal(EXPR_TYPE::UNARY_POSTDECR), 78)
    },
    {
        Current(74),
        Action(ACTION::CALL_NONTERM, 11, 75)
    },
    {
        Current(75, TOKEN_COLON),
        Action(ACTION::SHIFT, 76)
    },
    {
        Current(76),
        Action(ACTION::CALL_NONTERM, 11, 77)
    },
    {
        Current(77),
        Action(ACTION::REDUCE, Goal(EXPR_TYPE::TERNARY), 78)
    },
    {
        Current(96, TOKEN_PARENTHESIS_CLOSE),
        Action(ACTION::SHIFT, 41)
    },
    {
        Current(78, TOKEN_SQBRACKET_OPEN),
        Action(ACTION::CALL_NONTERM_REC, 42, 78)
    },
    {
        Current(78, TOKEN_PARENTHESIS_OPEN),
        Action(ACTION::CALL_NONTERM_REC, 42, 78)
    },
    {
        Current(78, TOKEN_EQUALS),
        Action(ACTION::CALL_NONTERM_REC, 42, 78)
    },
    {
        Current(78, TOKEN_PLUS),
        Action(ACTION::CALL_NONTERM_REC, 42, 78)
    },
    {
        Current(78, TOKEN_MINUS),
        Action(ACTION::CALL_NONTERM_REC, 42, 78)
    },
    {
        Current(78, TOKEN_PLUSEQUALS),
        Action(ACTION::CALL_NONTERM_REC, 42, 78)
    },
    {
        Current(78, TOKEN_MINUSEQUALS),
        Action(ACTION::CALL_NONTERM_REC, 42, 78)
    },
    {
        Current(78, TOKEN_OR),
        Action(ACTION::CALL_NONTERM_REC, 42, 78)
    },
    {
        Current(78, TOKEN_AND),
        Action(ACTION::CALL_NONTERM_REC, 42, 78)
    },
    {
        Current(78, TOKEN_COMPARE),
        Action(ACTION::CALL_NONTERM_REC, 42, 78)
    },
    {
        Current(78, TOKEN_NEGATEEQUALS),
        Action(ACTION::CALL_NONTERM_REC, 42, 78)
    },
    {
        Current(78, TOKEN_COMMA),
        Action(ACTION::CALL_NONTERM_REC, 42, 78)
    },
    {
        Current(78, TOKEN_INCREMENT),
        Action(ACTION::CALL_NONTERM_REC, 42, 78)
    },
    {
        Current(78, TOKEN_DECREMENT),
        Action(ACTION::CALL_NONTERM_REC, 42, 78)
    },
    {
        Current(78, TOKEN_QUESTIONMARK),
        Action(ACTION::CALL_NONTERM_REC, 42, 78)
    },
    {
        Current(78),
        Action(ACTION::REDUCE, Goal(GOAL::NONE))
    }
};

#endif // LANG_SYNTAX_H_INCLUDED
