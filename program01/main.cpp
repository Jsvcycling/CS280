//-------------------------------------------------
// Title: CS 280 Program 01
// Author: Joshua Vega <jsv28@njit.edu>
// Date: 2 October, 2016
//-------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <list>

void split_word(std::list<char *> *tokens, int word_len) {
	char *word = tokens->front();

	int first_part_len = word_len;
	int second_part_len = strlen(word) - word_len;

	char *first_part = (char *)malloc(sizeof(char) * (first_part_len + 1));
	char *second_part = (char *)malloc(sizeof(char) * (second_part_len + 2));
	
	memset(first_part, '\0', first_part_len + 1);
	memset(second_part, '\0', second_part_len + 2);
	
	strncpy(first_part, word, first_part_len);
	strncpy(second_part, &word[first_part_len - 1], second_part_len + 1);

	first_part[first_part_len - 1] = '-';

	tokens->pop_front();
	tokens->push_front(second_part);
	tokens->push_front(first_part);

	free(word);
}

void print_line(std::list<char *> *tokens, int line_len) {
	std::list<char *> print_list;
	int slots_left = line_len;

	while (tokens->size() > 0) {
		if (strlen(tokens->front()) == line_len) {
			printf("%s\n", tokens->front());
			tokens->pop_front();
			return;
		} else if (strlen(tokens->front()) == (slots_left - print_list.size())) {
			slots_left -= strlen(tokens->front());
			print_list.push_back(tokens->front());
			tokens->pop_front();
			break;
		} else if (strlen(tokens->front()) < (slots_left - print_list.size())) {
			slots_left -= strlen(tokens->front());
			print_list.push_back(tokens->front());
			tokens->pop_front();
		} else if (strlen(tokens->front()) > line_len) {
			split_word(tokens, line_len);
		} else if (((print_list.size() - 1) * 3) < slots_left) {
			split_word(tokens, (slots_left - print_list.size()));
		} else {
			break;
		}
	}

	for (std::list<char *>::iterator it = print_list.begin(); it != print_list.end(); ++it) {
		if (strlen(*it) < 1) {
			it = print_list.erase(it);
		}
	}
	
	if (print_list.size() == 1 && slots_left < 3) {
		for (int i = 0; i < slots_left; i++) {
			printf(" ");
		}

		char *str = print_list.front();
		print_list.pop_front();
		printf("%s\n", str);
		// free(str);
	} else {
		int spaces_per_slot = 0;
		int remaining_spaces = 0;

		bool use_remaining[print_list.size() - 1];
		memset(use_remaining, false, print_list.size() - 1);

		if (print_list.size() > 1) {
			if (((print_list.size() - 1) * 3) < slots_left) {
				spaces_per_slot = 1;
			} else {
				spaces_per_slot = slots_left / (print_list.size() - 1);
				remaining_spaces = slots_left - (spaces_per_slot * (print_list.size() - 1));

				srand(time(NULL));

				for (int i = 0; i < remaining_spaces; i++) {
					while (true) {
						int idx = rand() % (print_list.size() - 1);

						if (!use_remaining[idx]) {
							use_remaining[idx] = true;
							break;
						}
					}
				}
			}
		}

		int j = 0;
		while (print_list.size() > 0) {
			char *str = print_list.front();
			print_list.pop_front();
			printf("%s", str);
			free(str);

			for (int k = 0; k < spaces_per_slot; k++) {
				if (print_list.size() > 0) {
					printf(" ");
				}
			}

			if (use_remaining[j]) {
				printf(" ");
			}

			j++;
		}

		printf("\n");
	}
}

void parse_paragraph(FILE *filePtr, int *line_len) {
	std::list<char *> token_list;
	char buf[256];
	bool generate = false;

	int next_line_len = *line_len;
	
	while (true) {
		if (fgets(buf, 256, filePtr)) {
			if (strlen(buf) <= 1) {
				generate = (token_list.size() > 0);
				break;
			}
		    
			char tokbuf[256];
			strcpy(tokbuf, buf);
			char *token = strtok(tokbuf, " \t\n");
			
			if (strncmp(token, ".", 1) == 0) {
				if (strcmp(token, ".ll") == 0) {
					token = strtok(NULL, " \t\n");

					if (token != NULL) {
						if (strlen(buf) == (strlen(".ll ") + strlen(token) + 1)) {
						    int new_len = atoi(token);

							if (new_len >= 10 && new_len < 120) {
								next_line_len = new_len;
								generate = (token_list.size() > 0);
								break;
							}
						}
					}
				}
			} else {
				while (token != NULL) {
					char *str = (char *)malloc(sizeof(char) * strlen(token));
					
					strcpy(str, token);
					token_list.push_back(str);
					token = strtok(NULL, " \t\n");
				}
			}
		} else {
			while (token_list.size() > 0) {
				print_line(&token_list, *line_len);
			}
			
			return;
		}
	}

	if (generate) {
		while (token_list.size() > 0) {
			print_line(&token_list, *line_len);
		}

		printf("\n");
	}

	*line_len = next_line_len;
}

int main(int argc, char **argv) {
	if (argc < 2) exit(1);

	FILE *filePtr = fopen(argv[1], "r");
	if (filePtr == NULL) exit(1);
	
	int line_len = 60;

	while (!feof(filePtr)) {
		parse_paragraph(filePtr, &line_len);
	}

	fclose(filePtr);
	return 0;
}
