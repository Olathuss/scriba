#pragma once

#include <string_view>
#include <vector>
#include "scriba/token.h"
#include "scriba/token_type.h"

namespace scriba {
	class Scanner {
	public:
		Scanner(std::string_view in_source);

		std::vector<Token> scan_tokens();

	private:
		std::string_view source;
		std::vector<Token> tokens;

		size_t start = 0;
		size_t current = 0;
		int line = 0;
		int column = 0;

		bool at_line_start = true;

		void scan_token();
		char advance();
		char peek() const;
		char peek_next() const;
		bool match(char expected);
		bool is_at_end() const;

		void add_token(TokenType in_type);
		void add_token(TokenType in_type, std::string_view in_lexeme);
		void add_indent_token(IndentLevel in_indent);

		void identifier();
		void number();
		void string_literal();

		void handle_whitespace();
		void handle_indentation();
		void handle_newline();

		static bool is_alpha(char in_char);
		static bool is_digit(char in_char);
		static bool is_alphanumeric(char in_char);
	};
} // namespace scriba