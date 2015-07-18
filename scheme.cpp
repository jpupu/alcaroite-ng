#include "lexer.hpp"

#include <algorithm>
#include <exception>
#include <map>
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


namespace scheme_ns {

constexpr size_t OBJECT_ALIGNMENT = 8;


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
    bool gc_mark;

    std::string type_name () const
    {
        switch (type) {
            case pair_type: return "pair";
            case symbol_type: return "symbol";
            case number_type: return "number";
            case string_type: return "string";
            case nil_type: return "nil";
        }
    }

protected:
    Object (TypeTag type) : type(type), gc_mark(false) {}
};

// template<typename T>
// struct is_object : std::conditional< std::is_base_of<>


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



size_t object_sizeof (const Object* obj)
{
    switch (obj->type) {
        case Object::pair_type: return sizeof(Pair);
        case Object::symbol_type:
            return sizeof(Symbol) + static_cast<const Symbol*>(obj)->len + 1;
        case Object::string_type:
            return sizeof(String) + static_cast<const String*>(obj)->len + 1;
        case Object::number_type: return sizeof(Number);
        case Object::nil_type: return sizeof(Nil);
    }
}

template<typename T>
T* object_align (const T* ptr)
{
    auto p = reinterpret_cast<size_t>(ptr);
    return reinterpret_cast<T*>(p + OBJECT_ALIGNMENT - 1 - (p - 1) % OBJECT_ALIGNMENT); 
}

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

    std::vector<Object**> root_pointers;

private:
    void mark_object_recursive (Object* obj)
    {
        // std::cout << "GC: mark " << obj << std::endl;
        if (obj->gc_mark) {
            // std::cout << "GC: loop detected\n";
            return;
        }
        obj->gc_mark = true;

        if (obj->type == Object::pair_type) {
            auto p = static_cast<Pair*>(obj);
            mark_object_recursive(p->car);
            mark_object_recursive(p->cdr);
        }
    }

    void update_object_recursive (Object* obj, const std::map<Object*, Object*>& moves)
    {
        // std::cout << "GC: update " << obj << std::endl;
        if (!obj->gc_mark) {
            // std::cout << "GC: loop detected\n";
            return;
        }
        obj->gc_mark = false;

        if (obj->type == Object::pair_type) {
            auto p = static_cast<Pair*>(obj);

            auto it = moves.find(p->car);
            if (it != moves.end()) { p->car = it->second; }
            it = moves.find(p->cdr);
            if (it != moves.end()) { p->cdr = it->second; }

            update_object_recursive(p->car, moves);
            update_object_recursive(p->cdr, moves);
        }
    }


public:
    /// Sizes are given in bytes.
    GarbageCollector (size_t generic_pool_size)
    {
        generic_pool.reserve(generic_pool_size);
    }

    template<class T>
    void add_root_pointer (T** pointer)
    {
        static_assert(std::is_base_of<Object,T>::value, "T must be-an object");
        auto p = reinterpret_cast<Object**>(pointer);
        // std::cout << "add " << p << "->" << *p << std::endl;
        root_pointers.push_back(p);
    }

    template<class T>
    void remove_root_pointer (T** pointer)
    {
        static_assert(std::is_base_of<Object,T>::value, "T must be-an object");
        auto p = reinterpret_cast<Object**>(pointer);
        root_pointers.erase(std::remove(root_pointers.begin(),
                                        root_pointers.end(),
                                        p));
    }


    void collect ()
    {
        // std::cout << "GC: collection started\n";

        // Mark.
        // for(int i = 0; i < root_pointers.size(); i++)
        // {
        //     std::cout << "***" << i << "  " << root_pointers[i] << " -> " << (*root_pointers[i]) << '\n';
        // }
        for (auto it : root_pointers) {
            mark_object_recursive(*it);
        }

        // Sweep.
        char* point = generic_pool.data();
        char* limit = point + generic_pool.size();
        char* new_limit = point;
        std::map<Object*, Object*> moves;
        while (point < limit) {
            auto o = reinterpret_cast<Object*>(point);
            size_t size = object_sizeof(o);

            // std::cout << "GC: check " << o << " type: " << o->type << std::endl;

            if (!o->gc_mark) {
                // std::cout << "GC:   remove " << o << std::endl;
            }
            else {
                // move if needed.
                if (new_limit != point) {
                    // std::cout << "GC:   move -> " << new_limit << std::endl;
                    memmove(new_limit, point, size);
                    moves[o] = reinterpret_cast<Object*>(new_limit);
                }
                new_limit = object_align(new_limit + size);
            }
            point = object_align(point + size);
        }
        generic_pool.resize(new_limit - generic_pool.data());
        // std::cout << "GC: new size " << generic_pool.size() << std::endl;

        // Update pointers.
        if (!moves.empty()) {
            for (auto it : root_pointers) {
                auto move_it = moves.find(*it);
                if (move_it != moves.end()) { *it = move_it->second; }
                update_object_recursive(*it, moves);
            }
        }

        // std::cout << "GC: collection ended\n";
    }

    void* allocate (size_t alignment, size_t size)
    {
        // std::cout << "GC: allocating " << size << " bytes (" << alignment << " byte alignemnt)\n"; 

        size_t point = generic_pool.size();
        // Move point so it's properly aligned.
        point += alignment - 1 - (point - 1) % alignment;
        
        // If capacity is still insufficient, we're SOL.
        if (point + size >= generic_pool.capacity()) {
            throw std::runtime_error("GC: out of memory");
        }

        // // Collect garbage if not enough capacity.
        // if (point + size >= generic_pool.capacity()) {
        //     collect();
    
        //     point = generic_pool.size();
        //     // Move point so it's properly aligned.
        //     point += alignment - 1 - (point - 1) % alignment;
    
        //     // If capacity is still insufficient, we're SOL.
        //     if (point + size >= generic_pool.capacity()) {
        //         throw std::runtime_error("GC: out of memory");
        //     }
        // }

        // std::cout << "GC: allocated at point " << point << " @ " << static_cast<void*>(generic_pool.data() + point) << '\n';

        // We have capacity, so allocate the memory.
        generic_pool.resize(point + size);
        return static_cast<void*>(generic_pool.data() + point);
    }
};


namespace consts {
    template<typename T>
    struct automatic_ptr {
        std::unique_ptr<T> ptr;
        automatic_ptr () : ptr(new T()) {
            static_assert(std::is_base_of<Object,T>::value, "T must be-an object");
        }
        operator T* () { return ptr.get(); }
    };

   automatic_ptr<Nil> nil;
};

GarbageCollector gc(1000);


template<class T, class... Args>
T* make_object (Args&&... args)
{
    static_assert(std::is_base_of<Object,T>::value, "T must be-an object");

    size_t size = T::total_size(std::forward<Args>(args)...);
    // void* ptr = gc.allocate(alignof(T), size);
    void* ptr = gc.allocate(OBJECT_ALIGNMENT, size);
    return new(ptr) T(std::forward<Args>(args)...);
}

Object* car (Object* o)
{
    if (o->type != Object::pair_type) {
        throw std::runtime_error("expecting pair, got " + o->type_name());
    }
    return static_cast<Pair*>(o)->car;
}

Object* cdr (Object* o)
{
    if (o->type != Object::pair_type) {
        throw std::runtime_error("expecting pair, got " + o->type_name());
    }
    return static_cast<Pair*>(o)->cdr;
}

Object* caar (Object* o) { return car(car(o)); }
Object* cadr (Object* o) { return car(cdr(o)); }
Object* cdar (Object* o) { return cdr(car(o)); }
Object* cddr (Object* o) { return cdr(cdr(o)); }

Object* read_list ();

Object* read (const Token& tok)
{
    switch (tok.type) {
        case T_STRING: return make_object<String>(tok.s_val.size(), tok.s_val.data());
        case T_IDENTIFIER: return make_object<Symbol>(tok.s_val.size(), tok.s_val.data());
        case T_NUMBER: return make_object<Number>(tok.n_val);
        case T_LPAREN: return read_list();
        case T_RPAREN:
            throw std::runtime_error("unexpected closing paren");
        case T_EOF: return consts::nil; // FIXME
    }
}

Object* read_list ()
{
    Token tok = scan(std::cin);
    if (tok.type == T_RPAREN) {
        return consts::nil;
    }
    else if (tok.type == T_EOF) {
        throw;
    }
    return make_object<Pair>(read(tok), read_list());
}

/// Reads syntax datum and returns a datum.
Object* read ()
{
    return read(scan(std::cin));
}


class evaluation_error : public std::runtime_error
{
public:
    evaluation_error (const std::string& what)
        : std::runtime_error(what)
        { }

    std::vector<Object*> expressions;
};

class Scheme
{
    std::map<std::string, Object*> variable_bindings;

public:
    Scheme ()
    {
        variable_bindings["pi"] = make_object<Number>(3.141592);
        gc.add_root_pointer(&variable_bindings["pi"]);
    }

    Object* special_form_define (Object* args)
    {
        if (args->type != Object::pair_type) {
            throw evaluation_error("invalid `define' form (1st arg)");
        }

        if (cdr(args)->type != Object::pair_type) {
            throw evaluation_error("invalid `define' (2nd arg)");
        }

        if (cddr(args) != consts::nil) {
            throw evaluation_error("invalid `define' (more than 2 arguments)");
        }

        if (car(args)->type != Object::symbol_type) {
            throw evaluation_error("invalid `define' (variable is not a symbol)");        
        }
        auto name = std::string(static_cast<Symbol*>(car(args))->data);

        if (variable_bindings.find(name) != variable_bindings.end()) {
            throw evaluation_error("variable `" + name + "' already defined");
        }

        auto value = evaluate(cadr(args));
        variable_bindings[name] = value;
        gc.add_root_pointer(&variable_bindings[name]);
        return make_object<String>(3, "non");
    }

    Object* evaluate (Object* form)
    {
        try {
        if (form->type == Object::symbol_type) {
            auto name = std::string(static_cast<String*>(form)->data);
            auto it = variable_bindings.find(name);
            if (it == variable_bindings.end()) {
                throw evaluation_error("variable not bound");
            }
            return it->second;
        }
        if (form->type != Object::pair_type) {
            return form;
        }

        auto p = static_cast<Pair*>(form);

        // Check for syntax.
        if (p->car->type == Object::symbol_type) {
            auto name = std::string(static_cast<String*>(p->car)->data);
            if (name == "define") { return special_form_define(p->cdr); }
        }

        throw evaluation_error("I'm confused...");
        }
        catch (evaluation_error& e) {
            e.expressions.push_back(form);
            throw;
        }
    }
};

void print (std::ostream& stream, const Object* obj);

/// (internal) Used in printing a list/pair to stream.
void print_tail (std::ostream& stream, const Pair* obj, bool initial = false)
{
    if (initial) { stream << '('; }
    print(stream, obj->car);
    if (obj->cdr == consts::nil) {
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

}; // namespace scheme_ns

int main (int argc, char* argv[])
{
    using namespace scheme_ns;

    Scheme sc;

    while (true) {
        std::cout << "> ";
        auto expr = read();
        try {
            auto x = sc.evaluate(expr);
            std::cout << "==> " << x << std::endl;
            if (x == consts::nil) break;
        }
        catch (const evaluation_error& e) {
            std::cerr << "Error\n  " << e.what() << std::endl;
            std::cerr << "while evaluating expression\n";
            std::cerr << "     " << e.expressions.front() << std::endl;
            for (auto it = e.expressions.begin()+1; it != e.expressions.end(); ++it) {
                std::cerr << "  in " << (*it) << std::endl;
            }
            break;
        }
        catch (const std::runtime_error& e) {
            std::cerr << "Error\n  " << e.what() << std::endl;
            std::cerr << "while evaluating expression " << expr << std::endl;
            break;
        }
        gc.collect();
    } 

    // while (true) {
    //     Token tok = scan(std::cin);
    //     std::cout << "Read token: " << tok.repr() << std::endl;
    //     if (tok.type == T_EOF) break;
    // }
}

