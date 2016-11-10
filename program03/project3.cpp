#include "p2lex.h"
#include <string>
#include <istream>
#include <fstream>
using namespace std;

/////////
//// this class can be used to represent the parse result
//// the various different items that the parser might recognize can be
//// subclasses of ParseTree

class ParseTree {
private:
	ParseTree *leftChild;
	ParseTree *rightChild;

	int	whichLine;

public:
	ParseTree(ParseTree *left = 0, ParseTree *right = 0) : leftChild(left),rightChild(right) {
		whichLine = linenum;
	}

	int onWhichLine() { return whichLine; }
};

//// for example, an integer...
class Integer : public ParseTree {
private:
	Token	iTok;

public:
	Integer(const Token& iTok) : ParseTree(), iTok(iTok) {}

	int	getInteger() { return stoi( iTok.getLexeme() ); }
};

int linenum = 0;
int globalErrorCount = 0;

/// error handler
void error(string msg)
{
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


ParseTree *Program(istream *in)
{
	ParseTree *result = StmtList(in);

	// make sure there are no more tokens...
	if( getToken(in).getTok() != DONE )
		return 0;

	return result;
}


ParseTree *StmtList(istream *in)
{
	return 0;
}


ParseTree *Stmt(istream *in)
{
	Token t;

	t = getToken(in);

	if( t.getTok() == PRINT ) {
		// process PRINT
	}
	else if( t.getTok() == SET ) {
		// process SET
	}

	else
		error("Invalid statement");

	return 0;
}


ParseTree *Expr(istream *in)
{
	return 0;
}


ParseTree *Term(istream *in)
{
	return 0;
}


ParseTree *Primary(istream *in)
{
	Token t = getToken(in);

	if( t.getTok() == ID ) {
	}
	else if( t.getTok() == INT ) {
		return new Integer(t);
	}
	else if( t.getTok() == STR ) {
	}
	else if( t.getTok() == LPAREN ) {
		ParseTree *ex = Expr(in);
		if( ex == 0 )
			return 0;
		t = getToken(in);
		if( t.getTok() != RPAREN ) {
			error("expected right parens");
			return 0;
		}

		return ex;
	}

	return 0;
}


ParseTree *String(istream *in)
{
	return 0;
}



main()
{
	ParseTree *prog = Program(&cin);

	if( prog == 0 || globalErrorCount != 0 ) {
		cout << "Parse failed, exiting" << endl;
		return 0;
	}

	cout << "Success. Congrats!" << endl;
	return 0;
}
