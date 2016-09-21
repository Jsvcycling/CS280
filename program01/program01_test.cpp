#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv) {
	if (argc < 2) exit(1);

	FILE *filePtr = fopen(argv[1], "r");
	if (filePtr == NULL) exit(1);

	char buf[256];

	while (fgets(buf, 256, filePtr)) {
		char tokbuf[256];
		strcpy(tokbuf, buf);
		char *token = strtok(tokbuf, " \t\n");

		while (token != NULL) {
			if (strncmp(token, ".", 1) == 0) {
				if (strcmp(token, ".ll") == 0) {
					printf("token is line length change... ");
					token = strtok(NULL, " \t\n");
				
					if (token != NULL) {
						if (strlen(buf) > (strlen(".ll ") + strlen(token) + 1)) {
							printf("invalid line length command.\n");
							break;
						} else {
							int new_len = atoi(token);
							printf("new line length is %i.\n", new_len);
						}
					}
					token = strtok(NULL, " \t\n");
				} else {
					break;
				}
			} else {
				printf("%s\n", token);
				token = strtok(NULL,  " \t\n");
			}
		}
	}

	fclose(filePtr);
	return 0;
}
