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

void test_expression_success() {
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
    expect_value("object comparison == (false)", "test == other_test", Value(false));
    expect_value("object comparison != (true)", "test != other_test", Value(true));
    expect_value("range comparison == (true)", "1..4 == 1..4", Value(true));
    expect_value("range comparison == (true)", "1..4 == 2..5", Value(false));
    expect_value("range comparison != (true)", "1..4 != 2..5", Value(true));
    expect_value("range comparison != (true)", "1..4 != 1..4", Value(false));
    expect_value("left assoc subtraction", "10 - 3 - 2", Value(5.0));
    expect_value("left assoc addition", "1 + 2 + 3", Value(6.0));
    expect_value("left assoc division", "8 / 2 / 2", Value(2.0));


    std::cout << "Expression (success) tests completed." << std::endl;
}

void test_expression_failure() {
    std::cout << "Running expression failure tests..." << std::endl;

    expect_value_failure("invalid arithmetic string + number", "\"hello\" + 5");
    expect_value_failure("invalid arithmetic string - number", "\"hello\" - 5");
    expect_value_failure("invalid arithmetic string * number", "\"hello\" * 5");
    expect_value_failure("invalid arithmetic string / number", "\"hello\" / 5");
    expect_value_failure("invalid arithmetic string + bool", "\"hello\" + true");
    expect_value_failure("invalid arithmetic string - bool", "\"hello\" - true");
    expect_value_failure("invalid arithmetic string * bool", "\"hello\" * true");
    expect_value_failure("invalid arithmetic string / bool", "\"hello\" / true");
    expect_value_failure("invalid arithmetic string + object", "\"hello\" + test");
    expect_value_failure("invalid arithmetic string - object", "\"hello\" - test");
    expect_value_failure("invalid arithmetic string * object", "\"hello\" * test");
    expect_value_failure("invalid arithmetic string / object", "\"hello\" / test");
    expect_value_failure("invalid arithmetic string + range", "\"hello\" + 1..4");
    expect_value_failure("invalid arithmetic string - range", "\"hello\" - 1..4");
    expect_value_failure("invalid arithmetic string * range", "\"hello\" * 1..4");
    expect_value_failure("invalid arithmetic string / range", "\"hello\" / 1..4");
    expect_value_failure("invalid arithmetic number + bool", "5 + true");
    expect_value_failure("invalid arithmetic number - bool", "5 - true");
    expect_value_failure("invalid arithmetic number * bool", "5 * true");
    expect_value_failure("invalid arithmetic number / bool", "5 / true");
    expect_value_failure("invalid arithmetic number + object", "5 + test");
    expect_value_failure("invalid arithmetic number - object", "5 - test");
    expect_value_failure("invalid arithmetic number * object", "5 * test");
    expect_value_failure("invalid arithmetic number / object", "5 / test");
    expect_value_failure("invalid arithmetic bool + object", "true + test");
    expect_value_failure("invalid arithmetic bool - object", "true - test");
    expect_value_failure("invalid arithmetic bool * object", "true * test");
    expect_value_failure("invalid arithmetic bool / object", "true / test");
    expect_value_failure("invalid arithmetic bool + range", "true + 1..4");
    expect_value_failure("invalid arithmetic bool - range", "true - 1..4");
    expect_value_failure("invalid arithmetic bool * range", "true * 1..4");
    expect_value_failure("invalid arithmetic bool / range", "true / 1..4");
    expect_value_failure("invalid arithmetic object + range", "test + 1..4");
    expect_value_failure("invalid arithmetic object - range", "test - 1..4");
    expect_value_failure("invalid arithmetic object * range", "test * 1..4");
    expect_value_failure("invalid arithmetic object / range", "test / 1..4");

    std::cout << "Expression (failure) tests completed." << std::endl;
}

void test_expression_cross_type_comparison_success() {
    std::cout << "Running expression cross-type comparison success tests..." << std::endl;

    expect_value("invalid comparison string == number", "\"hello\" == 5", Value(false));
    expect_value("invalid comparison string != number", "\"hello\" != 5", Value(true));
    expect_value("invalid comparison string == bool", "\"hello\" == true", Value(false));
    expect_value("invalid comparison string != bool", "\"hello\" != true", Value(true));
    expect_value("invalid comparison string == range", "\"hello\" == 1..4", Value(false));
    expect_value("invalid comparison string != range", "\"hello\" != 1..4", Value(true));
    expect_value("invalid comparison string == object", "\"hello\" == test", Value(false));
    expect_value("invalid comparison string != object", "\"hello\" != test", Value(true));
    expect_value("invalid comparison number == bool", "5 == false", Value(false));
    expect_value("invalid comparison number != bool", "5 != false", Value(true));
    expect_value("invalid comparison number == object", "5 == test", Value(false));
    expect_value("invalid comparison number != object", "5 != test", Value(true));
    expect_value("invalid comparison number == range", "5 == 1..4", Value(false));
    expect_value("invalid comparison number != range", "5 != 1..4", Value(true));

    std::cout << "Expression cross-type comparison (success) tests completed." << std::endl;
}

void test_expression_cross_type_comparison_failure() {
    std::cout << "Running expression cross-type comparison failure tests..." << std::endl;

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

    std::cout << "Expressions cross-type comparison (failure) tests completed." << std::endl;
}

void test_string_manipulation_success() {
    std::cout << "Running expression string manipulation (success) tests..." << std::endl;

    expect_value("string concat", "\"hello \" + \"world\"", Value("hello world"));

    std::cout << "Expressions string manipulations (success) tests completed." << std::endl;
}

void test_ev_unary_success() {
    std::cout << "Running unary (success) tests..." << std::endl;

    expect_value("unary bool (true)", "!false", Value(true));
    expect_value("unary bool (false)", "!true", Value(false));
    expect_value("unary number", "-1", Value(-1.0));
    expect_value("unary unary number", "--1", Value(1.0));
    expect_value("unary object property", "-test.x", Value(-10.0));
    expect_value("unary unary object property", "--test.y", Value(20.0));
    expect_value("unary object method", "-test.add(10, 10)", Value(-20.0));

    std::cout << "Expressions unary (success) tests completed." << std::endl;
}

void test_invalid_unary_failure() {
    std::cout << "Running invalid unary (failure) tests..." << std::endl;

    expect_value_failure("unary string", "-\"hello\"");
    expect_value_failure("unary number", "!5");
    expect_value_failure("unary range", "!1..4");
    expect_value_failure("unary object property", "!test.x");
    expect_value_failure("unary object method", "!test.add(1,1)");

    std::cout << "Expressions invalid unary (failure) tests completed." << std::endl;
}

void test_member_access_failure() {
    std::cout << "Running member access (failure) tests..." << std::endl;

    expect_value_failure("number member", "5.x");
    expect_value_failure("string member", "\"test\".x");
    expect_value_failure("bool member", "true.x");
    expect_value_failure("grouping member", "(1 + 2).x");
    expect_value_failure("invalid member", "test.z");
    expect_value_failure("invalid member member", "test.test.x");
    expect_value_failure("number method", "5.add(1)");
    expect_value_failure("string method", "\"test\".add(1)");
    expect_value_failure("bool method", "true.add(1)");
    expect_value_failure("range method", "1..5.add(1)");
    expect_value_failure("missing arguments", "test.add(1)");
    expect_value_failure("too many arguments", "test.add(1, 2, 3)");

    std::cout << "Expressions member access (failure) tests completed." << std::endl;
}

void test_syntax_errors() {
    std::cout << "Running member access (failure) tests..." << std::endl;

    expect_value_failure("missing operand", "1 + ");
    expect_value_failure("missing operand group", "(1 + )");
    expect_value_failure("incomplete range", "1..");
    expect_value_failure("dangling dot", "test.");
    expect_value_failure("invalid member access", "test.(x)");

    std::cout << "Expressions member access (failure) tests completed." << std::endl;
}

void run_evaluator_tests() {
    std::cout << "Running evaluator tests..." << std::endl;

    test_expression_success();
    test_expression_failure();
    test_expression_cross_type_comparison_success();
    test_expression_cross_type_comparison_failure();
    test_string_manipulation_success();
    test_ev_unary_success();
    test_invalid_unary_failure();
    test_member_access_failure();
    test_syntax_errors();

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