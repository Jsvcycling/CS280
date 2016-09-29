#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <list>

// #define DEBUG

void split_word(std::list<char *> *tokens, int word_len) {
	char *word = tokens->front();

	// Careful, this can become unruly with large input files!!!
	char *first_part = (char *)malloc(sizeof(char) * (word_len + 1));
	char *second_part = (char *)malloc(sizeof(char) * ((strlen(word) - word_len) + 1));
	
	memset(first_part, '\0', word_len);
	memset(second_part, '\0', (strlen(tokens->front()) - word_len));
	
	strncpy(first_part, word, word_len - 1);
	strncpy(second_part, &word[word_len - 1], (strlen(word) - word_len));

	first_part[word_len - 1] = '-';

	tokens->pop_front();
	tokens->push_front(second_part);
	tokens->push_front(first_part);

#ifdef DEBUG
	printf("first_part = %s\tsecond_part = %s\n", first_part, second_part);
#endif
}

void print_line(std::list<char *> *tokens, int line_len) {
	std::list<char *> print_list;
	int slots_left = line_len;
	int space_slots = 0;

	while (tokens->size() > 0) {
#ifdef DEBUG
		printf("space_slots = %i\tslots_left = %i\n", space_slots, slots_left);
#endif
		if ((space_slots * 3) < slots_left) {
			if (strlen(tokens->front()) < (slots_left - space_slots)) {
#ifdef DEBUG
				printf("Next token: %s\tstrlen: %i\n", tokens->front(), (int)strlen(tokens->front()));
#endif
				slots_left -= strlen(tokens->front());
				print_list.push_back(tokens->front());
				tokens->pop_front();

				if (tokens->size() > 0) {
					space_slots += 1;
				}
			} else if (strlen(tokens->front()) == (slots_left - space_slots)) {
#ifdef DEBUG
				printf("Final token: %s\n", tokens->front());
#endif
				slots_left -= strlen(tokens->front());

				print_list.push_back(tokens->front());
				tokens->pop_front();
				break;
			} else {
				split_word(tokens, (slots_left - space_slots));
				
#ifdef DEBUG
				printf("Next token: %s\tstrlen: %i\n", tokens->front(), (int)strlen(tokens->front()));
#endif
				
				slots_left -= strlen(tokens->front());
				print_list.push_back(tokens->front());
				tokens->pop_front();
				break;
			}
		} else {
			break;
		}
	}

	int spaces_per_slot = 0;
	int remaining_spaces = 0;
	
	if (space_slots > 0) {
		if ((space_slots * 3) >= slots_left) {
			spaces_per_slot = slots_left / space_slots;
			spaces_per_slot = (spaces_per_slot <= 3) ? spaces_per_slot : 3;
			
			if (spaces_per_slot < 3) {
				remaining_spaces = slots_left % space_slots;
			}
		} else {
			spaces_per_slot = 1;
		}
	}

#ifdef DEBUG
	printf("space_slots: %d\tspaces_per_slot: %d\tremaining_spaces: %d\tslots_left: %d\n", space_slots, spaces_per_slot, remaining_spaces, slots_left);
#endif

	while (print_list.size() > 0) {
		printf("%s", print_list.front());
		print_list.pop_front();

		for (int i = 0; i < spaces_per_slot; i++) {
			printf(" ");
		}
	}

	printf("\n");
}

int main(int argc, char **argv) {
	if (argc < 2) exit(1);

	FILE *filePtr = fopen(argv[1], "r");
	if (filePtr == NULL) exit(1);

	std::list<char *> token_list;

	char buf[256];
	int line_len = 60;
	int next_line_len = line_len;

	while (!feof(filePtr)) {
		bool generate = false;
		
		while (fgets(buf, 256, filePtr)) {
			if (strlen(buf) <= 1) {
#ifdef DEBUG
				printf("Empty line...\n");
#endif
				generate = (token_list.size() > 0);
				break;
			}

			char tokbuf[256];
			strcpy(tokbuf, buf);
			char *token = strtok(tokbuf, " \t\n");
		    
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
							
							if (new_len >= 10 && new_len <= 120) {
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

				generate = (token_list.size() > 0);
				break;
			} else {
				while (token != NULL) {
					char *str = (char *)malloc(sizeof(char) * strlen(token));
					strcpy(str, token);

					token_list.push_back(str);
					token = strtok(NULL, " \t\n");
				}
			}
		}

#ifdef DEBUG
		for (std::list<char *>::iterator i = token_list.begin(); i != token_list.end(); ++i) {
			printf("%s\n", *i);
		}
#endif

		if (generate) {
#ifdef DEBUG
			printf("Let's generate a paragraph...\n");
#endif
			while (token_list.size() > 0) {
				print_line(&token_list, line_len);
			}
			
			printf("\n");
		}

		line_len = next_line_len;
	}

	fclose(filePtr);
	return 0;
}
