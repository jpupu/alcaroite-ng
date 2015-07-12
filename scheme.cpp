#include "lexer.hpp"

#include <exception>
#include <memory>
#include <vector>

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
};

struct Pair : Object
{
    Object* car;
    Object* cdr;

    Pair (Object* car, Object* cdr)
        : Object(pair_type), car(car), cdr(cdr) {}
    static size_t total_size (Object* car, Object* cdr) { return sizeof(Pair); }
};

struct String : Object
{
    /// length of string, excluding terminating null character.
    const int len;
    /// null terminated string data.
    char data[];

    /// len = length of string, excluding terminating null character.
    /// source = string data, need not be null terminated.
    String (int len, const char* source)
        : Object(string_type), len(len)
    {
        memcpy(data, source, len);
        data[len] = '\0';
    }
    static size_t total_size (int len, const char* source) { return sizeof(String) + len + 1; }
};

struct Symbol : Object
{
    const int len;
    char data[];

    /// len = length of string, excluding terminating null character.
    /// source = string data, need not be null terminated.
    Symbol (int len, const char* source)
        : Object(symbol_type), len(len)
    {
        memcpy(data, source, len);
        data[len] = '\0';
    }
    static size_t total_size (int len, const char* source) { return sizeof(Symbol) + len + 1; }
};

struct Number : Object
{
    const double value;

    Number (double v) : Object(number_type), value(v) {}
    static size_t total_size (double v) { return sizeof(Number); }
};

struct Nil : Object
{
    Nil () : Object(nil_type) {}
    static size_t total_size () { return sizeof(Nil); }
};

// shared_ptr<Nil> nil_constant = make_shared<Nil>();

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


class GarbageCollector
{
private:
    std::vector<char> generic_pool;
    // std::vector<Pair> pair_pool;

public:
    /// Sizes are given in bytes.
    GarbageCollector (size_t pair_pool_size, size_t generic_pool_size)
    {
        generic_pool.reserve(generic_pool_size);
        // pair_pool.reserve(pair_pool_size / sizeof(Pair));
    }

    // Pair* new_pair (Object* car, Object* cdr)
    // {
    //     if (pair_pool.size() == pair_pool.capacity()) {
    //         collect();
    //         if (pair_pool.size() == pair_pool.capacity()) {
    //             throw std::runtime_error("GC: pair pool full!");
    //         }
    //     }
    //     pair_pool.emplace_back(car, cdr);
    //     return &pair_pool.back();
    // }

    void collect ()
    {
        // TODO
    }

    void* allocate (size_t alignment, size_t size)
    {
        std::cout << "GC: allocating " << size << " bytes (" << alignment << " byte alignemnt)\n"; 

        size_t point = generic_pool.size();
        // Move point so it's properly aligned.
        point += alignment - 1 - (point - 1) % alignment;
        
        // Collect garbage if not enough capacity.
        if (point + size >= generic_pool.capacity()) {
            collect();

            // If capacity is still insufficient, we're SOL.
            if (point + size >= generic_pool.capacity()) {
                throw std::runtime_error("GC: out of memory");
            }
        }

        // We have capacity, so allocate the memory.
        generic_pool.resize(point + size);
        return static_cast<void*>(generic_pool.data() + point);
    }

    // template<class T, class... Args>
    // T* make_object (Args&&... args)
    // {
    //     T* ptr = static_cast<T*>(allocate(alignof(T),
    //         T::total_size(std::forward<Args>(args));
    //     return new(ptr) T(std::forward<Args>(args));
    // }

    // Pair* make_pair (Object* car, Object* cdr)
    // {
    //     auto p = static_cast<Pair*>(allocate(alignof(Pair), sizeof(Pair)));
    //     p->car = car;
    //     p->cdr = cdr;
    //     return p;
    // }

    // String* make_string (const std::string& s)
    // {
    //     auto p = static_cast<String*>(allocate(alignof(String), sizeof(String + s.size())));
    //     p.len = s.size();
    //     memcpy(p.data, s.c_str(), s.size());
    //     return p;
    // }

    // Number* make_number (double v)
    // {
    //     auto p = static_cast<Number*>(allocate(alignof(Number), sizeof(Number)));
    //     p->value = v;
    //     return p;
    // }


// template<class T, class... U>
// std::unique_ptr<T> make_unique(U&&... u)
// {
//     return std::unique_ptr<T>(new T(std::forward<U>(u)...));
// }

};

template<class T, class... Args>
T* make_object (GarbageCollector& gc, Args&&... args)
{
    size_t size = T::total_size(std::forward<Args>(args)...);
    void* ptr = gc.allocate(alignof(T), size);
    // T* ptr = static_cast<T*>(gc.allocate(alignof(T), size));
        // T::total_size(std::forward<Args>(args))));
    return new(ptr) T(std::forward<Args>(args)...);
}

Nil* nil_constant = nullptr;

// template<class T>
// T* allocate<T> ()
// {
//     // FIXME: take alignment into account!!
//     if (generic_pool.size() + size >= generic_pool.capacity()) {
//         collect();
//         if (generic_pool.size() + size >= generic_pool.capacity()) {
//             throw std::runtime_error("GC: out of memory");
//         }
//     }

//     return static_cast<T*>(generic_pool.back()
// }

// template<class T, class... Args>
// shared_ptr<T> make_object (GarbageCollector& gc, Args&&... args)
// {
//     gc.reserve<T>(sizeof(T));
// }


Object* read_list (GarbageCollector&);

Object* read (GarbageCollector& gc, const Token& tok)
{
    switch (tok.type) {
        case T_STRING: return make_object<String>(gc, tok.s_val.size(), tok.s_val.data());
        case T_IDENTIFIER: return make_object<Symbol>(gc, tok.s_val.size(), tok.s_val.data());
        case T_NUMBER: return make_object<Number>(gc, tok.n_val);
        case T_LPAREN: return read_list(gc);
        case T_RPAREN:
            throw std::runtime_error("unexpected closing paren");
        case T_EOF: return make_object<Nil>(gc); // FIXME
    }
}

Object* read_list (GarbageCollector& gc)
{
    Token tok = scan(std::cin);
    if (tok.type == T_RPAREN) {
        return nil_constant;
    }
    else if (tok.type == T_EOF) {
        throw;
    }
    return make_object<Pair>(gc, read(gc, tok), read_list(gc));
}

/// Reads syntax datum and returns object.
Object* read (GarbageCollector& gc)
{
    return read(gc, scan(std::cin));
}

void print (std::ostream& stream, const Object* obj);

/// (internal) Used in printing a list/pair to stream.
void print_tail (std::ostream& stream, const Pair* obj, bool initial = false)
{
    if (initial) { stream << '('; }
    print(stream, obj->car);
    if (obj->cdr == nil_constant) {
        stream << ')';
    }
    else if (obj->cdr->type == Object::pair_type) {
        stream << ' ';
        print_tail(stream, static_cast<Pair*>(obj->cdr));
    }
    else {
        stream << " . ";
        print(stream, obj->cdr);
        stream << ')';
    }
}

/// Prints objects external representation to stream.
void print (std::ostream& stream, const Object* obj)
{
    switch (obj->type) {
        case Object::pair_type: {
            auto o = static_cast<const Pair*>(obj);
            print_tail(stream, o, true);
            // stream << '(' << o->car << " . " << o->cdr << ')';
            break;
        }
        case Object::symbol_type: {
            auto o = static_cast<const Symbol*>(obj);
            stream << o->data;
            break;
        }
        case Object::number_type: {
            auto o = static_cast<const Number*>(obj);
            stream << o->value;
            break;
        }
        case Object::string_type: {
            auto o = static_cast<const String*>(obj);
            stream << o->data;
            break;
        }
        case Object::nil_type: {
            stream << "'()";
            break;
        }
    }
}

std::ostream& operator<< (std::ostream& stream, const Object* obj)
{
    print(stream, obj);
    return stream;
}


int main (int argc, char* argv[])
{
    GarbageCollector gc(10000, 10000);
    nil_constant = make_object<Nil>(gc);
    std::cout << "> ";
    auto x = read(gc);
    std::cout << "==> " << x << std::endl; 
    // while (true) {
    //     Token tok = scan(std::cin);
    //     std::cout << "Read token: " << tok.repr() << std::endl;
    //     if (tok.type == T_EOF) break;
    // }
}

