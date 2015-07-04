#include <cctype>
#include <cstdio>
#include <iostream>
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
	bool proper_list_nr () const { return !is_atom || type == ATOM_NIL; }
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
};

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

shared_ptr<Sexp> reverse_step (const shared_ptr<Sexp>& xs, shared_ptr<Sexp> tail)
{
	assert(tail->proper_list_nr());
	assert(xs->proper_list_nr());
	if (xs->nil()) return tail;
	return reverse_step(xs->cdr, cons(xs->car, tail));
}

shared_ptr<Sexp> reverse (const shared_ptr<Sexp>& xs)
{
	assert(xs->proper_list_nr());
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


shared_ptr<Sexp> eval_sexp (shared_ptr<Sexp> sexp)
{
	if (sexp->atom()) return sexp;

	auto newsexp = cons(eval_sexp(sexp->car), eval_sexp(sexp->cdr));

	auto car = eval_sexp(sexp->car);
	auto cdr = eval_sexp(sexp->cdr);

	if (!car->atom() || car->type != ATOM_SYMBOL) return cons(car, cdr);

	if (car->s_val == "pi") return new_atom(ATOM_NUMBER, 3.1415916);


	if (car->s_val == "sum") {
		double sum = 0.0;
		auto cursor = cdr;
		while (!cursor->nil()) {
			if (!cursor->cons_cell() || !cursor->car->atom(ATOM_NUMBER)) {
				throw std::runtime_error("sum argument list must be a proper list of numbers.");
			}
			sum += cursor->car->n_val;
			cursor = cursor->cdr;
		}
		return new_atom(ATOM_NUMBER, sum);
	}

	return cons(car, cdr);
}


int main () {
	std::vector<Token> tokens = foo(std::cin);

	for (auto t : tokens) {
		printf("%s\n", t.repr().c_str());
	}

	auto it = tokens.begin();
	auto root = extract_sexp(it, tokens.end());
	printf("%s\n", root->repr().c_str());
	printf("\n");
	root = eval_sexp(root);
	printf("%s\n", root->repr().c_str());
	printf("\n");
}

