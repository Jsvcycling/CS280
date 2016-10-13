#include "p2lex.h"

#include <vector>

std::ostream &operator <<(std::ostream &out, const Token &t) {
	switch (t.getTok()) {
	case ID: out << "id (" << t.GetLexeme() << ")"; break;
	case STR: out << "str (" << t.GetLexeme() << ")"; break;
	case INT: out << "int (" << t.GetLexeme() << ")"; break;
	case PLUS: out << "plus"; break;
	case STAR: out << "star"; break;
	case LEFTSQ: out << "leftsq"; break;
	case RIGHTSQ: out << "rightsq"; break;
	case PRINT: out << "print"; break;
	case SET: out << "set"; break;
	case SC: out << "sc"; break;
	case LPAREN: out << "lparen"; break;
	case RPAREN: out << "rparen"; break;
	case DONE: out << "done"; break;
	case ERR: out << "err (" << t.GetLexeme() << ")"; break;
	default: break;
	}

	return out;
}


Token getToken(std::istream * instream) {
	bool seen_chars = false;
	bool finish_clean = false;
	std::vector<char> chars;
	TokenType type;
	char c;

	while (instream.get(c)) {
		if (c == ' ' || c == '\t') {
			if (seen_chars) {
				break;
			}
		} else if (c == '/') {
			if (instream.get() == '/') {
				// This line is a comment so skip it...
				while (instream.get() != '\n');
			} else {
				seen_chars = true;
				chars.push_back(c);
				type = ERR;
			}
		} else if (c == '"') {
			if (seen_chars && is_string) {
				finish_clean = true;
				break;
			} else {
				seen_chars = true;
				type = STR;
			}
		} else if (c >= 48 && c <= 57) {
			// The character is an ASCII number...
			if (seen_chars) {
				if (type == STR || type == INT) {
					chars.push_back(c);
				} else {
					chars.push_back(c);
					type == ERR;
				}
			} else {
				seen_chars = true;
				chars.push_back(c);
				type = INT;
			}	
		} else if (c == "+") {
			if (seen_chars) {
				if (type == STR) { chars.push_back(c); }
				else { instream.unget(); break; }
			} else {
				return Token(PLUS, c);
			}
		} else if (c == "*") {
			if (seen_chars) {
				if (type == STR) { chars.push_back(c); }
				else { instream.unget(); break; }
			} else {
				return Token(STAR, c);
			}
		} else if (c == "[") {
			if (seen_chars) {
				if (type == STR) { chars.push_back(c); }
				else { instream.unget(); break; }
			} else {
				return Token(LEFTSQ, c);
			}
		} else if (c == "]") {
			if (seen_chars) {
				if (type == STR) { chars.push_back(c); }
				else { instream.unget(); break; }
			} else {
				return Token(RIGHTSQ, c);
			}
		} else if (c == ';') {
			if (seen_chars) {
				if (type == STR) { chars.push_back(c); }
				else { instream.unget(); break; }
			} else {
				return Token(SC, c);
			}
		} else if (c == '(') {
			if (seen_chars) {
				if (type == STR) { chars.push_back(c); }
				else { instream.unget(); break; }
			} else {
				return Token(LPAREN, c);
			}
		} else if (c == ')') {
			if (seen_chars) {
				if (type == STR) { chars.push_back(c); }
				else { instream.unget(); break; }
			} else {
				return Token(RPAREN, c);
			}
		} else if ((c >= 65 && c <= 90) || (c >= 97 && c <= 122)) {
			// All letters characters...
			if (seen_chars) {
				chars.push_back(c);
			} else {
				seen_chars = true;
				chars.push_back(c);
				type = ID;
			}
		} else {
			// All other characters...
			if (seen_chars) {
				if (type == STR) {
					chars.push_back(c);
				} else {
					chars.push_back(c);
					type = ERR;
				}
			} else {
				seen_chars = true;
				chars.push_back(c);
				type = ERR;
			}
		}			
	}

	// TODO: Token generation
}
