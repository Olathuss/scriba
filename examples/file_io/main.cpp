#include <fstream>
#include <sstream>
#include <iostream>
#include "scriba/scanner.h"
#include "scriba/parser.h"

int main() {
	std::ifstream file("test.scriba");
	std::stringstream buffer;
	buffer << file.rdbuf();
	
	std::string source = buffer.str();
	
	scriba::Scanner scanner(source);
	auto tokens = scanner.scan_tokens();
	
	for (auto& t : tokens) {
		std::cout << token_type_to_string(t.type)
				  << "  '" << t.lexeme << "'\n";
	}
}