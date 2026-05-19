#include <iostream>
#include <vector>
#include <cassert>
#include <utility>
#include <string>

#include "scriba/token.h"
#include "scriba/token_type.h"
#include "scriba/scanner.h"
#include "scriba/errors/scanner_error.h"

using namespace scriba;

std::vector<std::string> failed_tests_scanner;

bool assert_token(const Token& token, const TokenType& type, const std::string_view lexeme) {
	if (token.get_type() != type || token.lexeme != lexeme) {
		std::cout << "Expected: " << token_type_to_string(type) << " \"" << lexeme << "\"" << std::endl;
		std::cout << "Actual: " << token_type_to_string(token.get_type()) << " \"" << token.lexeme << "\"" << std::endl;
		std::cout << token.to_string() << std::endl;
		return false;
	}
	return true;
}

void test(
	std::string test_name,
	std::string source,
	std::vector<std::pair<TokenType, std::string>> token_list) {
	std::cout << "Running test: " << test_name << std::endl;
	std::cout << "Source:\n" << source << std::endl;

	try {
		Scanner scanner = Scanner(source);
		std::vector<Token> tokens = scanner.scan_tokens();

		token_list.insert(token_list.begin(), { TokenType::INDENT, "" });
		token_list.push_back({ TokenType::END_OF_FILE, "\0" });

		if (tokens.size() != token_list.size()) {
			std::cout << "Token count mismatch, expected: " << token_list.size() << " actual: " << tokens.size() << std::endl;
			assert(false);
		}

		for (int i = 0; i < token_list.size(); ++i) {
			if (!assert_token(tokens[i], token_list[i].first, token_list[i].second)) {
				failed_tests_scanner.push_back(test_name);
				std::cout << "Test \"" << test_name << "\" failed." << std::endl;
				return;
			}
		}

		std::cout << "Test \"" << test_name << "\" complete." << std::endl;
	}
	catch (const ScannerError& e) {
		std::cout << "ScannerError in test \"" << test_name << "\": "
			<< e.what() << "\n";
		failed_tests_scanner.push_back(test_name);
	}
	catch (const std::exception& e) {
		std::cout << "Unhandled exception in test \"" << test_name << "\": "
			<< e.what() << "\n";
		failed_tests_scanner.push_back(test_name);
	}
}

void test_tokens() {
	std::cout << "Testing tokens..." << std::endl;

	test("Identifier", "x", {
		{TokenType::IDENTIFIER, "x"}
		});

	test("Identifier", "player", {
		{TokenType::IDENTIFIER, "player"}
		});

	test("Number", "5", {
		{TokenType::NUMBER, "5"},
		});

	test("Number", "3.14", {
		{TokenType::NUMBER, "3.14"}
		});

	test("String", "\"hello world\"", {
		{TokenType::STRING, "\"hello world\""}
		});

	std::cout << "Token testing passed." << std::endl;
}

void test_keywords() {
	std::cout << "Testing keywords..." << std::endl;

	test("Keyword", "on", {
		{TokenType::ON, "on"}
		});

	test("Keyword", "trigger", {
		{TokenType::TRIGGER, "trigger"}
		});

	test("Keyword", "if", {
		{TokenType::IF, "if"}
		});

	test("Keyword", "else", {
			{TokenType::ELSE, "else"}
		});

	test("Keyword", "true", {
		{TokenType::TRUE, "true"}
		});

	test("Keyword", "false", {
		{TokenType::FALSE, "false"}
		});

	std::cout << "Keyword testing passed." << std::endl;
}

void test_operators() {
	std::cout << "Testing operators..." << std::endl;

	test("Operator", "=", {
	{TokenType::ASSIGN, "="}
		});

	test("Operator", "+", {
		{TokenType::PLUS, "+"}
		});

	test("Operator", "-", {
		{TokenType::MINUS, "-"}
		});

	test("Operator", "*", {
		{TokenType::STAR, "*"}
		});

	test("Operator", "/", {
		{TokenType::SLASH, "/"}
		});

	test("Operator", ".", {
		{TokenType::DOT, "."}
		});

	test("Operator", "!", {
		{TokenType::NOT, "!"}
		});

	test("Operator", "(", {
		{TokenType::PAREN_OPEN, "("}
		});

	test("Operator", ")", {
		{TokenType::PAREN_CLOSE, ")"}
		});

	std::cout << "Operators testing passed." << std::endl;
}

void test_comparison_operators() {
	std::cout << "Testing comparison operators..." << std::endl;

	test("Comparison", "==", {
	{TokenType::EQUAL, "=="}
		});

	test("Comparison", "!=", {
		{TokenType::NOT_EQUAL, "!="}
		});

	test("Comparison", "<", {
		{TokenType::LESS_THAN, "<"}
		});

	test("Comparison", ">", {
		{TokenType::GREATER_THAN, ">"}
		});

	test("Comparison", "<=", {
		{TokenType::LESS_EQUAL, "<="}
		});

	test("Comparison", ">=", {
		{TokenType::GREATER_EQUAL, ">="}
		});

	std::cout << "Comparison Operators testing passed." << std::endl;
}

void test_misc() {
	std::cout << "Testing misc..." << std::endl;

	test("Misc", ":", {
	{TokenType::COLON, ":"}
		});

	test("Misc", "[", {
	{TokenType::BRACKET_OPEN, "["}
		});

	test("Misc", "]", {
		{TokenType::BRACKET_CLOSE, "]"}
		});

	test("Misc", ",", {
	{TokenType::COMMA, ","}
		});

	test("Comment", "# this is a comment", {
		// no tokens before EOF
		});

	test("Newline", "\n", {
	{TokenType::NEWLINE, ""}
		});

	test("Indent (space)", "    x", {
	{TokenType::IDENTIFIER, "x"}
		});

	test("Indent (tab)", "\tx", {
	{TokenType::IDENTIFIER, "x"}
		});


	std::cout << "Misc testing passed." << std::endl;
}

void test_sequences() {
	std::cout << "Testing sequences..." << std::endl;

	test("Sequence: assignment", "x = 5", {
	{TokenType::IDENTIFIER, "x"},
	{TokenType::ASSIGN, "="},
	{TokenType::NUMBER, "5"}
		});

	test("Sequence: arithmetic", "a + b - c", {
	{TokenType::IDENTIFIER, "a"},
	{TokenType::PLUS, "+"},
	{TokenType::IDENTIFIER, "b"},
	{TokenType::MINUS, "-"},
	{TokenType::IDENTIFIER, "c"}
		});

	test("Sequence: parentheses", "3 * (4 + 1)", {
	{TokenType::NUMBER, "3"},
	{TokenType::STAR, "*"},
	{TokenType::PAREN_OPEN, "("},
	{TokenType::NUMBER, "4"},
	{TokenType::PLUS, "+"},
	{TokenType::NUMBER, "1"},
	{TokenType::PAREN_CLOSE, ")"}
		});

	test("Sequence: member access", "player.x", {
	{TokenType::IDENTIFIER, "player"},
	{TokenType::DOT, "."},
	{TokenType::IDENTIFIER, "x"}
		});

	test("Sequence: array literal", "[1, 2, 3]", {
	{TokenType::BRACKET_OPEN, "["},
	{TokenType::NUMBER, "1"},
	{TokenType::COMMA, ","},
	{TokenType::NUMBER, "2"},
	{TokenType::COMMA, ","},
	{TokenType::NUMBER, "3"},
	{TokenType::BRACKET_CLOSE, "]"}
		});

	test("Sequence: comparison", "if x == 5", {
	{TokenType::IF, "if"},
	{TokenType::IDENTIFIER, "x"},
	{TokenType::EQUAL, "=="},
	{TokenType::NUMBER, "5"}
		});

	test("Sequence: mixed operators", "a*2+3/4-5", {
	{TokenType::IDENTIFIER, "a"},
	{TokenType::STAR, "*"},
	{TokenType::NUMBER, "2"},
	{TokenType::PLUS, "+"},
	{TokenType::NUMBER, "3"},
	{TokenType::SLASH, "/"},
	{TokenType::NUMBER, "4"},
	{TokenType::MINUS, "-"},
	{TokenType::NUMBER, "5"}
		});

	test("Sequence: call-like syntax", "move(player, 3)", {
	{TokenType::IDENTIFIER, "move"},
	{TokenType::PAREN_OPEN, "("},
	{TokenType::IDENTIFIER, "player"},
	{TokenType::COMMA, ","},
	{TokenType::NUMBER, "3"},
	{TokenType::PAREN_CLOSE, ")"}
		});

	test("Sequence: command", "move player 3", {
	{TokenType::IDENTIFIER, "move"},
	{TokenType::IDENTIFIER, "player"},
	{TokenType::NUMBER, "3"}
		});

	test("Sequence: command with expression", "player.add_health player.health + 33", {
	{TokenType::IDENTIFIER, "player"},
	{TokenType::DOT, "."},
	{TokenType::IDENTIFIER, "add_health"},
	{TokenType::IDENTIFIER, "player"},
	{TokenType::DOT, "."},
	{TokenType::IDENTIFIER, "health"},
	{TokenType::PLUS, "+"},
	{TokenType::NUMBER, "33"}
		});

	test("Sequence: command with parentheses", "move player (22 + 3)", {
	{TokenType::IDENTIFIER, "move"},
	{TokenType::IDENTIFIER, "player"},
	{TokenType::PAREN_OPEN, "("},
	{TokenType::NUMBER, "22"},
	{TokenType::PLUS, "+"},
	{TokenType::NUMBER, "3"},
	{ TokenType::PAREN_CLOSE, ")" },
		});

	test("Sequence: string concat", "\"hello\" + name", {
	{TokenType::STRING, "\"hello\""},
	{TokenType::PLUS, "+"},
	{TokenType::IDENTIFIER, "name"}
		});

	test("Sequence: mixed whitespace", "  x\t+\t  5", {
	{TokenType::IDENTIFIER, "x"},
	{TokenType::PLUS, "+"},
	{TokenType::NUMBER, "5"}
		});

	std::cout << "Sequences testing passed." << std::endl;
}

void test_edge_cases() {
	std::cout << "Testing edge cases..." << std::endl;

	test("Edge: range-like", "1..10", {
	{TokenType::NUMBER, "1"},
	{TokenType::DOT, "."},
	{TokenType::DOT, "."},
	{TokenType::NUMBER, "10"}
		});

	test("Edge: ellipsis", "...", {
	{TokenType::DOT, "."},
	{TokenType::DOT, "."},
	{TokenType::DOT, "."}
		});

	test("Edge: decimal vs dot operator", "3.<5", {
	{TokenType::NUMBER, "3"},
	{TokenType::DOT, "."},
	{TokenType::LESS_THAN, "<"},
	{TokenType::NUMBER, "5"}
		});

	test("Edge: spaced decimal", "3. .5", {
	{TokenType::NUMBER, "3"},
	{TokenType::DOT, "."},
	{TokenType::DOT, "."},
	{TokenType::NUMBER, "5"}
		});

	test("Edge: chained decimals", "3.14.15", {
	{TokenType::NUMBER, "3.14"},
	{TokenType::DOT, "."},
	{TokenType::NUMBER, "15"}
		});

	test("Edge: minus collision", "x--y", {
	{TokenType::IDENTIFIER, "x"},
	{TokenType::MINUS, "-"},
	{TokenType::MINUS, "-"},
	{TokenType::IDENTIFIER, "y"}
		});

	test("Edge: unary minus", "x+-5", {
	{TokenType::IDENTIFIER, "x"},
	{TokenType::PLUS, "+"},
	{TokenType::MINUS, "-"},
	{TokenType::NUMBER, "5"}
		});
	
	test("Edge: double negative", "x - -5", {
	{TokenType::IDENTIFIER, "x"},
	{TokenType::MINUS, "-"},
	{TokenType::MINUS, "-"},
	{TokenType::NUMBER, "5"}
		});

	test("Edge: triple minus", "x---y", {
	{TokenType::IDENTIFIER, "x"},
	{TokenType::MINUS, "-"},
	{TokenType::MINUS, "-"},
	{TokenType::MINUS, "-"},
	{TokenType::IDENTIFIER, "y"}
		});

	test("Edge: double dot member", "player..health", {
	{TokenType::IDENTIFIER, "player"},
	{TokenType::DOT, "."},
	{TokenType::DOT, "."},
	{TokenType::IDENTIFIER, "health"}
		});

	test("Edge: trailing dot", "3.", {
	{TokenType::NUMBER, "3"},
	{TokenType::DOT, "."}
		});

	test("Edge: leading dot", ".5", {
	{TokenType::DOT, "."},
	{TokenType::NUMBER, "5"}
		});

	test("Edge: dot identifier", ".player", {
	{TokenType::DOT, "."},
	{TokenType::IDENTIFIER, "player"}
		});

	test("Edge: dot keyword", ".if", {
	{TokenType::DOT, "."},
	{TokenType::IF, "if"}
		});

	test("Edge: dot number", ".123", {
	{TokenType::DOT, "."},
	{TokenType::NUMBER, "123"}
		});

	test("Edge: identifier dot number", "A.1", {
	{TokenType::IDENTIFIER, "A"},
	{TokenType::DOT, "."},
	{TokenType::NUMBER, "1"}
		});

	std::cout << "Edge case testing passed." << std::endl;
}

void run_scanner_tests() {
	std::cout << "Running scanner tests..." << std::endl;

	test_tokens();
	test_keywords();
	test_operators();
	test_comparison_operators();
	test_misc();
	test_sequences();
	test_edge_cases();

	if (failed_tests_scanner.size() > 0) {
		std::cout << "The following tests failed: " << std::endl;
		for (auto& test : failed_tests_scanner) {
			std::cout << test << std::endl;
		}
		std::cout << "Scanner tests did not pass." << std::endl;
	} else {
		std::cout << "Scanner tests passed." << std::endl;
	}
	
	std::cout << "Scanner tests completed." << std::endl;
}