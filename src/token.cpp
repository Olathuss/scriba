#include "scriba/token.h"

namespace scriba {
	const char* token_type_to_string(TokenType type) {
		switch (type) {
		case TokenType::ON: return "ON";
		case TokenType::TRIGGER: return "TRIGGER";
		case TokenType::IF: return "IF";
		case TokenType::ELSE: return "ELSE";
		case TokenType::TRUE: return "TRUE";
		case TokenType::FALSE: return "FALSE";
		case TokenType::IDENTIFIER: return "IDENTIFIER";
		case TokenType::STRING: return "STRING";
		case TokenType::NUMBER: return "NUMBER";
		case TokenType::ARRAY: return "ARRAY";
		case TokenType::ASSIGN: return "ASSIGN";
		case TokenType::PLUS: return "PLUS";
		case TokenType::MINUS: return "MINUS";
		case TokenType::STAR: return "STAR";
		case TokenType::SLASH: return "SLASH";
		case TokenType::DOT: return "DOT";
		case TokenType::NOT: return "NOT";
		case TokenType::EQUAL: return "EQUAL";
		case TokenType::NOT_EQUAL: return "NOT_EQUAL";
		case TokenType::LESS_THAN: return "LESS_THAN";
		case TokenType::GREATER_THAN: return "GREATER_THAN";
		case TokenType::LESS_EQUAL: return "LESS_EQUAL";
		case TokenType::GREATER_EQUAL: return "GREATER_EQUAL";
		case TokenType::NEWLINE: return "NEWLINE";
		case TokenType::COMMENT: return "COMMENT";
		case TokenType::COLON: return "COLON";
		case TokenType::BRACKET_OPEN: return "BRACKET_OPEN";
		case TokenType::BRACKET_CLOSE: return "BRACKET_CLOSE";
		case TokenType::PAREN_OPEN: return "PAREN_OPEN";
		case TokenType::PAREN_CLOSE: return "PAREN_CLOSE";
		case TokenType::COMMA: return "COMMA";
		case TokenType::INDENT: return "INDENT";
		case TokenType::END_OF_FILE: return "END_OF_FILE";
		case TokenType::ERROR: return "ERROR";
		default: return "UNKNOWN";
		}
	}
}