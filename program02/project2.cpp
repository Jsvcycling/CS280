#include "p2lex.h"

#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <list>
#include <map>

int main(int argc, char **argv) {
	bool verbose = false;

	if (argc < 2 || argc > 3) exit(EXIT_FAILURE);

	// Check for the verbose parameter
	if (argc == 3 && strcmp(argv[1], "-v") == 0) {
		verbose = true;
	}

	std::list<Token> tokens;
	std::map<std::string, bool> unique_id;
	std::map<std::string, bool> unique_str;
	std::map<std::string, bool> unique_int;
	
	std::map<TokenType, int> token_count;

	std::ifstream file;

	if (verbose) {
		file.open(argv[2], std::ifstream::in);
	} else {
		file.open(argv[1], std::ifstream::in);
	}

	Token token = getToken(&file);
	
	while (token.getTok() != ERR && token.getTok() != DONE) {
		tokens.push_back(token);
		token_count[token.getTok()] += 1;

		if (token.getTok() == ID) {
			if (!unique_id[token.getLexeme()]) {
				unique_id[token.getLexeme()] = true;
			}
		}

		if (token.getTok() == STR) {
			if (!unique_str[token.getLexeme()]) {
				unique_str[token.getLexeme()] = true;
			}
		}

		if (token.getTok() == INT) {
			if (!unique_int[token.getLexeme()]) {
				unique_int[token.getLexeme()] = true;
			}
		}

		token = getToken(&file);
	}

	if (token.getTok() == ERR) {
		std::cout << token << std::endl;
		exit(EXIT_FAILURE);
	}

	if (verbose) {
		for (Token t : tokens) {
			std::cout << t << std::endl;
		}
	}

	if (token_count[ID] > 0) std::cout << "id: " << token_count[ID] << std::endl;
	if (token_count[STR] > 0) std::cout << "str: " << token_count[STR] << std::endl;
	if (token_count[INT] > 0) std::cout << "int: " << token_count[INT] << std::endl;
	if (token_count[PLUS] > 0) std::cout << "plus: " << token_count[PLUS] << std::endl;
	if (token_count[STAR] > 0) std::cout << "star: " << token_count[STAR] << std::endl;
	if (token_count[LEFTSQ] > 0) std::cout << "leftsq: " << token_count[LEFTSQ] << std::endl;
	if (token_count[RIGHTSQ] > 0) std::cout << "rightsq: " << token_count[RIGHTSQ] << std::endl;
	if (token_count[PRINT] > 0) std::cout << "print: " << token_count[PRINT] << std::endl;
	if (token_count[SET] > 0) std::cout << "set: " << token_count[SET] << std::endl;
	if (token_count[SC] > 0) std::cout << "sc: " << token_count[SC] << std::endl;
	if (token_count[LPAREN] > 0) std::cout << "lparen: " << token_count[LPAREN] << std::endl;
	if (token_count[RPAREN] > 0) std::cout << "rparen: " << token_count[RPAREN] << std::endl;

	std::cout << "Number of unique lexemes for id: " << unique_id.size() << std::endl;
	std::cout << "Number of unique lexemes for str: " << unique_str.size() << std::endl;
	std::cout << "Number of unique lexemes for int: " << unique_int.size() << std::endl;

	file.close();

	exit(EXIT_SUCCESS);
}
