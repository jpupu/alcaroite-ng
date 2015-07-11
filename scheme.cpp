#include "lexer.hpp"

#include <exception>
#include <memory>

using std::shared_ptr;
using std::make_shared;

// enum SObjType
// {
//     // O_BOOLEAN,
//     // O_PAIR,
//     O_SYMBOL,
//     O_NUMBER,
//     // O_CHAR,
//     O_STRING,
//     // O_VECTOR,
//     // O_PORT,
//     // O_PROCEDURE,
//     // O_NIL,
// };



struct Object
{
    enum TypeTag {
        pair_type,
        symbol_type,
        number_type,
        string_type,
        nil_type,
    };
    TypeTag type;

protected:
    Object (TypeTag type) : type(type) {}
    virtual ~Object () {}
};

struct Pair : public Object
{
    Pair (shared_ptr<Object> car, shared_ptr<Object> cdr)
        : Object(pair_type), car(car), cdr(cdr) {}
    shared_ptr<Object> car;
    shared_ptr<Object> cdr;
};

struct String : public Object
{
    String (const std::string& v) : Object(string_type), value(v) {}
    std::string value;
};

struct Symbol : public Object
{
    Symbol (const std::string& v) : Object(symbol_type), value(v) {}
    const std::string value;
};

struct Number : public Object
{
    Number (double v) : Object(number_type), value(v) {}
    const double value;
};

struct Nil : public Object
{
    Nil () : Object(nil_type) {}
};

shared_ptr<Nil> nil_constant = make_shared<Nil>();

// struct Value
// {
//     enum Mark {
//         pointer_mark = 0,
//         integer_mark = 1,
//         double_mark = 2,
//         nil_mark = 3
//     };
//     TypeTag type_tag;
//     union {
//         Value* value_ptr;
//         int integer_data;
//         double double_data;
//         std::string* symbol_ptr;
//     };


shared_ptr<Object> read_list ();

shared_ptr<Object> read (const Token& tok)
{
    switch (tok.type) {
        case T_STRING: return make_shared<String>(tok.s_val);
        case T_IDENTIFIER: return make_shared<Symbol>(tok.s_val);
        case T_NUMBER: return make_shared<Number>(tok.n_val);
        case T_LPAREN: return read_list();
        case T_RPAREN:
            throw std::runtime_error("unexpected closing paren");
        case T_EOF: return make_shared<Nil>(); // FIXME
    }
}

shared_ptr<Object> read_list ()
{
    Token tok = scan(std::cin);
    if (tok.type == T_RPAREN) {
        return nil_constant;
    }
    else if (tok.type == T_EOF) {
        throw;
    }
    return make_shared<Pair>(read(tok), read_list());
}

/// Reads syntax datum and returns object.
shared_ptr<Object> read ()
{
    return read(scan(std::cin));
}

void print (std::ostream& stream, const shared_ptr<Object>& obj);

/// (internal) Used in printing a list/pair to stream.
void print_tail (std::ostream& stream, const shared_ptr<Pair>& obj, bool initial = false)
{
    if (initial) { stream << '('; }
    print(stream, obj->car);
    if (obj->cdr == nil_constant) {
        stream << ')';
    }
    else if (obj->cdr->type == Object::pair_type) {
        stream << ' ';
        print_tail(stream, std::dynamic_pointer_cast<Pair>(obj->cdr));
    }
    else {
        stream << " . ";
        print(stream, obj->cdr);
        stream << ')';
    }
}

/// Prints objects external representation to stream.
void print (std::ostream& stream, const shared_ptr<Object>& obj)
{
    switch (obj->type) {
        case Object::pair_type: {
            auto o = std::dynamic_pointer_cast<Pair>(obj);
            print_tail(stream, o, true);
            // stream << '(' << o->car << " . " << o->cdr << ')';
            break;
        }
        case Object::symbol_type: {
            auto o = std::dynamic_pointer_cast<Symbol>(obj);
            stream << o->value;
            break;
        }
        case Object::number_type: {
            auto o = std::dynamic_pointer_cast<Number>(obj);
            stream << o->value;
            break;
        }
        case Object::string_type: {
            auto o = std::dynamic_pointer_cast<String>(obj);
            stream << o->value;
            break;
        }
        case Object::nil_type: {
            auto o = std::dynamic_pointer_cast<Nil>(obj);
            stream << "'()";
            break;
        }
    }
}

std::ostream& operator<< (std::ostream& stream, const shared_ptr<Object>& obj)
{
    print(stream, obj);
    return stream;
}


int main (int argc, char* argv[])
{
    std::cout << "> ";
    auto x = read();
    std::cout << "==> " << x << std::endl; 
    // while (true) {
    //     Token tok = scan(std::cin);
    //     std::cout << "Read token: " << tok.repr() << std::endl;
    //     if (tok.type == T_EOF) break;
    // }
}

