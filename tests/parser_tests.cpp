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

void test_unary_success() {
    expect_ast("unary: negative literal",
        "on test:\n    x -1\n",
        "(Event test:\n"
        "\t(Command (Ident x) (Unary - (Literal 1)))\n"
        ")\n"
    );

    expect_ast("unary: not literal",
        "on test:\n    x !1\n",
        "(Event test:\n"
        "\t(Command (Ident x) (Unary ! (Literal 1)))\n"
        ")\n"
    );

    expect_ast("unary: negative identifier",
        "on test:\n    x -y\n",
        "(Event test:\n"
        "\t(Command (Ident x) (Unary - (Ident y)))\n"
        ")\n"
    );

    expect_ast("unary: not identifier",
        "on test:\n    x !flag\n",
        "(Event test:\n"
        "\t(Command (Ident x) (Unary ! (Ident flag)))\n"
        ")\n"
    );

    expect_ast("unary: grouping",
        "on test:\n    x -(1)\n",
        "(Event test:\n"
        "\t(Command (Ident x) (Unary - (Group (Literal 1))))\n"
        ")\n"
    );

    expect_ast("unary: member access",
        "on test:\n    x -a.b\n",
        "(Event test:\n"
        "\t(Command (Ident x) (Unary - (Member (Ident a) b)))\n"
        ")\n"
    );

    expect_ast("unary: member access",
        "on test:\n    x !a.b.c\n",
        "(Event test:\n"
        "\t(Command (Ident x) (Unary ! (Member (Member (Ident a) b) c)))\n"
        ")\n"
    );

    expect_ast("unary: array literal",
        "on test:\n    x -[1,2]\n",
        "(Event test:\n"
        "\t(Command (Ident x) (Unary - (Array (Literal 1) (Literal 2))))\n"
        ")\n"
    );

    expect_ast("unary: array literal",
        "on test:\n    x ![1,2]\n",
        "(Event test:\n"
        "\t(Command (Ident x) (Unary ! (Array (Literal 1) (Literal 2))))\n"
        ")\n"
    );

    expect_ast("unary: range",
        "on test:\n    x -(1..5)\n",
        "(Event test:\n"
        "\t(Command (Ident x) (Unary - (Group (Range (Literal 1) (Literal 5)))))\n"
        ")\n"
    );

    expect_ast("unary: range",
        "on test:\n    x !(1..5)\n",
        "(Event test:\n"
        "\t(Command (Ident x) (Unary ! (Group (Range (Literal 1) (Literal 5)))))\n"
        ")\n"
    );

    expect_ast("unary: chain",
        "on test:\n    x !!-y\n",
        "(Event test:\n"
        "\t(Command (Ident x) (Unary ! (Unary ! (Unary - (Ident y)))))\n"
        ")\n"
    );

    expect_ast("unary: after postfix",
        "on test:\n    x -a.b\n",
        "(Event test:\n"
        "\t(Command (Ident x) (Unary - (Member (Ident a) b)))\n"
        ")\n"
    );

    expect_ast("unary: grouping tightness",
        "on test:\n    x !((1))\n",
        "(Event test:\n"
        "\t(Command (Ident x) (Unary ! (Group (Group (Literal 1)))))\n"
        ")\n"
    );
}

void test_unary_failure() {
    std::cout << "Running unary failure tests..." << std::endl;

    expect_parse_error("unary: missing operand after minus",
        "on test:\n    x -\n"
    );

    expect_parse_error("unary: missing operand after not",
        "on test:\n    x !\n"
    );

    expect_parse_error("unary: invalid token after minus",
        "on test:\n    x -,\n"
    );

    expect_parse_error("unary: invalid token after not",
        "on test:\n    x !,\n"
    );

    expect_parse_error("unary: unexpected ')' after unary operator",
        "on test:\n    x -)\n"
    );

    expect_parse_error("unary: unexpected ']' after unary operator",
        "on test:\n    x !]\n"
    );

    expect_parse_error("unary: missing operand before comma",
        "on test:\n    x [ - , 1 ]\n"
    );
    
    expect_parse_error("unary: invalid range start",
        "on test:\n    x -..5\n"
    );
    
    expect_parse_error("unary: missing right operand of range",
        "on test:\n    x 1..-\n"
    );
    
    expect_parse_error("unary: missing operand inside grouping",
        "on test:\n    x (-)\n"
    );
    
    expect_parse_error("unary: missing operand in array element",
        "on test:\n    x [-]\n"
    );
    
    expect_parse_error("unary: expected identifier after '.'",
        "on test:\n    x a.-b\n"
    );
    
    expect_parse_error("unary: missing operand before member access",
        "on test:\n    x -.b\n"
    );

    std::cout << "Unary (failure) tests completed." << std::endl;
}

void test_binary_success() {
    expect_ast("binary: addition",
        "on test:\n    x 1 + 2\n",
        "(Event test:\n"
        "\t(Command (Ident x) (Binary + (Literal 1) (Literal 2)))\n"
        ")\n"
    );
    
    expect_ast("binary: subtraction",
        "on test:\n    x 54 - 25\n",
        "(Event test:\n"
        "\t(Command (Ident x) (Binary - (Literal 54) (Literal 25)))\n"
        ")\n"
    );
    
    expect_ast("binary: multiplication",
        "on test:\n    x 123 * 256\n",
        "(Event test:\n"
        "\t(Command (Ident x) (Binary * (Literal 123) (Literal 256)))\n"
        ")\n"
    );
    
    expect_ast("binary: division",
        "on test:\n    x 8 / 2\n",
        "(Event test:\n"
        "\t(Command (Ident x) (Binary / (Literal 8) (Literal 2)))\n"
        ")\n"
    );
    
    expect_ast("binary: precedence * over +",
        "on test:\n    x 1 + 2 * 3\n",
        "(Event test:\n"
        "\t(Command (Ident x) (Binary + (Literal 1) (Binary * (Literal 2) (Literal 3))))\n"
        ")\n"
    );
    
    expect_ast("binary: precedence / over -",
        "on test:\n    x 10 - 6 / 2\n",
        "(Event test:\n"
        "\t(Command (Ident x) (Binary - (Literal 10) (Binary / (Literal 6) (Literal 2))))\n"
        ")\n"
    );
    
    expect_ast("binary: associativity left",
        "on test:\n    x 1 - 2 - 3\n",
        "(Event test:\n"
        "\t(Command (Ident x) (Binary - (Binary - (Literal 1) (Literal 2)) (Literal 3)))\n"
        ")\n"
    );
    
    expect_ast("binary: grouping overrides precedence",
        "on test:\n    x (1 + 2) * 3\n",
        "(Event test:\n"
        "\t(Command (Ident x) (Binary * (Group (Binary + (Literal 1) (Literal 2))) (Literal 3)))\n"
        ")\n"
    );

    expect_ast("binary: comparison less",
        "on test:\n    x 1 < 2\n",
        "(Event test:\n"
        "\t(Command (Ident x) (Binary < (Literal 1) (Literal 2)))\n"
        ")\n"
    );

    expect_ast("binary: comparison greater",
        "on test:\n    x 1 > 2\n",
        "(Event test:\n"
        "\t(Command (Ident x) (Binary > (Literal 1) (Literal 2)))\n"
        ")\n"
    );
    
    expect_ast("binary: comparison less or equal",
        "on test:\n    x 3 <= 2\n",
        "(Event test:\n"
        "\t(Command (Ident x) (Binary <= (Literal 3) (Literal 2)))\n"
        ")\n"
    );

    expect_ast("binary: comparison greater or equal",
        "on test:\n    x 4 >= 2\n",
        "(Event test:\n"
        "\t(Command (Ident x) (Binary >= (Literal 4) (Literal 2)))\n"
        ")\n"
    );

    expect_ast("binary: equality equal",
        "on test:\n    x a == b\n",
        "(Event test:\n"
        "\t(Command (Ident x) (Binary == (Ident a) (Ident b)))\n"
        ")\n"
    );

    expect_ast("binary: and",
        "on test:\n    x a and b\n",
        "(Event test:\n"
        "\t(Command (Ident x) (And (Ident a) (Ident b)))\n"
        ")\n"
    );

    expect_ast("binary: or",
        "on test:\n    x a or b\n",
        "(Event test:\n"
        "\t(Command (Ident x) (Or (Ident a) (Ident b)))\n"
        ")\n"
    );

    expect_ast("binary: equality equal",
        "on test:\n    x a != b\n",
        "(Event test:\n"
        "\t(Command (Ident x) (Binary != (Ident a) (Ident b)))\n"
        ")\n"
    );

    expect_ast("binary: precedence comparison over equality",
        "on test:\n    x 1 < 2 == true\n",
        "(Event test:\n"
        "\t(Command (Ident x) (Binary == (Binary < (Literal 1) (Literal 2)) (Literal true)))\n"
        ")\n"
    );

    expect_ast("binary: unary inside binary",
        "on test:\n    x -1 + 2\n",
        "(Event test:\n"
        "\t(Command (Ident x) (Binary + (Unary - (Literal 1)) (Literal 2)))\n"
        ")\n"
    );

    expect_ast("binary: member access inside binary",
        "on test:\n    x a.b + c.d\n",
        "(Event test:\n"
        "\t(Command (Ident x) (Binary + (Member (Ident a) b) (Member (Ident c) d)))\n"
        ")\n"
    );

    expect_ast("binary: array inside binary",
        "on test:\n    x [1,2] + [3,4]\n",
        "(Event test:\n"
        "\t(Command (Ident x) (Binary + (Array (Literal 1) (Literal 2)) (Array (Literal 3) (Literal 4))))\n"
        ")\n"
    );

    expect_ast("binary: arithmetic inside range",
        "on test:\n    x 1+1..2+2\n",
        "(Event test:\n"
        "\t(Command (Ident x) (Range (Binary + (Literal 1) (Literal 1)) (Binary + (Literal 2) (Literal 2))))\n"
        ")\n"
    );

    expect_ast("binary: grouping and range",
        "on test:\n    x (a+b)..(c+d)\n",
        "(Event test:\n"
        "\t(Command (Ident x) (Range (Group (Binary + (Ident a) (Ident b))) (Group (Binary + (Ident c) (Ident d)))))\n"
        ")\n"
    );

    expect_ast("binary: complex mixed",
        "on test:\n    x a.b * (1 + 2) == c.d / 3\n",
        "(Event test:\n"
        "\t(Command (Ident x) (Binary == (Binary * (Member (Ident a) b) (Group (Binary + (Literal 1) (Literal 2)))) (Binary / (Member (Ident c) d) (Literal 3))))\n"
        ")\n"
    );

    expect_ast("binary: precedence and over or",
        "on test:\n    x a or b and c\n",
        "(Event test:\n"
        "\t(Command (Ident x) (Or (Ident a) (And (Ident b) (Ident c))))\n"
        ")\n"
    );

    expect_ast("binary: equality inside logical and",
        "on test:\n    x a == b and c == d\n",
        "(Event test:\n"
        "\t(Command (Ident x) (And (Binary == (Ident a) (Ident b)) (Binary == (Ident c) (Ident d))))\n"
        ")\n"
    );

    std::cout << "Binary (success) tests completed." << std::endl;
}

void test_binary_failure() {
    std::cout << "Running binary failure tests..." << std::endl;

    expect_parse_error("binary: missing operand for +",
        "on test:\n    x 1 +\n"
    );

    expect_parse_error("binary: missing operand for -",
        "on test:\n    x 1 -\n"
    );

    expect_parse_error("binary: missing operand for *",
        "on test:\n    x 1 *\n"
    );

    expect_parse_error("binary: missing operand for /",
        "on test:\n    x 1 /\n"
    );

    expect_parse_error("binary: missing operand for <",
        "on test:\n    x 1 <\n"
    );

    expect_parse_error("binary: missing operand for >",
        "on test:\n    x 1 >\n"
    );

    expect_parse_error("binary: missing operand for <=",
        "on test:\n    x 1 <=\n"
    );

    expect_parse_error("binary: missing operand for >=",
        "on test:\n    x 1 >=\n"
    );

    expect_parse_error("binary: missing operand for ==",
        "on test:\n    x 1 ==\n"
    );

    expect_parse_error("binary: missing operand for !=",
        "on test:\n    x 1 !=\n"
    );

    expect_parse_error("binary: missing left operand for +",
        "on test:\n    x + 1\n"
    );

    expect_parse_error("binary: missing left operand for *",
        "on test:\n    x * 2\n"
    );

    expect_parse_error("binary: missing left operand for ==",
        "on test:\n    x == 25\n"
    );

    expect_parse_error("binary: missing left operand for <",
        "on test:\n    x < 6\n"
    );

    expect_parse_error("binary: missing left operand for >",
        "on test:\n    x > 3\n"
    );

    expect_parse_error("binary: invalid operator sequence ++",
        "on test:\n    x 1 ++ 2\n"
    );

    expect_parse_error("binary: invalid operator sequence +*",
        "on test:\n    x 1 +* 2\n"
    );

    expect_parse_error("binary: invalid operator sequence ==<",
        "on test:\n    x 1 ==< 2\n"
    );

    expect_parse_error("binary: invalid operator sequence */",
        "on test:\n    x 1 */ 2\n"
    );

    expect_parse_error("binary: invalid operator sequence !==",
        "on test:\n    x a !== b\n"
    );

    expect_parse_error("binary: operator before range",
        "on test:\n    x 1 + ..5\n"
    );

    expect_parse_error("binary: operator after range",
        "on test:\n    x 1.. + 5\n"
    );

    expect_parse_error("binary: missing operand inside grouping",
        "on test:\n    x (1 + )\n"
    );

    expect_parse_error("binary: missing operand inside grouping 2",
        "on test:\n    x ( + 1)\n"
    );

    expect_parse_error("binary: missing operand inside array",
        "on test:\n    x [1 + ,2]\n"
    );

    expect_parse_error("binary: missing operand inside array 2",
        "on test:\n    x [ , 1 + 2]\n"
    );

    expect_parse_error("binary: missing operand inside array 3",
        "on test:\n    x [ 1, 2 *]\n"
    );

    expect_parse_error("binary: operator before member access",
        "on test:\n    x 1 + .a\n"
    );

    expect_parse_error("binary: operator after member access dot",
        "on test:\n    x a. + b\n"
    );

    expect_parse_error("binary: missing operand inside range",
        "on test:\n    x 1..2+\n"
    );

    expect_parse_error("binary: missing operand inside range 2",
        "on test:\n    x 1+..2\n"
    );

    expect_parse_error("binary: missing operand inside range 3",
        "on test:\n    x 1..+2\n"
    );

    expect_parse_error("binary: trailing operator",
        "on test:\n    x 1 + 2 -\n"
    );

    expect_parse_error("binary: trailing operator 2",
        "on test:\n    x a == b and\n"
    );

    expect_parse_error("binary: trailing operator 3",
        "on test:\n    x a == b or\n"
    );

    expect_parse_error("binary: leading operator",
        "on test:\n    x * a\n"
    );

    expect_parse_error("binary: leading operator 2",
        "on test:\n    x == a\n"
    );

    expect_parse_error("binary: operator between member access",
        "on test:\n    x a.b + .c\n"
    );

    expect_parse_error("binary: operator after array literal",
        "on test:\n    x [1,2] + ,3\n"
    );

    std::cout << "Binary (failure) tests completed." << std::endl;
}

void run_parser_tests() {
    std::cout << "Running parser tests..." << std::endl;

    test_atomic_success();
    test_atomic_failure();
    test_unary_success();
    test_unary_failure();
    test_binary_success();
    test_binary_failure();

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