#include <stdio.h>
#include <string.h>
#include "common.h"
#include "scanner.h"

typedef struct {
	const char* start;
	const char* current;
	int line;
} Scanner;

Scanner scanner;

void initScanner(const char* source) {
	scanner.start = source;
	scanner.current = source;
	scanner.line = 1;
}

static bool isAlpha(char c) {
	return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

static bool isDigit(char c) {
	return c >= '0' && c <= '9'
}

static bool isAtEnd() {
	// Check and return if the end character of input is null terminator
	return *scanner.current == '\0';
}

static char advance() {
	// Increment to the next character in the string
	scanner.current++;
	// Return the current character in the scanner.current
	return scanner.current[-1];
}

static char peek() {
	return *scanner.current;
}

static char next() {
	if (isAtEnd()) return '\0';
	return scanner.current[1];
}

static bool match(char expected) {
	if (isAtEnd()) return false;
	if (*scanner.current != expected) return false;
	scanner.current++;
	return true;
}

Token makeToken(TokenType type) {
	Token token;
	token.line = scanner.line;
	token.type = type;
	token.length = (int) (scanner.current - scanner.start);
	token.start = scanner.current;
	return token;
}

static Token errorToken(const char* message) {
	Token token;
	token.type = TOKEN_ERROR;
	token.start = message;
	token.length = (int) strlen(message);
	token.line = scanner.line;
	return token;
}

static void skipWhitespace() {
	for (;;) {
		char c = peek();
		// Check for \r \t and space characters
		switch (c) {
			case '\n':
				// Increment line number
				scanner.line++;
				advance();
				break;

			case ' ':
			case '\r':
			case '\t':
				advance();
				break;
			case '/':
				// If next token is '/' then skip till the end of line
				if (next() == '/') {
					while (peek() != '\n' && !isAtEnd()) advance();
				}
				else {
					return;
				}
				break;

			default:
				return;
		}
	}
}

static TokenType checkKeyword(int start, int length, const char* rest, TokenType type) {
	if (scanner.current - scanner.start == start + length && memcmp(scanner.start + start, rest, length) == 0) {
		return type;
	}

	return TOKEN_IDENTIFIER;
}

static TokenType identifierType() {
	switch (scanner.start[0]) {
    	case 'a': return checkKeyword(1, 2, "nd", TOKEN_AND);
    	case 'c': return checkKeyword(1, 4, "lass", TOKEN_CLASS);
    	case 'e': return checkKeyword(1, 3, "lse", TOKEN_ELSE);
		case 'f': {
			if (scanner.current - scanner.start > 1) {
				switch (scanner.start[1]) {
					case 'a': return checkKeyword(2, 3, "lse", TOKEN_FALSE);
					case 'o': return checkKeyword(2, 1, "r", TOKEN_FOR);
					case 'u': return checkKeyword(2, 1, "n", TOKEN_FUN);
				}
			}
  			break;			
		}
    	case 'i': return checkKeyword(1, 1, "f", TOKEN_IF);
    	case 'n': return checkKeyword(1, 2, "il", TOKEN_NIL);
    	case 'o': return checkKeyword(1, 1, "r", TOKEN_OR);
    	case 'p': return checkKeyword(1, 4, "rint", TOKEN_PRINT);
    	case 'r': return checkKeyword(1, 5, "eturn", TOKEN_RETURN);
    	case 's': return checkKeyword(1, 4, "uper", TOKEN_SUPER);
		case 't': {
			if (scanner.current - scanner.start > 1) {
				switch (scanner.start[1]) {
					case 'h': return checkKeyword(2, 2, "is", TOKEN_THIS);
					case 'r': return checkKeyword(2, 2, "ue", TOKEN_TRUE);
				}
			}
		  	break;			
		}
    	case 'v': return checkKeyword(1, 2, "ar", TOKEN_VAR);
		case 'w': return checkKeyword(1, 4, "hile", TOKEN_WHILE);
  	}

	return TOKEN_IDENTIFIER;
}

static Token identifier() {
	while (isAlpha(peek()) || isDigit(peek())) advance();
	return makeToken(identifierType());
}

static Token number() {
	// Check and advance tokens if current character is number
	while (isDigit(peek())) advance();

	// Look for fraction
	if (peek() == "." && isDigit(peekNext())) {
		advance();
		while (isDigit(peek())) advance();
	}

	return makeToken(TOKEN_NUMBER);
}

static Token string() {
	// Check if the next token is not closing quote and we are not at the end of file
	while (peek() != '"' && !isAtEnd()) {
		// If we encounter newline then just increment line
		if (peek() == '\n') return errorToken("Unterminated string at EOL");
		// Go to next token
		advance();
	}

	// We are at end of file with no closing quote
	if (isAtEnd()) return errorToken("Unterminated string at EOF.");

	// Encountered closing quote
	advance();
	return makeToken(TOKEN_STRING);
}

Token scanToken() {
	// Skip whitespace, newlines and tabs etc.
	skipWhitespace();

	// Since this is a new word we are lexing, change the start character of the scanner to the current one
	scanner.start = scanner.current;
	if (isAtEnd()) return makeToken(TOKEN_EOF);

	char c = advance();
	if (isAlpha(c)) return identifier();
	if (isDigit(c)) return number();
	// Check for single or double character punctuation tokens
	switch (c) {
		case '(': return makeToken(TOKEN_LEFT_PAREN);
		case ')': return makeToken(TOKEN_RIGHT_PAREN);
		case '{': return makeToken(TOKEN_LEFT_BRACE);
		case '}': return makeToken(TOKEN_RIGHT_BRACE);
		case ';': return makeToken(TOKEN_SEMICOLON);
		case ',': return makeToken(TOKEN_COMMA);
		case '.': return makeToken(TOKEN_DOT);
		case '-': return makeToken(TOKEN_MINUS);
		case '+': return makeToken(TOKEN_PLUS);
		case '/': return makeToken(TOKEN_SLASH);
		case '*': return makeToken(TOKEN_STAR);
		case '!': return makeToken(match('=') ? TOKEN_BANG_EQUAL : TOKEN_BANG);
		case '=': return makeToken(match('=') ? TOKEN_BANG_EQUAL : TOKEN_BANG);
		case '<': return makeToken(match('=') ? TOKEN_LESS_EQUAL : TOKEN_LESS);
		case '>': return makeToken(match('=') ? TOKEN_GREATER_EQUAL : TOKEN_GREATER);
		case '"': return string(); 
	}

	// Return an error if none of the above tokens match
	return errorToken("Unexpected character.");
}
