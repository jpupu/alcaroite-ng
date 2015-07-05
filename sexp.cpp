#include <cctype>
#include <cstdio>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <stdexcept>
#include <utility>
#include <vector>
#include <cassert>

using std::shared_ptr;
using std::make_shared;

enum TokenType
{
	T_LPAREN,
	T_RPAREN,
	T_STRING,
	T_NUMBER,
	T_SYMBOL,
};
class Token
{
public:
	TokenType type;
	std::string s_val;
	double n_val;
	std::string lexeme;
	int line, column;

	Token (TokenType type, const std::string& lexeme)
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
			case T_SYMBOL:
				s_val = lexeme;
				break;
			default:
				s_val = lexeme;
		}
	}

	std::string repr () const
	{
		std::string types[] = {"lparen", "rparen", "string", "number", "symbol", "eot"};
		switch (type) {
			case T_STRING:
			case T_SYMBOL:
				return "<"+types[type]+":"+s_val+">"; break;
			case T_NUMBER:
				return "<"+types[type]+":"+std::to_string(n_val)+">"; break;
			default:
				return "<"+types[type]+">"; break;
		}
	}
};


enum AtomType
{
	ATOM_NIL,
	ATOM_NUMBER,
	ATOM_SYMBOL,
	ATOM_STRING
};


struct Sexp
{
	bool is_atom;

	// Atom data.
	AtomType type;
	double n_val;
	std::string s_val;

	// Cons cell data.
	shared_ptr<Sexp> car, cdr;

	Sexp () : is_atom(true), type(ATOM_NIL) {}
	Sexp (AtomType type, double n_val)
		: is_atom(true), type(type), n_val(n_val) {}
	Sexp (AtomType type, const std::string& s_val)
		: is_atom(true), type(type), s_val(s_val) {}
	Sexp (shared_ptr<Sexp> x, shared_ptr<Sexp> y) : is_atom(false), car(x), cdr(y) {}

	bool atom () const { return is_atom; }
	bool atom (AtomType t) const { return is_atom && type == t; }
	bool cons_cell () const { return !is_atom; }
	bool nil () const {	return is_atom && type == ATOM_NIL; }
	bool list () const { return !is_atom || type == ATOM_NIL; }
	bool proper_list () const { return nil() || (cons_cell() && cdr->proper_list()); }

	std::string repr () const
	{
		if (nil()) return "nil";
		else if (cons_cell()) return "(" + car->repr() + " : " + cdr->repr() + ")";
		else switch (type) {
			case ATOM_NIL:
				return "nil"; break;
			case ATOM_NUMBER:
				return std::to_string(n_val); break;
			case ATOM_SYMBOL:
				return s_val; break;
			case ATOM_STRING:
				return "\""+s_val+"\""; break;
			default:
				return "<error>"; break;
		}
	}


	friend std::ostream& operator<< (std::ostream&, const shared_ptr<Sexp>&);

	void print_dotted (std::ostream& stream) const
	{
		assert(cons_cell());

		stream << car;

		if (cdr->nil()) {
			stream << ')';
		}
		else if (cdr->atom()) {
			stream << " . " << cdr << ')';
		}
		else {
			stream << ' ';
			cdr->print_dotted(stream);
		}
	}

	void print (std::ostream& stream) const
	{
		if (cons_cell()) {
			stream << "(";
			print_dotted(stream);
		}
		else switch (type) {
			case ATOM_NIL:
				stream << "nil"; break;
			case ATOM_NUMBER:
				stream << std::to_string(n_val); break;
			case ATOM_SYMBOL:
				stream << s_val; break;
			case ATOM_STRING:
				stream << "\""+s_val+"\""; break;
			default:
				stream << "<error>"; break;
		}
		
	}
};

std::ostream& operator<< (std::ostream& stream, const shared_ptr<Sexp>& sexp)
{
	sexp->print(stream);
	return stream;
}

shared_ptr<Sexp> new_atom (AtomType type, double val)
{
	return make_shared<Sexp>(type, val);
}

shared_ptr<Sexp> new_atom (AtomType type, const std::string& val)
{
	return make_shared<Sexp>(type, val);
}

shared_ptr<Sexp> new_nil ()
{
	return make_shared<Sexp>(ATOM_NIL, 0.0);
}

shared_ptr<Sexp> new_cell (shared_ptr<Sexp> x, shared_ptr<Sexp> y)
{
	return make_shared<Sexp>(x, y);
}


shared_ptr<Sexp> cons (shared_ptr<Sexp> x, shared_ptr<Sexp> y)
{
	return new_cell(x, y);
}

shared_ptr<Sexp> car (shared_ptr<Sexp> x) { assert(x->cons_cell()); return x->car; }
shared_ptr<Sexp> cdr (shared_ptr<Sexp> x) { assert(x->cons_cell()); return x->cdr; }
shared_ptr<Sexp> caar (shared_ptr<Sexp> x) { assert(x->cons_cell()); return car(car(x)); }
shared_ptr<Sexp> cadr (shared_ptr<Sexp> x) { assert(x->cons_cell()); return car(cdr(x)); }
shared_ptr<Sexp> cdar (shared_ptr<Sexp> x) { assert(x->cons_cell()); return cdr(car(x)); }
shared_ptr<Sexp> cddr (shared_ptr<Sexp> x) { assert(x->cons_cell()); return cdr(cdr(x)); }
shared_ptr<Sexp> caaar (shared_ptr<Sexp> x) { assert(x->cons_cell()); return car(car(car(x))); }
shared_ptr<Sexp> caadr (shared_ptr<Sexp> x) { assert(x->cons_cell()); return car(car(cdr(x))); }
shared_ptr<Sexp> cadar (shared_ptr<Sexp> x) { assert(x->cons_cell()); return car(cdr(car(x))); }
shared_ptr<Sexp> caddr (shared_ptr<Sexp> x) { assert(x->cons_cell()); return car(cdr(cdr(x))); }
shared_ptr<Sexp> cdaar (shared_ptr<Sexp> x) { assert(x->cons_cell()); return cdr(car(car(x))); }
shared_ptr<Sexp> cdadr (shared_ptr<Sexp> x) { assert(x->cons_cell()); return cdr(car(cdr(x))); }
shared_ptr<Sexp> cddar (shared_ptr<Sexp> x) { assert(x->cons_cell()); return cdr(cdr(car(x))); }
shared_ptr<Sexp> cdddr (shared_ptr<Sexp> x) { assert(x->cons_cell()); return cdr(cdr(cdr(x))); }

int list_size (shared_ptr<Sexp> xs)
{
	assert(xs->list());
	if (xs->nil()) return 0;
	if (xs->cdr->atom() && !xs->cdr->nil()) return 2;
	return 1 + list_size(xs->cdr);
}

shared_ptr<Sexp> reverse_step (const shared_ptr<Sexp>& xs, shared_ptr<Sexp> tail)
{
	assert(tail->proper_list());
	assert(xs->proper_list());
	if (xs->nil()) return tail;
	return reverse_step(xs->cdr, cons(xs->car, tail));
}

shared_ptr<Sexp> reverse (const shared_ptr<Sexp>& xs)
{
	assert(xs->proper_list());
	return reverse_step(xs, new_nil());
}



enum LexerState
{
	S_START,
	S_INTEGER,
	S_DECIMAL,
	S_STRING,
	S_SYMBOL
};

std::vector<Token> foo (std::istream& is)
{
	LexerState state = S_START;
	std::string lexeme;
	std::vector<Token> tokens;
	bool stop = false;
	while (!stop) {
		int c = is.get();
		printf("%d: %d %c\n", state, c, c);
		bool pop = true;
		switch (state) {
			case S_START:
				if (isdigit(c)) {
					lexeme += c;
					state = S_INTEGER;
				}
				else if (c == '_' || isalpha(c)) {
					lexeme += c;
					state = S_SYMBOL;
				}
				else if (c == '(') {
					tokens.push_back(Token(T_LPAREN, "("));
					lexeme = "";
					state = S_START;
				}
				else if (c == ')') {
					tokens.push_back(Token(T_RPAREN, ")"));
					lexeme = "";
					state = S_START;
				}
				else if (c == '"') {
					lexeme += c;
					state = S_STRING;
				}
				else if (isspace(c)) {
					// nop
				}
				else if (c == -1) { 
					stop = true;
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
				else if (c == '(' || c == ')' || isspace(c) || c == -1) {
					tokens.push_back(Token(T_NUMBER, lexeme));
					lexeme = "";
					state = S_START;
					pop = false;
				}
				else {
					throw std::runtime_error("Bad integer");
				}
				break;

			case S_DECIMAL:
				if (isdigit(c)) {
					lexeme += c;
				}
				else if (c == '(' || c == ')' || isspace(c) || c == -1) {
					tokens.push_back(Token(T_NUMBER, lexeme));
					lexeme = "";
					state = S_START;
					pop = false;
				}
				else {
					throw std::runtime_error("Bad decimal number");
				}
				break;

			case S_STRING:
				lexeme += c;
				if (c == '"') {
					tokens.push_back(Token(T_STRING, lexeme));
					lexeme = "";
					state = S_START;
				}
				else if (c == -1) {
					throw std::runtime_error("End of text while reading quoted string");
				}
				break;

			case S_SYMBOL:
				if (isalnum(c)) {
					lexeme += c;
				}
				else if (c == '(' || c == ')' || isspace(c) || c == -1) {
					tokens.push_back(Token(T_SYMBOL, lexeme));
					lexeme = "";
					state = S_START;
					pop = false;
				}
				else {
					throw std::runtime_error("Bad symbol");
				}
				break;
		}

		if (!pop) {
			is.unget();
		}
	}

	return tokens;
}


shared_ptr<Sexp> extract_sexp (std::vector<Token>::iterator& iter, const std::vector<Token>::iterator& end)
{
	switch (iter->type) {
		case T_SYMBOL:
			return new_atom(ATOM_SYMBOL, iter->s_val);
		case T_STRING:
			return new_atom(ATOM_STRING, iter->s_val);
		case T_NUMBER:
			return new_atom(ATOM_NUMBER, iter->n_val);
		case T_LPAREN: {
			auto list = new_nil();
			while (true) {
				++iter;
				if (iter == end) {
					throw std::runtime_error("End of text while reading list");
				}
				if (iter->type == T_RPAREN) {
					break;
				}
				list = cons(extract_sexp(iter,end), list);
			}
			return reverse(list);
		}
		case T_RPAREN:
			throw std::runtime_error("Closing paren with no open paren");
	}
}



class Evaluator 
{
	std::map<std::string, shared_ptr<Sexp>> variables;

	shared_ptr<Sexp> lookup_variable (const std::string& name)
	{
		auto it = variables.find(name);
		if (it == variables.end()) {
			throw std::runtime_error(std::string("no such symbol: ") + name);
		}
		return it->second;
	}

public:
	shared_ptr<Sexp> eval_program (shared_ptr<Sexp> sexp)
	{
		auto result = new_nil();
		auto cursor = sexp;

		while (!cursor->nil()) {
			if (cursor->atom()) {
				result = cons(eval(cursor), result);
				break;
			}
			result = cons(eval(cursor->car), result);
			cursor = cursor->cdr; 
		}
		return reverse(result);
	}

	shared_ptr<Sexp> eval (shared_ptr<Sexp> sexp)
	{
		if (sexp->atom()) {
			if (sexp->type == ATOM_SYMBOL) {
				return lookup_variable(sexp->s_val);
			}
			else {
				return sexp;
			}
		}

		assert(sexp->proper_list());

		if (sexp->car->atom(ATOM_SYMBOL)) {
			auto syntax = sexp->car->s_val;

			if (syntax == "define") {
				if (!cadr(sexp)->atom(ATOM_SYMBOL)) {
					throw std::runtime_error("attempting to define non-symbol");
				}
				if (!cdddr(sexp)->nil()) {
					throw std::runtime_error("define takes single value");
				}
				auto sym = cadr(sexp)->s_val;
				if (variables.find(sym) != variables.end()) {
					throw std::runtime_error("cannot re-define symbol");
				}
				variables[sym] = eval(caddr(sexp));
				return new_nil();
			}

			if (syntax == "quote") {
				if (cdr(sexp)->nil() || !cddr(sexp)->nil()) {
					throw std::runtime_error("bad quote form");
				}
				return cadr(sexp);
			}

			if (syntax == "if") {
				auto args = cdr(sexp);
				if (list_size(args) != 3) {
					throw std::runtime_error("if-form takes 3 arguments");
				}
				auto test = eval(car(args));
				if (test->nil() || (test->atom(ATOM_NUMBER) && test->n_val == 0)) {
					// Test evaluates to FALSE
					return eval(caddr(args));
				}
				else {
					// Test evaluates to TRUE.
					return  eval(cadr(args));
				}
			}
		}

		// Function call
		if (!sexp->car->atom(ATOM_SYMBOL)) {
			throw std::runtime_error("Function name must be a symbol!");
		}
		auto name = car(sexp)->s_val;
		auto args = eval_program(cdr(sexp));

		if (name == "sum") {
			double sum = 0.0;
			auto cursor = args;
			while (!cursor->nil()) {
				if (!cursor->cons_cell() || !cursor->car->atom(ATOM_NUMBER)) {
					throw std::runtime_error("sum argument list must be a proper list of numbers.");
				}
				sum += cursor->car->n_val;
				cursor = cursor->cdr;
			}
			return new_atom(ATOM_NUMBER, sum);

		}

		throw std::runtime_error(std::string("undefined function: ") + sexp->car->repr());
	}
};



int main () {
	std::vector<Token> tokens = foo(std::cin);

	for (auto t : tokens) {
		printf("%s\n", t.repr().c_str());
	}

	auto it = tokens.begin();
	auto root = extract_sexp(it, tokens.end());
	std::cout << root << std::endl << std::endl;
	auto evaluator = Evaluator();
	root = evaluator.eval_program(root);
	std::cout << root << std::endl;
}

