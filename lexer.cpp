#include <stdexcept>

#include "lexer.hpp"


Token::Token (TokenType type, const std::string& lexeme)
    : type(type),
    n_val(0),
    lexeme(lexeme)
{
    switch (type) {
        case T_STRING:
            s_val = lexeme.substr(1, lexeme.size()-2);
            break;
        case T_NUMBER:
            n_val = std::stod(lexeme);
            break;
        case T_IDENTIFIER:
            s_val = lexeme;
            break;
        default:
            s_val = lexeme;
    }
}

std::string Token::repr () const
{
    std::string types[] = {"eof", "lparen", "rparen", "string", "number", "identifier"};
    switch (type) {
        case T_STRING:
        case T_IDENTIFIER:
            return "<"+types[type]+":"+s_val+">"; break;
        case T_NUMBER:
            return "<"+types[type]+":"+std::to_string(n_val)+">"; break;
        default:
            return "<"+types[type]+">"; break;
    }
}




enum LexerState
{
    S_START,
    S_INTEGER,
    S_DECIMAL,
    S_STRING,
    S_IDENTIFIER
};

Token scan (std::istream& is)
{
    LexerState state = S_START;
    std::string lexeme;

    while (true) {
        int c = is.get();
        printf("%d: %d %c\n", state, c, c);

        switch (state) {
            case S_START:
                if (isdigit(c)) {
                    lexeme += c;
                    state = S_INTEGER;
                }
                else if (c == '_' || isalpha(c)) {
                    lexeme += c;
                    state = S_IDENTIFIER;
                }
                else if (c == '(') {
                    return Token(T_LPAREN, "(");
                }
                else if (c == ')') {
                    return Token(T_RPAREN, ")");
                }
                else if (c == '"') {
                    lexeme += c;
                    state = S_STRING;
                }
                else if (isspace(c)) {
                    // nop
                }
                else if (c == -1) { 
                    return Token(T_EOF, lexeme);
                }
                else {
                    throw std::runtime_error("Expection something");
                }
                break;

            case S_INTEGER:
                if (isdigit(c)) {
                    lexeme += c;
                }
                else if (c == '.') {
                    lexeme += c;
                    state = S_DECIMAL;
                }
                else if (c == '(' || c == ')' || isspace(c)) {
                    is.unget();
                    return Token(T_NUMBER, lexeme);
                }
                else if (c == -1) {
                    throw std::runtime_error("End-of-file while reading an integer");
                }
                else {
                    throw std::runtime_error("Bad integer");
                }
                break;

            case S_DECIMAL:
                if (isdigit(c)) {
                    lexeme += c;
                }
                else if (c == '(' || c == ')' || isspace(c)) {
                    is.unget();
                    return Token(T_NUMBER, lexeme);
                }
                else if (c == -1) {
                    throw std::runtime_error("End-of-file while reading a decimal number");
                }
                else {
                    throw std::runtime_error("Bad decimal number");
                }
                break;

            case S_STRING:
                lexeme += c;
                if (c == '"') {
                    return Token(T_STRING, lexeme);
                }
                else if (c == -1) {
                    throw std::runtime_error("End-of-file while reading a quoted string");
                }
                break;

            case S_IDENTIFIER:
                if (isalnum(c)) {
                    lexeme += c;
                }
                else if (c == '(' || c == ')' || isspace(c)) {
                    is.unget();
                    return Token(T_IDENTIFIER, lexeme);
                }
                else if (c == -1) {
                    throw std::runtime_error("End-of-file while reading an identifier");
                }
                else {
                    throw std::runtime_error("Bad IDENTIFIER");
                }
                break;
        }
    }
}
