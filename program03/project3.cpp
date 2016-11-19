#include "p2lex.h"

#include <string>
#include <istream>
#include <fstream>

using namespace std;

// ParseTree object base class
class ParseTree {
private:
	ParseTree *leftChild;
	ParseTree *rightChild;

	int	whichLine;

public:
	ParseTree(ParseTree *left = 0, ParseTree *right = 0) : leftChild(left), rightChild(right) {
		whichLine = linenum;
	}

	ParseTree *left() { return leftChild; }
	ParseTree *right() { return rightChild; }

	int onWhichLine() { return whichLine; }
};

// PLUS operator
class PlusOp : public ParseTree {
private:
	Token tok;
	
public:
	PlusOp(const Token &t, ParseTree *left = 0, ParseTree *right = 0) : ParseTree(left, right), tok(t) { }
};

// STAR operator
class StarOp : public ParseTree {
private:
	Token tok;
	
public:
	StarOp(const Token &t, ParseTree *left = 0, ParseTree *right = 0) : ParseTree(left, right), tok(t) { }
};

// Print statement
class PrintStmt : public ParseTree {
public:
	PrintStmt(ParseTree *expr) : ParseTree(expr) { }
};

// Set statement
class SetStmt : public ParseTree {
private:
	Token tok;
	
public:
	PrintStmt(const Token &t, ParseTree *expr) : ParseTree(expr), tok(t) { }
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

Token savedToken;
bool hasSavedToken = false;

Token doGetToken(istream *in) {
	if (hasSavedToken) {
		hasSavedToken = false;
		return savedToken;
	}

	return getToken(in);
}

void saveToken(Token &t) {
	if (hasSavedToken) {
		error("a token was already save");
		return;
	}

	savedToken = t;
	hasSavedToken = true;
}

/* <Program> := <StmtList> */
ParseTree *Program(istream *in) {
	ParseTree *result = StmtList(in);

	// make sure there are no more tokens...
	if (getToken(in).getTok() != DONE) {
		return 0;
	}

	return result;
}

/* <StmtList> := <Stmt> {<Stmt>} */
ParseTree *StmtList(istream *in) {
	ParseTree *left = Stmt(in);
	ParseTree *right = StmtList(in);

	return new ParseTree(left, right);
}

/* <Stmt> := PRINT <Expr> SC | SET ID <Expr> SC */
ParseTree *Stmt(istream *in) {
	Token t;

	t = doGetToken(in);

	if (t.getTok() == PRINT) {
		ParseTree *left = Expr(in);

		if (ex == 0) {
			return 0;
		}

		t = doGetToken(in);

		if (t.getTok() != SC) {
			error("expected semicolon");
			return 0;
		}

		return new PrintStmt(left);
	} else if (t.getTok() == SET) {
		t = doGetToken(in);

		if (t.getTok() != ID) {
			error("expected id");
			return 0;
		}

		ParseTree *left = Id(t);
		ParseTree *right = Expr(in);

		if (ex == 0) {
			return 0;
		}

		t = doGetToken(in);

		if (t.getTok() != SC) {
			error("expected semicolon");
			return 0;
		}

		return new SetStmt(left, right);
	}
	
	error("Invalid statement");
	return 0;
}

/* <Expr> := <Term> {+ <Term>} */
ParseTree *Expr(istream *in) {
	ParseTree *left = Term(in);

	Token t = doGetToken(in);

	if (t.getTok() == PLUS) {
		ParseTree *right = Expr(in);

		if (right == 0) {
			return 0;
		}

		return new PlusOp(t, left, right);
	}

	saveToken(t);
	return left;
}

/* <Term> := <Primary> {* <Primary>} */
ParseTree *Term(istream *in) {
	ParseTree *left = Primary(in);

	Token t = doGetToken(in);

	if (t.getTok() == STAR) {
		ParseTree *right = Term(in);

		if (right == 0) {
			return 0;
		}

		return new StarOp(t, left, right);
	}

	saveToken(t);
	return left;
}

/* <Primary> := ID | <String> | INT | LPAREN <Expr> RPAREN */
ParseTree *Primary(istream *in) {
	Token t = doGetToken(in);

	if (t.getTok() == ID) {
		return new Id(t);
	} else if (t.getTok() == INT) {
		return new Integer(t);
	} else if (t.getTok() == STR) {
		saveToken(t);
		return String(in);
	} else if (t.getTok() == LPAREN) {
		t = doGetToken(in);

		if (t.getTok() != ID) {
			error("expected id");
			return 0;
		}
		
		ParseTree *left = Expr(in);
		
		if (ex == 0) {
			return 0;
		}
		
		t = doGetToken(in);
		
		if (t.getTok() != RPAREN) {
			error("expected right parens");
			return 0;
		}

		return left;
	}

	return 0;
}

/* <String> := STR | STR LEFTSQ <Expr> RIGHTSQ | STR LEFTSQ <Expr> SC <Expr> RIGHTSQ */
ParseTree *String(istream *in) {
	Token t = doGetToken(in);

	if (t.getTok() == STR) {
		Token t1 = getToken(in);

		if (t1.getTok() == LEFTSQ) {
			ParseTree *left = Expr(in);

			t1 = getToken(in);

			if (t1.getTok() == RIGHTSQ) {
				return Str(t, left);
			} else if (t1.getTok() == SC) {
				ParseTree *right = Expr(in);

				t1 = getToken(in);

				if (t1.getTok() == RIGHTSQ) {
					return new Str(t, left, right);
				}
			}

			error("expected right square bracket");
			return 0;
		}

		return Str(t);
	}
	
	error("expected string");
	return 0;
}

int plusOpCount = 0;
int starOpCount = 0;
int bracketOpCount = 0;

// For accumulating operator counts
void traverseTree1(ParseTree *t) {
	if (!t) return;
	traverseTree1(t->left());
	traverseTree1(t->right());

	// TODO: get node information
}

// For error checking
void traverseTree2(ParseTree *t) {
	if (!t) return;
	traverseTree2(t->left());
	traverseTree2(t->right());

	// TODO: check for errors
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

	traverseTree1(prog);

	cout << "Count of + operators: " << plusOpCount << endl;
	cout << "Count of * operators: " << starOpCount << endl;
	cout << "Count of [] operators: " << bracketOpCount << endl;
	
	traverseTree2(prog);
	
	cout << "Success. Congrats!" << endl;
	return 0;
}
