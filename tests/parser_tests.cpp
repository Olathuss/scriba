#include <iostream>
#include <algorithm>

#include "ast_printer.h"
#include "scriba/scanner.h"
#include "scriba/parser.h"
#include "scriba/storagemanager.h"

using namespace std;
using namespace scriba;

void expect_ast(const std::string& test_name, const std::string& source, const std::string& expected) {
    cout << "Running test: " << test_name << endl;
    cout << "Source:\n" << source << endl;

    Scanner scanner(source);
    auto tokens = scanner.scan_tokens();

    std::unique_ptr<StorageManager> storage = std::make_unique<StorageManager>();

    for (auto token : tokens) {
        storage->add_token(token);
    }

    Parser parser = Parser();
    parser.set_storage_manager(storage.get());

    parser.parse_script();

    const auto& events = parser.get_events();
    vector<string> keys;

    keys.reserve(events.size());
    for (auto& kv : events) keys.push_back(kv.first);

    sort(keys.begin(), keys.end());

    std::string actual;
    for (auto& key : keys) {
        actual += print(events.at(key)) + "\n";
    }

    if (actual != expected) {
        std::cerr << "AST mismatch!\n";
        std::cerr << "Source: " << source << "\n";
        std::cerr << "Expected: " << expected << "\n";
        std::cerr << "Actual:   " << actual << "\n";
        std::abort();
    }
}

void test_atomic() {
    expect_ast("test event, command, ident",
        "on test:\n    x\n",
        "(Event test:\n"
        "\t(Command (Ident x))\n"
        ")\n"
    );
}

void run_parser_tests() {
    cout << "Running parser tests..." << endl;

    test_atomic();

	cout << "Parser test completed." << endl;
}