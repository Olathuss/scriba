#include <iostream>
#include <vector>
#include <cassert>
#include <utility>

#include "scriba/token.h"
#include "scriba/token_type.h"
#include "scriba/scanner.h"

using namespace std;
using namespace scriba;

void assert_token(const Token& token, const TokenType& type, const string_view lexeme) {
	if (token.get_type() != type || token.lexeme != lexeme) {
		cout << "Expected: " << token_type_to_string(type) << " \"" << lexeme << "\"" << endl;
		cout << "Actual: " << token_type_to_string(token.get_type()) << " \"" << token.lexeme << "\"" << endl;
		cout << token.to_string() << endl;
		assert(false);
	}
}

void test(
	string test_name,
	string source,
	vector<pair<TokenType, string>> token_list) {
	cout << "Running test: " << test_name << endl;
	cout << "Source:\n" << source << endl;

	Scanner scanner = Scanner(source);
	vector<Token> tokens = scanner.scan_tokens();

	token_list.insert(token_list.begin(), { TokenType::INDENT, "" });
	token_list.push_back({ TokenType::END_OF_FILE, "\0" });

	if (tokens.size() != token_list.size()) {
		cout << "Token count mismatch, expected: " << token_list.size() << " actual: " << tokens.size() << endl;
		assert(false);
	}

	for (int i = 0; i < token_list.size(); ++i) {
		assert_token(tokens[i], token_list[i].first, token_list[i].second);
	}

	cout << "Test \"" << test_name << "\" complete." << endl;
}

void test_tokens() {
	cout << "Testing tokens..." << endl;

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

	cout << "Token testing passed." << endl;
}

void test_keywords() {
	cout << "Testing keywords..." << endl;

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

	cout << "Keyword testing passed." << endl;
}

void test_operators() {
	cout << "Testing operators..." << endl;

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

	cout << "Operators testing passed." << endl;
}

void test_comparison_operators() {
	cout << "Testing comparison operators..." << endl;

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

	cout << "Comparison Operators testing passed." << endl;
}

void test_misc() {
	cout << "Testing misc..." << endl;

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


	cout << "Misc testing passed." << endl;
}

void test_sequences() {
	cout << "Testing sequences..." << endl;

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

	cout << "Sequences testing passed." << endl;
}

void test_edge_cases() {
	cout << "Testing edge cases..." << endl;

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

	cout << "Edge case testing passed." << endl;
}

void run_scanner_tests() {
	cout << "Running scanner tests..." << endl;

	test_tokens();
	test_keywords();
	test_operators();
	test_comparison_operators();
	test_misc();
	test_sequences();
	test_edge_cases();
	
	cout << "Scanner tests completed." << endl;
}