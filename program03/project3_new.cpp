#include "p2lex.h"

#include <string>
#include <istream>
#include <fstream>

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
	ParseTreeToken(NodeType type, Token &t) : ParseTree(type), tok(t) { }

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
		// ERROR
		return 0;
	}
	
	ParseTree *right = StmtList(in);

	if (right == 0) {
		// ERROR
		return 0;
	}

	return new ParseTree(GENERIC, left, right);
}

/* <Stmt> := PRINT <Expr> SC | SET ID <Expr> SC */
ParseTree *Stmt(std::istream *in) {
	Token t = doGetToken(in);

	if (t.getTok() == PRINT) {
		ParseTree *left = Expr(in);

		if (left == 0) {
			// ERROR
			return 0;
		}

		t = doGetToken(in);

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
			// ERROR
			return 0;
		}

		t = doGetToken(in);

		if (t.getTok() != SC) {
			error("Expected semiolon, received " + t.getLexeme() + ".");
			return 0;
		}

		return new ParseTree(STMT_SET, left,  right);
	}

	error("Invalid statement, received " + t.getLexeme() + ".");
	return 0;
}

/* <Expr> := <Term> {+ <Term>} */
ParseTree *Expr(std::istream *in) {
	ParseTree *left = Term(in);

	if (left == 0) {
		// ERROR
		return 0;
	}

	Token t = doGetToken(in);

	if (t.getTok() == PLUS) {
		ParseTree *right = Expr(in);

		if (right == 0) {
			// ERROR
			return 0;
		}

		return new ParseTree(OP_PLUS, left, right);
	}

	saveToken(t);
	return left;
}

/* <Term> := <Primary> {* <Primary>} */
ParseTree *Term(std::istream *in) {
	ParseTree *left = Primary(in);

	if (left == 0) {
		// ERROR
		return 0;
	}

	Token t = doGetToken(in);

	if (t.getTok() == STAR) {
		ParseTree *right = Term(in);

		if (right == 0) {
			// ERROR
			return 0;
		}

		return new ParseTree(OP_STAR, left, right);
	}

	saveToken(t);
	return left;
}

/* <Primary> := ID | <String> | INT | LPAREN <Expr> RPAREN */
ParseTree *Primary(std::istream *in) {
	// TODO
	return 0;
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
		std::cout << "Parse failed, exiting." << std::endl;
		return 1;
	}

	std::cout << "Success. Congrats!" << std::endl;
	return 0;
}
