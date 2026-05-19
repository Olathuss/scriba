#include <iostream>
#include <algorithm>
#include <cassert>

#include "ast_printer.h"
#include "scriba/scanner.h"
#include "scriba/parser.h"
#include "scriba/storagemanager.h"
#include "scriba/errors/parse_error.h"
#include "scriba/errors/scanner_error.h"

using namespace scriba;

int parser_tests_total = 0;
int parser_tests_failed = 0;
std::vector<std::string> failed_tests_parser;

void expect_ast(const std::string& test_name,
    const std::string& source,
    const std::string& expected)
{
    std::cout << "Running test: " << test_name << std::endl;
    std::cout << "Source:\n" << source << std::endl;

    parser_tests_total++;

    try {
        Scanner scanner(source);
        auto tokens = scanner.scan_tokens();

        unique_ptr<StorageManager> storage = std::make_unique<StorageManager>();
        for (auto token : tokens) storage->add_token(token);

        Parser parser;
        parser.set_storage_manager(storage.get());
        parser.parse_script();

        const auto& events = parser.get_events();
        vector<string> keys;
        keys.reserve(events.size());
        for (auto& kv : events) keys.push_back(kv.first);
        sort(keys.begin(), keys.end());

        string actual;
        for (auto& key : keys)
            actual += print(events.at(key)) + "\n";

        if (actual != expected) {
            std::cout << "AST mismatch!\n";
            std::cout << "Expected:\n" << expected;
            std::cout << "Actual:\n" << actual;
            failed_tests_parser.push_back(test_name);
            std::cout << "Parser Test \"" << test_name << "\" failed." << std::endl;
            return;
        }
        std::cout << "Parser Test \"" << test_name << "\" passed." << std::endl;
    }
    catch (const exception& e) {
        std::cout << "Unexpected exception in test \"" << test_name << "\": "
            << e.what() << "\n";
        failed_tests_parser.push_back(test_name);
    }
}

void expect_parse_error(const std::string& test_name,
    const std::string& source)
{
    cout << "Running test: " << test_name << "\n";

    parser_tests_total++;

    try {
        Scanner scanner(source);
        auto tokens = scanner.scan_tokens();

        unique_ptr<StorageManager> storage = std::make_unique<StorageManager>();
        for (auto token : tokens) storage->add_token(token);

        Parser parser;
        parser.set_storage_manager(storage.get());
        parser.parse_script();

        std::cout << "Expected parse error, but parsing succeeded.\n";
        failed_tests_parser.push_back(test_name);
    }
    catch (const std::exception&) {
        std::cout << "Test \"" << test_name << "\" correctly failed, passed.\n";
    }
}

void test_atomic_success() {
    std::cout << "Running atomic success tests..." << std::endl;

    expect_ast("atomic: identifier",
        "on test:\n    x\n",
        "(Event test:\n"
        "\t(Command (Ident x))\n"
        ")\n"
    );

    expect_ast("atomic: identifier with literal arg",
        "on test:\n    x 5\n",
        "(Event test:\n"
        "\t(Command (Ident x) (Literal 5))\n"
        ")\n"
    );

    expect_ast("atomic: grouping identifier",
        "on test:\n    x (y)\n",
        "(Event test:\n"
        "\t(Command (Ident x) (Group (Ident y)))\n"
        ")\n"
    );

    expect_ast("atomic: member access",
        "on test:\n    x y.z\n",
        "(Event test:\n"
        "\t(Command (Ident x) (Member (Ident y) z))\n"
        ")\n"
    );

    expect_ast("atomic: member access",
        "on test:\n    x.y z\n",
        "(Event test:\n"
        "\t(Command (Member (Ident x) y) (Ident z))\n"
        ")\n"
    );

    expect_ast("atomic: array number argument",
        "on test:\n    x [1, 2, 3]\n",
        "(Event test:\n"
        "\t(Command (Ident x) (Array (Literal 1) (Literal 2) (Literal 3)))\n"
        ")\n"
    );

    expect_ast("atomic: array string argument",
        "on test:\n    x [\"hello\", \"world\"]\n",
        "(Event test:\n"
        "\t(Command (Ident x) (Array (Literal \"hello\") (Literal \"world\")))\n"
        ")\n"
    );

    expect_ast("atomic: array identifier argument",
        "on test:\n    x [hello, world]\n",
        "(Event test:\n"
        "\t(Command (Ident x) (Array (Ident hello) (Ident world)))\n"
        ")\n"
    );

    expect_ast("atomic: empty array argument",
        "on test:\n    x []\n",
        "(Event test:\n"
        "\t(Command (Ident x) (Array))\n"
        ")\n"
    );

    expect_ast("atomic: nested array argument",
        "on test:\n    x [1, [2, 3], 4]\n",
        "(Event test:\n"
        "\t(Command (Ident x) (Array (Literal 1) (Array (Literal 2) (Literal 3)) (Literal 4)))\n"
        ")\n"
    );

    expect_ast("atomic: array grouping argument",
        "on test:\n    x [(1), (2)]\n",
        "(Event test:\n"
        "\t(Command (Ident x) (Array (Group (Literal 1)) (Group (Literal 2))))\n"
        ")\n"
    );

    expect_ast("atomic: array member access argument",
        "on test:\n    x [a.b, c.d]\n",
        "(Event test:\n"
        "\t(Command (Ident x) (Array (Member (Ident a) b) (Member (Ident c) d)))\n"
        ")\n"
    );

    expect_ast("atomic: array range argument",
        "on test:\n    x [1..5]\n",
        "(Event test:\n"
        "\t(Command (Ident x) (Array (Range (Literal 1) (Literal 5))))\n"
        ")\n"
    );

    std::cout << "Atomic (success) tests completed." << std::endl;
}

void test_atomic_failure() {
    std::cout << "Running atomic failure tests..." << std::endl;

    expect_parse_error("atomic: number literal",
        "on test:\n    123\n"
    );

    expect_parse_error("atomic: string literal",
        "on test:\n    \"hello\"\n"
    );

    expect_parse_error("atomic: boolean literal",
        "on test:\n    true\n"
    );

    expect_parse_error("atomic: grouping literal",
        "on test:\n    (x)\n"
    );

    expect_parse_error("atomic: array literal",
        "on test:\n    [1, 2, 3]\n"
    );

    expect_parse_error("atomic: range literal",
        "on test:\n    1..5\n"
    );

    expect_parse_error("atomic: invalid range literal",
        "on test:\n    x 1...5\n"
    );

    expect_parse_error("atomic: no event",
        "no event\n"
    );

    expect_parse_error("atomic: event no body",
        "on test:\n"
    );

    expect_parse_error("atomic: mixed indentation",
        "on test:\n\t    x\n"
    );

    expect_parse_error("atomic: array invalid range argument",
        "on test:\n    x [1...5]\n"
    );

    std::cout << "Atomic (failure) tests completed." << std::endl;
}

void run_parser_tests() {
    std::cout << "Running parser tests..." << std::endl;

    test_atomic_success();
    test_atomic_failure();

    parser_tests_failed = failed_tests_parser.size();

    if (failed_tests_parser.size() > 0) {
        std::cout << "The following tests failed: " << std::endl;
        for (auto& test : failed_tests_parser) {
            std::cout << test << endl;
        }
        std::cout << "Parser tests did not pass." << std::endl;
    } else {
        std::cout << "Parser tests passed." << std::endl;
    }

    std::cout << "Parser test completed." << std::endl;
}

int get_parser_tests_total() {
    return parser_tests_total;
}

int get_parser_tests_failed() {
    return failed_tests_parser.size();
}