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
	ATOM_NUMBER,
	ATOM_SYMBOL,
	ATOM_STRING
};

struct Atom
{
	AtomType type;
	double n_val;
	std::string s_val;

	Atom (AtomType type, double val)
		: type(type), n_val(val)
	{
		if (type != ATOM_NUMBER) throw std::runtime_error("not numerical");
	}

	Atom (AtomType type, const std::string& val)
		: type(type), s_val(val)
	{
		if (type != ATOM_STRING && type != ATOM_SYMBOL) throw std::runtime_error("not stringal");
	}

	std::string repr () const
	{
		switch (type) {
			case ATOM_STRING:
				return "\""+s_val+"\""; break;
			case ATOM_SYMBOL:
				return s_val; break;
			case ATOM_NUMBER:
				return std::to_string(n_val); break;
			default:
				return "<error>"; break;
		}
	}
};


struct Sexp;

struct ConsCell
{
	shared_ptr<Sexp> a, b;

	ConsCell () {}
	ConsCell(shared_ptr<Sexp>& a, shared_ptr<Sexp>& b) : a(a), b(b) {}
};

struct Sexp
{
	bool is_atom;
	shared_ptr<Atom> atom;
	ConsCell cell;

	Sexp () : is_atom(true) {}
	Sexp (shared_ptr<Atom> x) : is_atom(true), atom(x) {}
	Sexp (shared_ptr<Sexp> x, shared_ptr<Sexp> y) : is_atom(false), cell(x,y) {}

	bool nil () const
	{
		return is_atom && !atom;
	}

};

shared_ptr<Sexp> cons (shared_ptr<Sexp> x, shared_ptr<Sexp> ys)
{
	assert(ys->nil() || !ys->is_atom);
	return make_shared<Sexp>(x, ys);
}

shared_ptr<Sexp> reverse_step (const shared_ptr<Sexp>& xs, shared_ptr<Sexp> tail)
{
	assert(xs->nil() || !xs->is_atom);
	if (xs->nil()) return tail;
	return reverse_step(xs->cell.b, cons(xs->cell.a, tail));
}

shared_ptr<Sexp> reverse (const shared_ptr<Sexp>& xs)
{
	return reverse_step(xs, make_shared<Sexp>());
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

// template<typename T>
// std::unique_ptr<Sexp> extract_sexp (T& iter, const T& end)
shared_ptr<Sexp> extract_sexp (std::vector<Token>::iterator& iter, const std::vector<Token>::iterator& end)
{
	switch (iter->type) {
		case T_SYMBOL:
			return make_shared<Sexp>(make_shared<Atom>(ATOM_SYMBOL, iter->s_val));
		case T_STRING:
			return make_shared<Sexp>(make_shared<Atom>(ATOM_STRING, iter->s_val));
		case T_NUMBER:
			return make_shared<Sexp>(make_shared<Atom>(ATOM_NUMBER, iter->n_val));
		case T_LPAREN: {
			auto list = make_shared<Sexp>();
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

void print_sexp (const Sexp* sexp)
{
	if (sexp->nil()) {
		printf("()");
	}
	else if (sexp->is_atom) {
		printf("%s", sexp->atom->repr().c_str());
	}
	else {
		printf("(");
		print_sexp(sexp->cell.a.get());
		printf(" : ");
		print_sexp(sexp->cell.b.get());
		printf(")");
	}
}

shared_ptr<Sexp> eval_sexp (shared_ptr<Sexp> sexp)
{
	if (sexp->is_atom) return sexp;

	shared_ptr<Sexp> newsexp = cons(eval_sexp(sexp->cell.a), eval_sexp(sexp->cell.b));

	if (!newsexp->cell.a->is_atom) return newsexp;

	if (newsexp->cell.a->atom->type != ATOM_SYMBOL) return newsexp;

	if (newsexp->cell.a->atom->s_val == "pi") return make_shared<Sexp>(make_shared<Atom>(ATOM_NUMBER, 3.141592));

	if (newsexp->cell.a->atom->s_val == "sum") {
		double sum = 0.0;
		auto cursor = newsexp->cell.b;
		while (!cursor->nil()) {
			sum += cursor->cell.a->atom->n_val;
			cursor = cursor->cell.b;
		}
		return make_shared<Sexp>(make_shared<Atom>(ATOM_NUMBER, sum));
	}

	return newsexp;

}


int main () {
	std::vector<Token> tokens = foo(std::cin);

	for (auto t : tokens) {
		printf("%s\n", t.repr().c_str());
	}

	auto it = tokens.begin();
	auto root = extract_sexp(it, tokens.end());
	print_sexp(root.get());
	printf("\n");
	root = eval_sexp(root);
	print_sexp(root.get());
	printf("\n");
}

