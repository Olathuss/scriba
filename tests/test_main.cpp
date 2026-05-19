#include <iostream>

void run_scanner_tests();
int get_scanner_tests_total();
int get_scanner_tests_failed();
void run_parser_tests();
int get_parser_tests_total();
int get_parser_tests_failed();

using namespace std;

void print_test_report() {
    int scanner_tests_total = get_scanner_tests_total();
    int scanner_tests_failed = get_scanner_tests_failed();
    int parser_tests_total = get_parser_tests_total();
    int parser_tests_failed = get_parser_tests_failed();

    std::cout << "\n====================\n";
    std::cout << "      TEST REPORT\n";
    std::cout << "====================\n";

    std::cout << "Scanner Tests: "
        << (scanner_tests_total - scanner_tests_failed)
        << " / " << scanner_tests_total
        << " passed";

    if (scanner_tests_failed > 0)
        std::cout << "  (" << scanner_tests_failed << " failed)";
    std::cout << "\n";

    std::cout << "Parser Tests: "
        << (parser_tests_total - parser_tests_failed)
        << " / " << parser_tests_total
        << " passed";

    if (parser_tests_failed > 0)
        std::cout << "  (" << parser_tests_failed << " failed)";
    std::cout << "\n";

    std::cout << "====================\n";
}

int main(int argc, char** argv) {
	run_scanner_tests();
	run_parser_tests();

	cout << "Testing completed" << endl;

    print_test_report();

	return 0;
}