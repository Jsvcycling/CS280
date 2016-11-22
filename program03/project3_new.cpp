#include "p2lex.h"

#include <string>
#include <istream>
#include <fstream>
#include <map>

enum NodeType {
	GENERIC,

	OP_PLUS,
	OP_STAR,
	OP_BRACKETS,

	STMT_PRINT,
	STMT_SET,

	TYPE_ID,
	TYPE_INT,
	TYPE_STR
};

class ParseTree {
protected:
	ParseTree *leftChild;
	ParseTree *rightChild;

	int whichLine;
	NodeType nodeType;

public:
	ParseTree(NodeType type, ParseTree *left = 0, ParseTree *right = 0) : nodeType(type), leftChild(left), rightChild(right) {
		whichLine = linenum;
	}

	ParseTree *left() { return leftChild; }
	ParseTree *right() { return rightChild; }

	int onWhichLine() { return whichLine; }
	NodeType getType() { return nodeType; }
};

class ParseTreeToken : public ParseTree {
protected:
	Token tok;

public:
	ParseTreeToken(NodeType type, Token &t, ParseTree *left = 0, ParseTree *right = 0) : ParseTree(type, left, right), tok(t) { }

	Token getToken() { return tok; }
};

int linenum = 0;
int globalErrorCount = 0;

void error(std::string msg) {
	std::cout << linenum << ": " << msg << std::endl;
	globalErrorCount += 1;
}

ParseTree *Program(std::istream *in);
ParseTree *StmtList(std::istream *in);
ParseTree *Stmt(std::istream *in);
ParseTree *Expr(std::istream *in);
ParseTree *Term(std::istream *in);
ParseTree *Primary(std::istream *in);
ParseTree *String(std::istream *in);

Token savedToken;
bool hasSavedToken = false;

Token doGetToken(std::istream *in) {
	if (hasSavedToken) {
		hasSavedToken = false;
		return savedToken;
	}

	return getToken(in);
}

void saveToken(Token &t) {
	if (hasSavedToken) {
		error("A token was already saved.");
		return;
	}

	savedToken = t;
	hasSavedToken = true;
}

/* <Program> := <StmtList> */
ParseTree *Program(std::istream *in) {
	ParseTree *result = StmtList(in);

	if (doGetToken(in).getTok() != DONE) {
		return 0;
	}

	return result;
}

/* <StmtList> := <Stmt> {<Stmt>} */
ParseTree *StmtList(std::istream *in) {
	ParseTree *left = Stmt(in);

	if (left == 0) {
		return 0;
	}
	
	ParseTree *right = StmtList(in);
	return new ParseTree(GENERIC, left, right);
}

/* <Stmt> := PRINT <Expr> SC | SET ID <Expr> SC */
ParseTree *Stmt(std::istream *in) {
	Token t = doGetToken(in);

	if (t.getTok() == PRINT) {
		ParseTree *left = Expr(in);

		if (left == 0) {
			error("Syntax error, invalid statement");
			return 0;
		}

		Token t = doGetToken(in);

		if (t.getTok() != SC) {
			error("Expected semicolon, received " + t.getLexeme() + ".");
			return 0;
		}

		return new ParseTree(STMT_PRINT, left);
	} else if (t.getTok() == SET) {
		t = doGetToken(in);

		if (t.getTok() != ID) {
			error("Expected id, received " + t.getLexeme() + ".");
			return 0;
		}

		ParseTree *left = new ParseTreeToken(TYPE_ID, t);
		ParseTree *right = Expr(in);

		if (right == 0) {
			error("Syntax error, invalid statement");
			return 0;
		}

		t = doGetToken(in);

		if (t.getTok() != SC) {
			error("Expected semicolon, received " + t.getLexeme() + ".");
			return 0;
		}

		return new ParseTree(STMT_SET, left, right);
	} else if (t.getTok() == DONE) {
		return 0;
	}
    
	error("Syntax error, invalid statement");
	return 0;
}

/* <Expr> := <Term> {+ <Expr>} */
ParseTree *Expr(std::istream *in) {
	ParseTree *left = Term(in);

	if (left == 0) {
		error("Syntax error, invalid expression");
		return 0;
	}

	Token t = doGetToken(in);

	if (t.getTok() == PLUS) {
		ParseTree *right = Expr(in);

		if (right == 0) {
			error("Syntax error, invalid expression");
			return 0;
		}

		return new ParseTree(OP_PLUS, left, right);
	}

	saveToken(t);
	return left;
}

/* <Term> := <Primary> {* <Term>} */
ParseTree *Term(std::istream *in) {
	ParseTree *left = Primary(in);

	if (left == 0) {
		error("Syntax error, invalid term");
		return 0;
	}

	Token t = doGetToken(in);

	if (t.getTok() == STAR) {
		ParseTree *right = Term(in);

		if (right == 0) {
			error("Syntax error, invalid term");
			return 0;
		}

		return new ParseTree(OP_STAR, left, right);
	}

	saveToken(t);
	return left;
}

/* <Primary> := ID | <String> | INT | LPAREN <Expr> RPAREN */
ParseTree *Primary(std::istream *in) {
	Token t = doGetToken(in);

	if (t.getTok() == ID) {
		return new ParseTreeToken(TYPE_ID, t);
	} else if (t.getTok() == INT) {
		return new ParseTreeToken(TYPE_INT, t);
	} else if (t.getTok() == STR) {
		saveToken(t);
		return String(in);
	} else if (t.getTok() == LPAREN) {
		ParseTree *expr = Expr(in);

		if (expr == 0) {
			error("Syntax error, invalid expression");
			return 0;
		}

		t = doGetToken(in);

		if (t.getTok() != RPAREN) {
			error("Expected a right parentheses, received " + t.getLexeme() + ".");
			return 0;
		}

		return expr;
	}

	error("Syntax error, invalid primary");
	return 0;
}

/* <String> := STR | STR LEFTSQ <Expr> RIGHTSQ | STR LEFTSQ <Expr> SC <Expr> RIGHTSQ */
ParseTree *String(std::istream *in) {
	Token t = doGetToken(in);

	if (t.getTok() == STR) {
		Token t1 = doGetToken(in);

		if (t1.getTok() == LEFTSQ) {
			ParseTree *left = Expr(in);

			t1 = doGetToken(in);

			if (t1.getTok() == RIGHTSQ) {
				return new ParseTreeToken(TYPE_STR, t, left);
			} else if (t1.getTok() == SC) {
				ParseTree *right = Expr(in);

				t1 = doGetToken(in);

				if (t1.getTok() == RIGHTSQ) {
					return new ParseTreeToken(TYPE_STR, t, left, right);
				}
			}

			error("Expected right square bracket, received " + t.getLexeme() + ".");
			return 0;
		}

		saveToken(t1);
		return new ParseTreeToken(TYPE_STR, t);
	}

	error("Expected string, received " + t.getLexeme() + ".");
	return 0;
}

int plusOpCount = 0;
int starOpCount = 0;
int bracketOpCount = 0;

void traverseAndCount(ParseTree *t) {
	if (!t) return;

	if (t->getType() == TYPE_STR && t->left() != 0) {
		bracketOpCount += 1;
	} else if (t->getType() == OP_PLUS) {
		plusOpCount += 1;
	} else if (t->getType() == OP_STAR) {
		starOpCount += 1;
	}
	
	traverseAndCount(t->left());
	traverseAndCount(t->right());
}

std::map<std::string, bool> id_names;

void traverseAndErrorCheck(ParseTree *t) {
	if (!t) return;

	// ID checking
	if (t->getType() == STMT_SET) {
		id_names.insert(std::pair<std::string, bool>(static_cast<ParseTreeToken *>(t->left())->getToken().getLexeme(), true));
	} else if (t->getType() == TYPE_ID) {
		if (!id_names[static_cast<ParseTreeToken *>(t)->getToken().getLexeme()]) {
			std::cout << "Symbol " << static_cast<ParseTreeToken *>(t)->getToken().getLexeme() << " used without being set at line " << t->onWhichLine() << std::endl;
		}
	}

	// String checking
	if (t->getType() == TYPE_STR && static_cast<ParseTreeToken *>(t)->getToken().getLexeme().length() <= 2) {
		std::cout << "Empty string not permitted on line " << t->onWhichLine() << std::endl;
	}

	traverseAndErrorCheck(t->left());
	traverseAndErrorCheck(t->right());
}

int main(int argc, char **argv) {
	std::istream *in = &std::cin;
	std::fstream infile;

	for (int i = 1; i < argc; i++) {
		if (in != &std::cin) {
			std::cout << "Too many arguments, exiting." << std::endl;
			return 1;
		}

		infile.open(argv[i]);

		if (!infile.is_open()) {
			std::cout << "Could not open file, exiting." << std::endl;
			return 1;
		}

		in = &infile;
	}

	ParseTree *prog = Program(in);

	if (prog == 0 || globalErrorCount != 0) {
		return 1;
	}
	
	traverseAndCount(prog);

	std::cout << "Count of + operators: " << plusOpCount << std::endl;
	std::cout << "Count of * operators: " << starOpCount << std::endl;
	std::cout << "Count of [] operators: " << bracketOpCount << std::endl;

	traverseAndErrorCheck(prog);
	
	return 0;
}
