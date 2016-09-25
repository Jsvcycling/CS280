#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <list>

// #define DEBUG

int main(int argc, char **argv) {
	if (argc < 2) exit(1);

	FILE *filePtr = fopen(argv[1], "r");
	if (filePtr == NULL) exit(1);

	std::list<char *> token_list;
	bool generate = false;

	char buf[256];
	int line_len = 60;
	int next_line_len = line_len;

	while (fgets(buf, 256, filePtr)) {
#ifdef DEBUG
		// printf("%s\t%i", buf, (int)strlen(buf));
#endif
		if (strlen(buf) == 1) {
#ifdef DEBUG
			printf("Empty line...\n");
#endif
			generate = (token_list.size() > 0);
		}
		
		char tokbuf[256];
		strcpy(tokbuf, buf);
		char *token = strtok(tokbuf, " \t\n");

		while (token != NULL && !generate) {
			if (strncmp(token, ".", 1) == 0) {
				if (strcmp(token, ".ll") == 0) {
#ifdef DEBUG
					printf("token is line length change... ");
#endif
					token = strtok(NULL, " \t\n");
				
					if (token != NULL) {
						if (strlen(buf) > (strlen(".ll ") + strlen(token) + 1)) {
#ifdef DEBUG
							printf("invalid line length command.\n");
#endif
						} else {
							int new_len = atoi(token);
							
							if (new_len > 10 && new_len < 120) {
#ifdef DEBUG
								printf("new line length is %i.\n", new_len);
#endif
								next_line_len = new_len;
							} else {
#ifdef DEBUG
								printf("invalid line length (%i).\n", new_len);
#endif
							}
						}
					}
				}

				break;
			} else {
#ifdef DEBUG
				printf("%s\n", token);
#endif
				token_list.push_back(token);
				token = strtok(NULL,  " \t\n");
			}
		}
	}

	fclose(filePtr);
	return 0;
}
