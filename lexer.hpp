
#include <iostream>
#include <string>


enum TokenType
{
    T_EOF,
    T_LPAREN,
    T_RPAREN,
    T_STRING,
    T_NUMBER,
    T_IDENTIFIER,
};

class Token
{
public:
    TokenType type;
    std::string s_val;
    double n_val;
    std::string lexeme;
    int line, column;

    Token (TokenType type, const std::string& lexeme);

    std::string repr () const;
};

Token scan (std::istream& is);
