#include "p2lex.h"

#include <vector>
#include <string>
#include <stdio.h>

int linenum = 0;

std::ostream &operator <<(std::ostream &out, const Token &t) {
	switch (t.getTok()) {
	case ID: out << "id(" << t.getLexeme().c_str() << ")"; break;
	case STR: out << "str(" << t.getLexeme().c_str() << ")"; break;
	case INT: out << "int(" << t.getLexeme().c_str() << ")"; break;
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
	case ERR: out << "err(" << t.getLexeme().c_str() << ")"; break;
	default: break;
	}

	return out;
}

Token getToken(std::istream *instream) {
	bool seen_chars = false;
	bool finish_clean = false;
	std::string str;
	TokenType type = DONE;
	char c;

	while (instream->get(c)) {
		if (c == ' ' || c == '\t') {
			if (seen_chars) {
				if (type != STR) {
					break;
				} else {
					str += c;
				}
			}
		} else if (c == '\n') {
			linenum += 1;
			
			if (seen_chars) {
				break;
			}
		} else if (c == '/') {
			if (instream->get() == '/') {
				// This line is a comment so skip it...
				while (instream->get() != '\n');
			} else {
				seen_chars = true;
				str += c;
				type = ERR;
			}
		} else if (c == '"') {
			if (seen_chars) {
				if (type == STR) {
					finish_clean = true;
					str += c;
					break;
				} else if (type != DONE) {
					type = ERR;
				}
			} else {
				seen_chars = true;
				str += c;
				type = STR;
			}
		} else if (c >= 48 && c <= 57) {
			// The character is an ASCII number...
			if (seen_chars) {
				if (type == STR || type == INT) {
					str += c;
				} else {
					str += c;
					type == ERR;
				}
			} else {
				seen_chars = true;
				str += c;
				type = INT;
			}	
		} else if (c == '+') {
			if (seen_chars) {
				if (type == STR) { str += c; }
				else { instream->unget(); break; }
			} else {
				str += c;
				return Token(PLUS, str);
			}
		} else if (c == '*') {
			if (seen_chars) {
				if (type == STR) { str += c; }
				else { instream->unget(); break; }
			} else {
				str += c;
				return Token(STAR, str);
			}
		} else if (c == '[') {
			if (seen_chars) {
				if (type == STR) { str += c; }
				else { instream->unget(); break; }
			} else {
				str += c;
				return Token(LEFTSQ, str);
			}
		} else if (c == ']') {
			if (seen_chars) {
				if (type == STR) { str += c; }
				else { instream->unget(); break; }
			} else {
				str += c;
				return Token(RIGHTSQ, str);
			}
		} else if (c == ';') {
			if (seen_chars) {
				if (type == STR) { str += c; }
				else { instream->unget(); break; }
			} else {
				str += c;
				return Token(SC, str);
			}
		} else if (c == '(') {
			if (seen_chars) {
				if (type == STR) { str += c; }
				else { instream->unget(); break; }
			} else {
				str += c;
				return Token(LPAREN, str);
			}
		} else if (c == ')') {
			if (seen_chars) {
				if (type == STR) { str += c; }
				else { instream->unget(); break; }
			} else {
				str += c;
				return Token(RPAREN, str);
			}
		} else if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) {
			// All letters characters...
			if (seen_chars && type == STR) {
				str += c;
			} else {
				seen_chars = true;
				str += c;
				type = ID;
			}
		} else {
			// All other characters...
			if (seen_chars) {
				if (type == STR) {
					str += c;
				} else {
					str += c;
					type = ERR;
				}
			} else {
				seen_chars = true;
				str += c;
				type = ERR;
			}
		}	
	}

	if (seen_chars) {
		if (type == STR) {
			if (finish_clean) {
				return Token(type, str);
			} else {
				return Token(ERR, str);
			}
		} else if (type == ID && str == "print") {
			return Token(PRINT, str);
		} else if (type == ID && str == "set") {
			return Token(SET, str);
		} else {
			return Token(type, str);
		}
	}

	return Token(DONE, "");
}
