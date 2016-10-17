#include "p2lex.h"

#include <stdlib.h>
#include <string.h>
#include <map>

int main(int argc, char **argv) {
	bool verbose = false;

	if (argc < 2) exit(EXIT_FAILURE);

	// Check for the verbose parameter
	if (argc == 3 && strcmp(argv[1], "-v") == 0) {
		verbose = true;
	}

	// TODO

	exit(EXIT_SUCCESS);
}
