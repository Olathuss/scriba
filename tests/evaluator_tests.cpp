#include <vector>
#include <string>
#include <iostream>

#include "scriba/evaluator.h"
#include "test_utils.h"

using namespace scriba;

int ev_tests_total = 0;
int ev_tests_failed = 0;
std::vector<std::string> failed_tests_ev;

void expect_value(const std::string& test_name,
    const std::string& source,
    const Value& expected)
{
    ev_tests_total++;

    Environment env = testutils::make_env_with_test_object();
    TypeRegistry reg;

    try {
        auto expr = testutils::parse_expr(source);
        Evaluator eval = Evaluator(reg, env);
        auto result = eval.evaluate_expression(*expr);

        if (result != expected) {
            failed_tests_ev.push_back(test_name);
        }
    }
    catch (const std::exception& e) {
        std::cout << test_name + " (exception: " + e.what() + ")" << std::endl;
        failed_tests_ev.push_back(test_name);
    }
}

void expect_value_failure(const std::string& test_name,
    const std::string& source)
{
    ev_tests_total++;

    Environment env = testutils::make_env_with_test_object();
    TypeRegistry reg;

    try {
        auto expr = testutils::parse_expr(source);
        Evaluator eval = Evaluator(reg, env);
        auto result = eval.evaluate_expression(*expr);

        std::cout << "Expected evaluation failure for \"" + test_name + "\", but evaluation succeeded." << std::endl;
        std::cout << "Source: " + source << std::endl;
        failed_tests_ev.push_back(test_name);
    }
    catch (const std::exception& e) {
        std::cout << "Test \"" << test_name << "\" correctly failed, passed.\n";
    }
}

void test_expressions_success() {
    std::cout << "Running expression success tests..." << std::endl;

    expect_value("literal number 5", "5", Value(5.0));
    expect_value("addition", "1 + 2", Value(3.0));
    expect_value("subtraction", "10 - 3", Value(7.0));
    expect_value("multiplication", "2 * 4", Value(8.0));
    expect_value("division", "8 / 2", Value(4.0));
    expect_value("unary -", "-5", Value(-5.0));
    expect_value("unary !true", "!true", Value(false));
    expect_value("unary !false", "!false", Value(true));
    expect_value("grouping", "(1 + 2) * 3", Value(9.0));
    expect_value("precedence *", "1 + 2 * 3", Value(7.0));
    expect_value("precedence /", "1 + 6 / 2", Value(4.0));
    expect_value("range", "1..5", Value(Range(1, 5)));
    expect_value("property read test.x", "test.x", Value(10.0));
    expect_value("property read test.y", "test.y", Value(20.0));
    expect_value("add property read", "test.x + test.y", Value(30.0));
    expect_value("method call", "test.add(3, 4)", Value(7.0));
    expect_value("method call with property read", "test.add(test.x, test.y)", Value(30.0));
    expect_value("string", "\"hello world\"", Value("hello world"));
    expect_value("true and true (true)", "true and true", Value(true));
    expect_value("true and false (false)", "true and false", Value(false));
    expect_value("false and false (false)", "true and false", Value(false));
    expect_value("true or true (true)", "true or true", Value(true));
    expect_value("true or false (true)", "true or false", Value(true));
    expect_value("false or true (true)", "false or true", Value(true));
    expect_value("false or false (false)", "false or false", Value(false));
    expect_value("number comparison == (true)", "1 == 1", Value(true));
    expect_value("number comparison == (false)", "1 == 2", Value(false));
    expect_value("number comparison != (true)", "1 != 2", Value(true));
    expect_value("number comparison != (false)", "1 != 1", Value(false));
    expect_value("number comparison > (true)", "2 > 1", Value(true));
    expect_value("number comparison > (false)", "1 > 2", Value(false));
    expect_value("number comparison < (true)", "1 < 2", Value(true));
    expect_value("number comparison < (false)", "2 < 1", Value(false));
    expect_value("number comparison <= (true equal)", "1 <= 1", Value(true));
    expect_value("number comparison <= (true less)", "1 <= 2", Value(true));
    expect_value("number comparison <= (false)", "2 <= 1", Value(false));
    expect_value("number comparison >= (true equal)", "2 >= 2", Value(true));
    expect_value("number comparison >= (true greater)", "2 >= 1", Value(true));
    expect_value("number comparison >= (false)", "1 >= 2", Value(false));
    expect_value("string comparison == (true)", "\"hello world\" == \"hello world\"", Value(true));
    expect_value("string comparison == (false)", "\"hello world\" == \"goodbye world\"", Value(false));
    expect_value("string comparison != (true)", "\"hello world\" != \"goodbye world\"", Value(true));
    expect_value("string comparison != (false)", "\"hello world\" != \"hello world\"", Value(false));
    expect_value("object comparison == (true)", "test == test", Value(true));
    expect_value("object comparison != (false)", "test != test", Value(false));

    std::cout << "Expression (success) tests completed." << std::endl;
}

void test_expression_cross_type_comparison_success() {
    std::cout << "Running expression cross-type comparison success tests..." << std::endl;

    expect_value("invalid comparison string == number", "\"hello\" == 5", Value(false));
    expect_value("invalid comparison string != number", "\"hello\" != 5", Value(false));
    expect_value("invalid comparison string == bool", "\"hello\" == true", Value(false));
    expect_value("invalid comparison string != bool", "\"hello\" != true", Value(false));
    expect_value("invalid comparison string == range", "\"hello\" == 1..4", Value(false));
    expect_value("invalid comparison string != range", "\"hello\" != 1..4", Value(false));
    expect_value("invalid comparison string == object", "\"hello\" == test", Value(false));
    expect_value("invalid comparison string != object", "\"hello\" != test", Value(false));
    expect_value("invalid comparison number == bool", "5 == false", Value(false));
    expect_value("invalid comparison number != bool", "5 != false", Value(false));
    expect_value("invalid comparison number == object", "5 == test", Value(false));
    expect_value("invalid comparison number != object", "5 != test", Value(false));
    expect_value("invalid comparison number == range", "5 == 1..4", Value(false));
    expect_value("invalid comparison number != range", "5 != 1..4", Value(false));

    std::cout << "Expression cross-type comparison (success) tests completed." << std::endl;
}

void test_expressions_failure() {
    std::cout << "Running expression failure tests..." << std::endl;

    expect_value_failure("invalid comparison string < number", "\"hello\" < 5");
    expect_value_failure("invalid comparison string <= number", "\"hello\" <= 5");
    expect_value_failure("invalid comparison string > number", "\"hello\" > 5");
    expect_value_failure("invalid comparison string >= number", "\"hello\" >= 5");
    expect_value_failure("invalid comparison string < bool", "\"hello\" < true");
    expect_value_failure("invalid comparison string > bool", "\"hello\" > true");
    expect_value_failure("invalid comparison string <= bool", "\"hello\" <= true");
    expect_value_failure("invalid comparison string >= bool", "\"hello\" >= true");
    expect_value_failure("invalid comparison string < range", "\"hello\" < 1..4");
    expect_value_failure("invalid comparison string > range", "\"hello\" > 1..4");
    expect_value_failure("invalid comparison string <= range", "\"hello\" <= 1..4");
    expect_value_failure("invalid comparison string >= range", "\"hello\" >= 1..4");
    expect_value_failure("invalid comparison string < object", "\"hello\" < test");
    expect_value_failure("invalid comparison string > object", "\"hello\" > test");
    expect_value_failure("invalid comparison string <= object", "\"hello\" <= test");
    expect_value_failure("invalid comparison string >= object", "\"hello\" >= test");
    expect_value_failure("invalid comparison number < bool", "5 < false");
    expect_value_failure("invalid comparison number > bool", "5 > false");
    expect_value_failure("invalid comparison number <= bool", "5 <= false");
    expect_value_failure("invalid comparison number >= bool", "5 >= false");
    expect_value_failure("invalid comparison number < object", "5 < test");
    expect_value_failure("invalid comparison number > object", "5 > test");
    expect_value_failure("invalid comparison number <= object", "5 <= test");
    expect_value_failure("invalid comparison number >= object", "5 >= test");
    expect_value_failure("invalid comparison number < range", "5 < 1..4");
    expect_value_failure("invalid comparison number > range", "5 > 1..4");
    expect_value_failure("invalid comparison number <= range", "5 <= 1..4");
    expect_value_failure("invalid comparison number >= range", "5 >= 1..4");

    std::cout << "Expressions (failure) tests completed." << std::endl;
}

void run_evaluator_tests() {
    std::cout << "Running evaluator tests..." << std::endl;

    test_expressions_success();
    test_expressions_failure();

    ev_tests_failed = failed_tests_ev.size();

    if (failed_tests_ev.size() > 0) {
        std::cout << "The following tests failed: " << std::endl;
        for (auto& test : failed_tests_ev) {
            std::cout << test << std::endl;
        }
        std::cout << "Evaluator tests did not pass." << std::endl;
    } else {
        std::cout << "Evaluator tests passed." << std::endl;
    }

    std::cout << "Evaluator test completed." << std::endl;
}

int get_evaluator_tests_total() {
    return ev_tests_total;
}

int get_evaluator_tests_failed() {
    return failed_tests_ev.size();
}