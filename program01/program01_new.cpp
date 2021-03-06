#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <list>

#define DEBUG

typedef struct {
	int line_length;
	int next_line_length;
} state_t;

void split_word(std::list<char *> token_list, int word_len) {
	char *word = token_list.front();
	int len = strlen(word);
	char first_part[word_len + 2];
	char second_part[(len - word_len) + 1];
	
	token_list.pop_front();
	strncpy(first_part, word, word_len - 1);
	first_part[word_len - 2] = '-';
	first_part[word_len - 1] = '\0';

	strncpy(second_part, word + word_len, len - word_len);
	second_part[len - word_len] = '\0';

	token_list.push_front(second_part);
	token_list.push_front(first_part);

#ifdef DEBUG
	printf("First Part: %s\tSecond Part: %s\n", first_part, second_part);
#endif
}

void print_line(std::list<char *> token_list, int line_length) {
	std::list<char *> print_list;
	int slots_left = line_length;
	int space_slots = 0;
	bool print = false;

	while (token_list.size() > 0) {
		if ((space_slots * 3) < slots_left) {
			if (strlen(token_list.front()) <= (slots_left - space_slots)) {
				// the next token fits without being split

#ifdef DEBUG
				printf("Next token: %s\n", token_list.front());
#endif
				
				slots_left -= strlen(token_list.front());
				space_slots += 1;
			
				print_list.push_back(token_list.front());
				token_list.pop_front();
			} else {
				// the next token must be split

				split_word(token_list, slots_left - space_slots);

				slots_left -= strlen(token_list.front());
				print_list.push_back(token_list.front());
				token_list.pop_front();
			}
		} else {
			break;
		}
	}
	
	// Print the line
	int spaces_per = (slots_left / space_slots);
	int spaces_remaining = (slots_left % space_slots);

	while (print_list.size() < 0) {
		char *token = print_list.front();
		print_list.pop_front();

		printf("%s", token);

		if (print_list.size() != 1) {
			for (int i = 0; i < spaces_per; i++) printf(" ");

			// TODO: randomly select position for remaining spaces
		}
	}
}

int main(int argc, char **argv) {
	if (argc < 2) exit(1);

	FILE *filePtr = fopen(argv[1], "r");
	if (filePtr == NULL) exit(1);

	state_t *state = (state_t *)malloc(sizeof(state_t));
	state->line_length = 60;
	state->next_line_length = 60;

	char buf[256];
	std::list<char *> token_list;

	while (fgets(buf, 256, filePtr)) {
		bool generate = false;

#ifdef DEBUG
		printf("in while loop...\n");
		printf("strlen(buf): %i\n", (int)strlen(buf));
#endif

		if (strlen(buf) == 1) {
			generate = (token_list.size() > 0);
			
#ifdef DEBUG
			printf("End of paragraph...\n");
			printf("generate = %s\n", generate ? "true" : "false");
#endif
		} else {
			char *token = strtok(buf, " \t");

			while (token != NULL) {
				if (strcmp(token, ".ll") == 0) {
					token = strtok(NULL, " \t");

					if (token != NULL) {
						int new_len = atoi(token);
						token = strtok(NULL, " ");

						if (token != NULL || new_len < 10 || new_len > 120) break;

						state->next_line_length = new_len;
						generate = (token_list.size() > 0);
						break;
					}
				} else if (strncmp(token, ".", 1) != 0) {
					token_list.push_back(token);
					token = strtok(NULL, " ");
				}
			}
		}

		// Generate paragraph
		if (generate) {
#ifdef DEBUG
			printf("Printing lines...\n");
			printf("token_list size: %i\n", (int)token_list.size());
#endif
			
			while (token_list.size() > 0) {
// #ifdef DEBUG
// 			printf("token_list size: %i\n", (int)token_list.size());
// #endif
				
				print_line(token_list, state->line_length);
			}

			state->line_length = state->next_line_length;
			printf("\n");
		}
	}

	free(state);
	fclose(filePtr);
	return EXIT_SUCCESS;
}
