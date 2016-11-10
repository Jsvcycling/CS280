// test version of main

#include <map>
#include <fstream>
#include "p2lex.h"
using namespace std;

// defines the linenum declared as extern in p2lex.h
int	linenum = 0;

extern string getPrintName(TokenType t);

int main(int argc, char *argv[])
{
	istream *in = &cin;
	fstream infile;

	bool	verbose;

	for( int i=1; i<argc; i++ ) {
		if( string(argv[i]) == "-v" )
			verbose = true;
		else {
			if( in != &cin ) {
				cerr << "Too many file names specified" << endl;
				return 1;
			}
			infile.open(argv[i]);
			if( !infile.is_open() ) {
				cerr << "Could not open file named: " << argv[i] << endl;
				return 1;
			}

			in = &infile;
		}
	}


	Token	tok;
	Token	doneTok(DONE,"");
	Token	errTok(ERR,"");

	map<TokenType,int> counters;
	map<TokenType,map<string,int>>	lexemeUniq;

	while( !((tok = getToken(in)) == doneTok) && !(tok == errTok) ) {
		if( verbose )
			cout << tok << endl;

		TokenType tt = tok.getTok();
		counters[tt]++;
		lexemeUniq[tt][tok.getLexeme()]++;
	}

	if( tok == errTok ) {
		cout << tok << endl;
		return 1;
	}

	for( map<TokenType,int>::iterator it = counters.begin(); it != counters.end(); it++ ) {
		cout << getPrintName(it->first) << ": " << it->second << endl;
	}

	cout << "Number of unique lexemes for " << getPrintName(ID) << ": " << lexemeUniq[ID].size() << endl;
	cout << "Number of unique lexemes for " << getPrintName(STR) << ": " << lexemeUniq[STR].size() << endl;
	cout << "Number of unique lexemes for " << getPrintName(INT) << ": " << lexemeUniq[INT].size() << endl;

	return 0;
}

