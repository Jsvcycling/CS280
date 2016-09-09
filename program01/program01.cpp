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

    while(output_queue.size() > 0) {
		int next_length = strlen(output_list.front());

		if (print_length + next_length < line_length) {
			// We still have space left in the line, just add the next word in
			// the queue (including the trailing space character).
			print_list.push_back(output_list.pop_back());
			print_list.push_back(" ");
			print_length += (next_length + 1);
		} else if (print_length + next_length > line_length) {
			// The line length limit has been reached.

			if (print_list.size() == 0) {
				// If not even the next token fits the line_length parameters,
				// split the token using a "-" symbol at the end of the end of
				// the first part, and put the second part on the front of the
				// output_list.
			} else {
				// TODO...
			}
		}
	}
	
	// TODO: print the line...
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
						// output queue has contents, treat the .ll command as a
						// end of paragraph.
						next_line_length = new_length;
						generate = (output_list.size() > 0);
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
		}
	}

	fclose(filePtr);
	return 0;
}
