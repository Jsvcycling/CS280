#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <list>

#define DEBUG

void print_line(std::list<char *> *tokens, int line_len) {
	std::list<char *> print_list;
	int slots_left = line_len;
	int space_slots = 0;

	while (tokens->size() > 0) {
#ifdef DEBUG
		// printf("space_slots = %i\tslots_left = %i\n", space_slots, slots_left);
		printf("tokens.size(): %i\n", (int)tokens->size());
#endif
		if ((space_slots * 3) < slots_left) {
			if (strlen(tokens->front()) < (slots_left - space_slots)) {
#ifdef DEBUG
				printf("Next token: %s\tstrlen: %i\n", tokens->front(), (int)strlen(tokens->front()));
#endif
				slots_left -= strlen(tokens->front());
				space_slots += 1;

				print_list.push_back(tokens->front());
				tokens->pop_front();
			} else if (strlen(tokens->front()) == (slots_left - space_slots)) {
#ifdef DEBUG
				printf("Final token: %s\n", tokens->front());
#endif
				slots_left -= strlen(tokens->front());

				print_list.push_back(tokens->front());
				tokens->pop_front();
				break;
			} else {
				// TODO: split the word.

				slots_left -= strlen(tokens->front());
				print_list.push_back(tokens->front());
				tokens->pop_front();
				break;
			}
		} else {
			break;
		}
	}
}

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
		if (strlen(buf) <= 1) {
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
				printf("%s\t%i\n", token, (int)strlen(token));
#endif
				token_list.push_back(token);
				token = strtok(NULL, " \t\n");
			}
		}

		if (generate) {
#ifdef DEBUG
			printf("Let's generate a paragraph...\n");
#endif
			while (token_list.size() > 0) {
				print_line(&token_list, line_len);
			}
			
			// token_list.clear();
		}
	}

	if (token_list.size() > 0) {
#ifdef DEBUG
		printf("Let's generate our final paragraph...\n");
#endif
		while (token_list.size() > 0) {
			print_line(&token_list, line_len);
		}
		
		// token_list.clear();
	}

	fclose(filePtr);
	return 0;
}
