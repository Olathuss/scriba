#include <iostream>

void run_scanner_tests();
void run_parser_tests();

using namespace std;

int main(int argc, char** argv) {
	run_scanner_tests();
	run_parser_tests();

	cout << "Testing completed" << endl;

	return 0;
}