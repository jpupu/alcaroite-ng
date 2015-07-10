#include "lexer.hpp"



int main (int argc, char* argv[])
{
    while (true) {
        Token tok = scan(std::cin);
        std::cout << "Read token: " << tok.repr() << std::endl;
        if (tok.type == T_EOF) break;
    }
}

