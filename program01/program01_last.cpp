#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <list>

void split_word(std::list<std::string> *tokens, int word_len) {
	std::string word = tokens->front();

	int first_part_len = word_len - 1;
	int second_part_len = word.size() - word_len;

	std::string first_part = word.substr(0, first_part_len);
	std::string second_part = word.substr(first_part_len, second_part_len);

	first_part += '-';

	tokens->pop_front();
	tokens->push_front(second_part);
	tokens->push_front(first_part);
}

void print_line(std::list<std::string> *tokens, int line_len) {
	std::list<std::string> print_list;
	int slots_left = line_len;
	int space_slots = 0;

	for (std::list<std::string>::iterator it = tokens->begin(); it != tokens->end(); ++it) {
		if (it->size() == line_len) {
			// The current token fits the line perfectly...
			std::cout << *it << std::endl;
			tokens->erase(it);
			return;
		} else if (it->size() == (slots_left - space_slots)) {
			// The current token fits perfectly in the remaining slots...
			slots_left = 0;
			print_list.push_back(*it);
			tokens->erase(it);
			break;
		} else if (it->size() < (slots_left - space_slots)) {
			// The current token doesn't take up all the remaining slots...
			slots_left -= it->size();
			print_list.push_back(*it);
			tokens->erase(it);

			space_slots += 1;
		} else if ((space_slots * 3) < (slots_left - 1)) {
			split_word(tokens, (slots_left - space_slots));
		} else {
			break;
		}
	}

	int spaces_per_slot = 0;
	int remaining_spaces = 0;

	// TODO: calculate space distribution
	

	for (std::list<std::string>::iterator it = print_list.begin(); it != print_list.end(); ++it) {
		std::cout << *it;

		for (int i = 0; i < spaces_per_slot; i++) {
			std::cout << " ";
		}

		if (remaining_spaces > 0) {
			std::cout << " ";
			remaining_spaces -= 1;
		}
	}

	std::cout << std::endl;
}

int main(int argc, char **argv) {
	if (argc < 2) exit(1);

	std::ifstream  file(argv[1], std::ifstream::in);

	std::list<std::string> tokens;
	std::string line;

	int line_len = 60;
	int next_line_len = line_len;
	
	while (std::getline(file, line)) {
		bool generate = false;
		
		if (file.gcount() == 1) {
			generate = (tokens.size() > 0);
			break;
		}

		if (!generate) {
			std::stringstream linestrm(line);
			std::string token;

			while (linestrm >> token) {
				if (token[0] == '.') {
					if (token.compare(".ll")) {
						linestrm >> token;

						if (linestrm.str().size() == token.size() + 5) {
							int new_len = std::stoi(token);

							if (new_len >= 10 && new_len <= 120) {
								next_line_len = new_len;
								generate = (tokens.size() > 0);
							}
						}
					}

					break;
				} else {
					tokens.push_back(token);
				}
			}
		}

		if (generate) {
			while (tokens.size() > 0) {
				print_line(&tokens, line_len);
			}

			std::cout << std::endl;
		}

		line_len = next_line_len;
	}

	file.close();
	return 0;
}
