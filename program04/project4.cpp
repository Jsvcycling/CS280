#include "p2lex.h"

#include <string>
#include <istream>
#include <fstream>
#include <map>

using namespace std;

/// Token Wrapper
Token saved;
bool isSaved = false;

Token GetAToken(istream *in) {
    if( isSaved ) {
        isSaved = false;
        return saved;
    }

    return getToken(in);
}

void PushbackToken(Token& t) {
    if( isSaved ) {
        cerr << "Can't push back more than one token!!!";
        exit(0);
    }

    saved = t;
    isSaved = true;
}

int linenum = 0;
int globalErrorCount = 0;

/// error handler
void error(string msg, bool showline=true) {
    if( showline )
        cout << linenum << ": ";
    cout << msg << endl;
    ++globalErrorCount;
}

void runtimeError(string msg) {
	cout << "RUNTIME ERROR: " << msg << endl;
}

enum ValueType {
    INTEGER,
    STRING,

    ERRORTYPE,
	PASSTYPE
};

class Value {
private:
    ValueType type;

protected:
    Value(ValueType type) : type(type) { }

public:
    Value() : type(PASSTYPE) { }

    ValueType getType() {
       return this->type;
    }

    virtual ostream& operator <<(ostream &strm) {
		return strm;
	}
};

class ErrValue : public Value {
public:
	ErrValue() : Value(ERRORTYPE) { }
};

class IntValue : public Value {
public:
    int val;

    IntValue(int val = 0) : Value(INTEGER), val(val) { }
};

class StrValue : public Value {
public:
    string val;

    StrValue(const string &val = "") : Value(STRING), val(val) { }
};

map<string, Value *> symbolTable;

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

    int onWhichLine() {
        return whichLine;
    }

    int traverseAndCount(int (ParseTree::*f)()) {
        int cnt = 0;
        if( leftChild ) cnt += leftChild->traverseAndCount(f);
        if( rightChild ) cnt += rightChild->traverseAndCount(f);
        return cnt + (this->*f)();
    }

    int countUseBeforeSet( map<string,int>& symbols ) {
        int cnt = 0;
        if( leftChild ) cnt += leftChild->countUseBeforeSet(symbols);
        if( rightChild ) cnt += rightChild->countUseBeforeSet(symbols);
        return cnt + this->checkUseBeforeSet(symbols);
    }

    ParseTree *getLeftChild() const {
        return this->leftChild;
    }
    ParseTree *getRightChild() const {
        return this->rightChild;
    }

    virtual int checkUseBeforeSet( map<string,int>& symbols ) {
        return 0;
    }

    virtual int isPlus() {
        return 0;
    }
    virtual int isStar() {
        return 0;
    }
    virtual int isBrack() {
        return 0;
    }
    virtual int isEmptyString() {
        return 0;
    }

    virtual Value *eval() {
		return new ErrValue();
	}
};

class Slist : public ParseTree {
public:
    Slist(ParseTree *left, ParseTree *right) : ParseTree(left,right) {}

    Value *eval() {
		Value *left = new ErrValue();
		Value *right = new ErrValue();

		if (getLeftChild() != nullptr) {
			left = getLeftChild()->eval();
		}
		
		if (getRightChild() != nullptr) {
			right = getRightChild()->eval();
		}

		if (left->getType() == ERRORTYPE) {
			return new ErrValue();
		}
		
		return new Value();
    }
};

class PrintStmt : public ParseTree {
public:
    PrintStmt(ParseTree *expr) : ParseTree(expr) {}

    Value *eval() {
        Value *value = getLeftChild()->eval();

		if (value->getType() == ERRORTYPE) {
			return new ErrValue();
		} else if (value->getType() == INTEGER) {
			std::cout << static_cast<IntValue *>(value)->val << endl;
		} else if (value->getType() == STRING) {
			std::cout << static_cast<StrValue *>(value)->val << endl;
		}
		
        return new Value();
    }
};

class SetStmt : public ParseTree {
private:
    string	ident;

public:
    SetStmt(string id, ParseTree *expr) : ParseTree(expr), ident(id) {}

    int checkUseBeforeSet( map<string,int>& symbols ) {
        symbols[ident]++;
        return 0;
    }

    Value *eval() {
        Value *val = getLeftChild()->eval();

		if (val->getType() == ERRORTYPE) {
			return new ErrValue();
		}

        if (symbolTable.find(ident) != symbolTable.end()) {
            symbolTable.find(ident)->second = val;
        } else {
            symbolTable.insert(pair<string, Value *>(ident, val));
        }

        return val;
    }
};

class PlusOp : public ParseTree {
public:
    PlusOp(ParseTree *left, ParseTree *right) : ParseTree(left,right) {}
    int isPlus() {
        return 1;
    }

    Value *eval() {
        Value *left = getLeftChild()->eval();
        Value *right = getRightChild()->eval();

        if (left->getType() == INTEGER && right->getType() == INTEGER) {
            return new IntValue(static_cast<IntValue *>(left)->val + static_cast<IntValue *>(right)->val);
        } else if (left->getType() == STRING && right->getType() == STRING) {
            return new StrValue(static_cast<StrValue *>(left)->val + static_cast<StrValue *>(right)->val);
        } else {
			runtimeError("types being added are not permitted");
            return new ErrValue();
        }
    }
};

class StarOp : public ParseTree {
public:
    StarOp(ParseTree *left, ParseTree *right) : ParseTree(left,right) {}
    int isStar() {
        return 1;
    }

    Value *eval() {
        Value *left = getLeftChild()->eval();
        Value *right = getRightChild()->eval();

        if (left->getType() == INTEGER && right->getType() == INTEGER) {
            return new IntValue(static_cast<IntValue *>(left)->val * static_cast<IntValue *>(right)->val);
        } else if (left->getType() == INTEGER && right->getType() == STRING) {
			int count = static_cast<IntValue *>(left)->val;
			string instr = static_cast<StrValue *>(right)->val;
			string outstr = "";

			for (int i = 0; i < count; i++) {
				outstr += instr;
			}

			return new StrValue(outstr);
		} else if (left->getType() == STRING && right->getType() == INTEGER) {
			string instr = static_cast<StrValue *>(left)->val;
			int count = static_cast<IntValue *>(right)->val;
			string outstr = "";

			for (int i = 0; i < count; i++) {
				outstr += instr;
			}

			return new StrValue(outstr);
		} else {
			runtimeError("types being multiplied are not permitted");
			return new ErrValue();
        }
    }
};

class BracketOp : public ParseTree {
private:
    Token sTok;

public:
    BracketOp(const Token& sTok, ParseTree *left, ParseTree *right = 0) : ParseTree(left,right), sTok(sTok) {}
    int isBrack() {
        return 1;
    }

    Value *eval() {
		Value *left = new ErrValue();
		Value *right = new ErrValue();
		
		if (getLeftChild() != nullptr) {
			left = getLeftChild()->eval();
		}

		if (getRightChild() != nullptr) {
			right = getRightChild()->eval();
		}

		if (left->getType() == INTEGER && right->getType() == ERRORTYPE) {
			int pos = static_cast<IntValue *>(left)->val;
			
			if (pos >= sTok.getLexeme().length()) {
				runtimeError("index position out of bounds");
				return new ErrValue();
			}
			
			string str = sTok.getLexeme().substr(pos, 1);

			// if (str.length() != 1) {
			// 	runtimeError("no characters selected")
			// 	return new ErrValue();
			// } else {
			// 	return new StrValue(str);
			// }

			return new StrValue(str);
		} else if (left->getType() == INTEGER && right->getType() == INTEGER) {
			int pos1 = static_cast<IntValue *>(left)->val;
			int pos2 = static_cast<IntValue *>(right)->val;

			if (pos1 >= sTok.getLexeme().length() || pos2 >= sTok.getLexeme().length()) {
				runtimeError("index position(s) out of bounds");
				return new ErrValue();
			}
			
			string str = sTok.getLexeme().substr(pos1, (pos2-pos1));

			// if (str.length () <= 0) {
			// 	return new ErrValue();
			// } else {
			// 	return new StrValue(str);
			// }

			return new StrValue(str);
		} else {
			runtimeError("invalid index parameters");
			return new ErrValue();
		}
    }
};

class StringConst : public ParseTree {
private:
    Token sTok;

public:
    StringConst(const Token& sTok) : ParseTree(), sTok(sTok) {}

    string	getString() {
		return sTok.getLexeme();
    }

    int isEmptyString() {
        if( sTok.getLexeme().length() == 2 ) {
            error("Empty string not permitted on line " + to_string(onWhichLine()), false );
            return 1;
        }
        return 0;
    }

    Value *eval() {

		// if (str.length() > 2) {
		// 	str = str.substr(1, str.length() - 2);
		// }

		return new StrValue(getString().substr(1, getString().size() - 2));
    }
};

//// for example, an integer...
class Integer : public ParseTree {
private:
    Token	iTok;

public:
    Integer(const Token& iTok) : ParseTree(), iTok(iTok) {}

    int	getInteger() {
        return stoi( iTok.getLexeme() );
    }

    Value *eval() {
        return new IntValue(getInteger());
    }
};

class Identifier : public ParseTree {
private:
    Token	iTok;

public:
    Identifier(const Token& iTok) : ParseTree(), iTok(iTok) {}

    int checkUseBeforeSet( map<string,int>& symbols ) {
        if( symbols.find( iTok.getLexeme() ) == symbols.end() ) {
            error("Symbol " + iTok.getLexeme() + " used without being set at line " + to_string(onWhichLine()), false);
            return 1;
        }
        return 0;
    }

    Value *eval() {
        if (symbolTable.find(iTok.getLexeme()) != symbolTable.end()) {
            return symbolTable.find(iTok.getLexeme())->second;
        } else {
			cout << "bad id" << endl;
			return new ErrValue();
        }
    }
};

/// function prototypes
ParseTree *Program(istream *in);
ParseTree *StmtList(istream *in);
ParseTree *Stmt(istream *in);
ParseTree *Expr(istream *in);
ParseTree *Term(istream *in);
ParseTree *Primary(istream *in);
ParseTree *String(istream *in);


ParseTree *Program(istream *in) {
    ParseTree *result = StmtList(in);

    // make sure there are no more tokens...
    if( GetAToken(in).getTok() != DONE )
        return 0;

    return result;
}


ParseTree *StmtList(istream *in) {
    ParseTree *stmt = Stmt(in);

    if( stmt == 0 )
        return 0;

    return new Slist(stmt, StmtList(in));
}


ParseTree *Stmt(istream *in) {
    Token t;

    t = GetAToken(in);

    if( t.getTok() == ERR ) {
        error("Invalid token");
        return 0;
    }

    if( t.getTok() == DONE )
        return 0;

    if( t.getTok() == PRINT ) {
        // process PRINT
        ParseTree *ex = Expr(in);

        if( ex == 0 ) {
            error("Expecting expression after print");
            return 0;
        }

        if( GetAToken(in).getTok() != SC ) {
            error("Missing semicolon");
            return 0;
        }

        return new PrintStmt(ex);
    } else if( t.getTok() == SET ) {
        // process SET
        Token tid = GetAToken(in);

        if( tid.getTok() != ID ) {
            error("Expecting identifier after set");
            return 0;
        }

        ParseTree *ex = Expr(in);

        if( ex == 0 ) {
            error("Expecting expression after identifier");
            return 0;
        }

        if( GetAToken(in).getTok() != SC ) {
            error("Missing semicolon");
            return 0;
        }

        return new SetStmt(tid.getLexeme(), ex);
    } else {
        error("Syntax error, invalid statement");
    }

    return 0;
}


ParseTree *Expr(istream *in) {
    ParseTree *exp = Term( in );

    if( exp == 0 ) return 0;

    while( true ) {

        Token t = GetAToken(in);

        if( t.getTok() != PLUS ) {
            PushbackToken(t);
            break;
        }

        ParseTree *exp2 = Term( in );
        if( exp2 == 0 ) {
            error("missing operand after +");
            return 0;
        }

        exp = new PlusOp(exp, exp2);
    }

    return exp;
}


ParseTree *Term(istream *in) {
    ParseTree *pri = Primary( in );

    if( pri == 0 ) return 0;

    while( true ) {

        Token t = GetAToken(in);

        if( t.getTok() != STAR ) {
            PushbackToken(t);
            break;
        }

        ParseTree *pri2 = Primary( in );
        if( pri2 == 0 ) {
            error("missing operand after *");
            return 0;
        }

        pri = new StarOp(pri, pri2);
    }

    return pri;
}


ParseTree *Primary(istream *in) {
    Token t = GetAToken(in);

    if( t.getTok() == ID ) {
        return new Identifier(t);
    } else if( t.getTok() == INT ) {
        return new Integer(t);
    } else if( t.getTok() == STR ) {
        PushbackToken(t);
        return String(in);
    } else if( t.getTok() == LPAREN ) {
        ParseTree *ex = Expr(in);
        if( ex == 0 )
            return 0;
        t = GetAToken(in);
        if( t.getTok() != RPAREN ) {
            error("expected right parens");
            return 0;
        }

        return ex;
    }

    return 0;
}


ParseTree *String(istream *in) {
    Token t = GetAToken(in); // I know it's a string!
    ParseTree *lexpr, *rexpr;

    Token lb = GetAToken(in);
    if( lb.getTok() != LEFTSQ ) {
        PushbackToken(lb);
        return new StringConst(t);
    }

    lexpr = Expr(in);
    if( lexpr == 0 ) {
        error("missing expression after [");
        return 0;
    }

    lb = GetAToken(in);
    if( lb.getTok() == RIGHTSQ ) {
        return new BracketOp(t, lexpr);
    } else if( lb.getTok() != SC ) {
        error("expected ; after first expression in []");
        return 0;
    }

    rexpr = Expr(in);
    if( rexpr == 0 ) {
        error("missing expression after ;");
        return 0;
    }

    lb = GetAToken(in);
    if( lb.getTok() == RIGHTSQ ) {
        return new BracketOp(t, lexpr, rexpr);
    }

    error("expected ]");
    return 0;
}


int main(int argc, char *argv[]) {
    istream *in = &cin;
    ifstream infile;

    for( int i = 1; i < argc; i++ ) {
        if( in != &cin ) {
            cerr << "Cannot specify more than one file" << endl;
            return 1;
        }

        infile.open(argv[i]);
        if( infile.is_open() == false ) {
            cerr << "Cannot open file " << argv[i] << endl;
            return 1;
        }

        in = &infile;
    }

    ParseTree *prog = Program(in);

    if( prog == 0 || globalErrorCount != 0 ) {
        return 0;
    }

    // prog->traverseAndCount( &ParseTree::isEmptyString );

    map<string,int> symbols;
    int useBeforeSetCount = prog->countUseBeforeSet( symbols );

	Value *val = prog->eval();
	
    return 0;
}
