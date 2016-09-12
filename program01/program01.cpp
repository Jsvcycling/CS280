//-------------------------------------------------
// Name: Program01.cpp
// Author: Joshua Vega
// Version: N/A
// Description: CS280 Programming Assignment #1
//-------------------------------------------------

/*
 * DESIGN DESCRIPTION
 *
 * TODO...
 */

#include <stdio.h>
#include <string.h>
#include <list>

void print_line(std::list<char *> output_list, int line_length) {
	std::list<char *> print_list;
	int print_length = 0;
	int spaces_to_add = 0;
	int min_spaces_per = 0;
	int remaining_spaces = 0;

    while(output_list.size() > 0) {
		int next_length = strlen(output_list.front());

		if (print_length + next_length < line_length) {
			// We still have space left in the line, just add the next word in
			// the queue (including the trailing space character).
			print_list.push_back(output_list.pop_back());
			print_list.push_back(" ");
			print_length += (next_length + 1);
		} else if (print_length + next_length > line_length) {
			// The next token would overflow the line length.

			if (print_list.size() == 0) {
				// If not even the first token of the paragrpah fits the
				// line_length parameters, split the token using a "-" symbol at
				// the end of the end of the first part, and put the second part
				// on the front of the output_list.
				int chars_left = line_length - print_length;

				char *last_word = output_list.pop_front();
				
				char *first_part[chars_left + 1];
				strncpy(first_part, last_word, chars_left - 2);
				first_part[chars_left - 2] = '-';  // Add dash.
				first_part[chars_left - 1] = '\0'; // Make sure to make this a
												   // null terminated string.

				print_list.push_back(first_part);

				// TODO: re-add the second part of the token to the front of the
				// list so that it is printed in the next line...
				break;
			}

			// Calculate the number of spaces needed and the number of spaces to
			// place in each slot.
			spaces_to_add = line_length - print_length;
			min_spaces_per = spaces_to_add / (print_list.size() / 2);
			remaining_spaces = spaces_to_add % (print_list.size() / 2);
			break;
		} else {
			// The print_sum is equal to the requested line length. It's a
			// perfectly sized line so just go straight to print.
			break;
		}
	}
	
	// Print the line.
	while (print_list.size() > 0) {
		char *token = print_list.pop_front();

		if (strcmp(token, ' ') == 0) {
			// The token is a space, use our spacing values to figure out the
			// correct number of spaces to insert here.

			// TODO...
			continue;
		}

		// Print out the token.
		printf("%s", token);
	}

	// Add a newline at the end.
	printf("\n");
}

int main(int argc, char **argv) {
	if (argc < 2) {
		// TODO: handle error...
	}
	
	FILE *filePtr = fopen(argv[1], "r");

	if (filePtr == NULL) {
		// TODO: handle error...
	}

	// Per the specification, the initial line length is 60.
	int line_length = 60;
	int next_line_length = 60;
	char buf[256];
	std::list<char *> output_list;

	while(fgets(buf, 256, filePtr)) {
		bool generate = false;
		
		if (strlen(buf) == 0) {
			// Check if the line is empty. If so, stop reading and either
			// generate the resulting paragraph or skip to the next interation.
			generate = (output_list.size() > 0);
		} else {
			// Tokenize the current string.
			char *token = strtok(buf, " ");

			// First, parse the current line.
			while (token != NULL) {
				if (strcmp(token, ".ll") == 0) {
					// Check if the line begins with the special line length
					// command.
					token = strtok(NULL, " ");

					if (token != NULL) {
						int new_length = atoi(token);
						token = strtok(NULL, " ");

						if (token != NULL) {
							// There is content after the length parameter, skip
							// the command entirely.
							break;
						}
						
						if (new_length < 10 || new_length > 120) {
							// The new line length is invalid, skip the command
							// entirely.
							break;
						}

						// Finally, set the next line length. Also, if the
						// output queue has content, treat the .ll command as a
						// end of paragraph.
						next_line_length = new_length;
						generate = (output_list.size() > 0);
						break;	// Should we break here? Why didn't I write this before??
					}
				} else if (strncmp(token, ".", 1) != 0) {
					// Check if the line does not begin with a dot (lines
					// beginning with dots are reserved for commands and the
					// only command supported is .ll which is already handled
					// above). If not, add the token to the end of the paragraph
					// token queue.
					output_list.push_back(token);
					token = strtok(NULL, " ");
				}
			}
		}

		if (generate) {
			// Generate the output paragraph.
			while (output_list.size() > 0) {
				print_line(output_list, line_length);
			}

			// Change the line length to the new line length.
			line_length = next_line_length;

			// Add a blank line between paragraphs.
			printf("\n");
		}
	}

	fclose(filePtr);
	return 0;
}
