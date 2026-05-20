#pragma once

namespace scriba {
	enum class TokenType {
		// Keywords
		ON,
		TRIGGER,
		IF,
		ELSE,
		AND,
		OR,
		TRUE,
		FALSE,

		// IDENTIFIERS & VALUES
		IDENTIFIER,
		STRING,
		NUMBER,
		ARRAY,

		// Operators
		ASSIGN,			// "=" Assignment
		PLUS,			// "+"
		MINUS,			// "-"
		STAR,			// "*"
		SLASH,			// "/"
		DOT,			// "."
		NOT,			// "!"
		PAREN_OPEN,		// "("
		PAREN_CLOSE,	// ")"

		// Comparison
		EQUAL,			// "==" Equals
		NOT_EQUAL,		// "!=" Not Equal
		LESS_THAN,		// "<" Less Than
		GREATER_THAN,	// ">" Greater Than
		LESS_EQUAL,		// "<=" Less Than or Equal
		GREATER_EQUAL,	// ">=" Greater Than or Equal

		// Misc
		NEWLINE,
		COMMENT,
		COLON,
		BRACKET_OPEN,
		BRACKET_CLOSE,
		COMMA,
		INDENT,
		END_OF_FILE,

		// Error
		ERROR
	};
} // namespace scriba