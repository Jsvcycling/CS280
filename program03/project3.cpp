#include "p2lex.h"
#include <string>
#include <istream>
#include <fstream>
using namespace std;

class ParseTree {
private:
	ParseTree *leftChild;
	ParseTree *rightChild;

	int	whichLine;

public:
	ParseTree(ParseTree *left = 0, ParseTree *right = 0) : leftChild(left), rightChild(right) {
		whichLine = linenum;
	}

	int onWhichLine() { return whichLine; }
};

// An identifier
class Id : public ParseTree {
private:
	Token tok;

public:
	Id(const Token &t) : ParseTree(), tok(t) { }

	string getId() { return t.getLexeme(); }
};

// An integer
class Integer : public ParseTree {
private:
	Token tok;

public:
	Integer(const Token &t) : ParseTree(), tok(t) { }

	int getInteger() { return stoi(t.getLexeme()); }
};

// A string
class Str : public ParseTree {
private:
	Token tok;

public:
	Str(const Token &t, ParseTree *left = 0, ParseTree *right = 0) : ParseTree(left, right), tok(t) { }

	string getString() { return t.getLexeme() };
};

int linenum = 0;
int globalErrorCount = 0;

/// error handler
void error(string msg) {
	cout << linenum << ": " << msg << endl;
	++globalErrorCount;
}

/// function prototypes
ParseTree *Program(istream *in);
ParseTree *StmtList(istream *in);
ParseTree *Stmt(istream *in);
ParseTree *Expr(istream *in);
ParseTree *Term(istream *in);
ParseTree *Primary(istream *in);
ParseTree *String(istream *in);

/* Program -> <StmtList> */
ParseTree *Program(istream *in) {
	ParseTree *result = StmtList(in);

	// make sure there are no more tokens...
	if (getToken(in).getTok() != DONE) {
		return 0;
	}

	return result;
}

/* StmtList -> <Stmt> | <Stmt> <StmtList> */
ParseTree *StmtList(istream *in) {
	ParseTree *left = Stmt(in);

	// TODO: Somehow we have to do right as well...
	ParseTree *right = 0;

	return new ParseTree(left, right);
}

/* Stmt -> PRINT <Expr> SC | SET ID <Expr> SC */
ParseTree *Stmt(istream *in) {
	Token t;

	t = getToken(in);

	if (t.getTok() == PRINT) {
		ParseTree *left = Expr(in);

		if (ex == 0) {
			return 0;
		}

		t = getToken(in);

		if (t.getTok() != SC) {
			error("expected semicolon");
			return 0;
		}

		return new ParseTree(left);
	} else if (t.getTok() == SET) {
		t = getToken(in);

		if (t.getTok() != ID) {
			error("expected id");
			return 0;
		}

		ParseTree *left = Id(t);
		ParseTree *right = Expr(in);

		if (ex == 0) {
			return 0;
		}

		t = getToken(in);

		if (t.getTok() != SC) {
			error("expected semicolon");
			return 0;
		}

		return new ParseTree(left, right);
	} else {
		error("Invalid statement");
	}

	return 0;
}

/* Expr -> <Expr> PLUS <Term> | <Term> */
ParseTree *Expr(istream *in) {
	return 0;
}

/* Term -> <Term> STAR <Primary> | <Primary> */
ParseTree *Term(istream *in) {
	return 0;
}

/* Primary -> ID | <String> | INT | LPAREN <Expr> RPAREN */
ParseTree *Primary(istream *in) {
	Token t = getToken(in);

	if (t.getTok() == ID) {
		return new Id(t);
	} else if (t.getTok() == INT) {
		return new Integer(t);
	} else if (t.getTok() == STR) {
		// We can't do this though since the token is already read...
		return String(in);
	} else if (t.getTok() == LPAREN) {
		t = getTok(in);

		if (t.getTok() != ID) {
			error("expected id");
			return 0;
		}
		
		ParseTree *ex = Expr(in);
		
		if (ex == 0) {
			return 0;
		}
		
		t = getToken(in);
		
		if (t.getTok() != RPAREN) {
			error("expected right parens");
			return 0;
		}

		return ex;
	}

	return 0;
}

/* String -> STR | STR LEFTSQ <Expr> RIGHTSQ | STR LEFTSQ <Expr> SC <Expr> RIGHTSQ */
ParseTree *String(istream *in) {
	Token t = getToken(in);

	if (t.getTok() == STR) {
		// TODO: we have to peek forward and check for a left square bracket...
	} else {
		error("expected string");
		return 0;
	}
}

int main(int argc, char **argv) {
	istream *in = &cin;
	fstream infile;

	for (int i = 1; i < argc; i++) {
		if (in != &cin) {
			// TODO
		}

		infile.open(argv[i]);

		if (!infile.is_open()) {
			// TODO
		}

		in = &infile;
	}
	
	ParseTree *prog = Program(in);

	if (prog == 0 || globalErrorCount != 0) {
		cout << "Parse failed, exiting" << endl;
		return 0;
	}

	cout << "Success. Congrats!" << endl;
	return 0;
}
