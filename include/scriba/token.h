#pragma once

#include <string>
#include <variant>
#include <vector>
#include <scriba/token_type.h>

namespace scriba {
	struct Token;

	struct IndentLevel {
		int indent_level = 0;
		int indent_width = 0;
		char indent_char = '\0';
	};

	using LiteralValue = std::variant<
		std::monostate,
		double,
		bool,
		std::string
	>;

	const char* token_type_to_string(TokenType type);

	struct Token {
		TokenType type;
		std::string lexeme;
		LiteralValue literal;
		int line = 0;
		int column = 0;
		IndentLevel indent;

		Token() : type(TokenType::ERROR) {}

		Token(TokenType in_token, std::string in_lexeme, LiteralValue in_literal, int in_line = 0, int in_column = 0)
			: type(in_token), lexeme(std::move(in_lexeme)), literal(std::move(in_literal)), line(in_line), column(in_column) {}

		std::string to_string() const {
			return "Token(" +
				std::string(token_type_to_string(type)) + ", \"" +
				lexeme + "\", line=" +
				std::to_string(line) + ", col=" +
				std::to_string(column) + ")";
		}

		TokenType get_type() const {
			return type;
		}

		void set_line(int in_line) {
			line = in_line;
		}
		int get_line() const {
			return line;
		}

		int get_column() const {
			return column;
		}

		bool is_unary() const {
			switch (type) {
			case TokenType::NOT:
			case TokenType::PLUS:
			case TokenType::MINUS:
				return true;
			default:
				return false;
			}
		}

		bool is_comparison() const {
			switch (type) {
			case TokenType::EQUAL:
			case TokenType::LESS_EQUAL:
			case TokenType::LESS_THAN:
			case TokenType::GREATER_EQUAL:
			case TokenType::GREATER_THAN:
			case TokenType::NOT_EQUAL:
				return true;
			default:
				return false;
			}
		}
	};
} // namespace scriba