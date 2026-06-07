# tests/CMakeLists.txt

add_executable(scriba_tests
	test_main.cpp
	scanner_tests.cpp
	parser_tests.cpp
	expression_tests.cpp
	ast_printer.h
	ast_printer.cpp
	test_utils.h
	test_utils.cpp
)

target_link_libraries(scriba_tests PRIVATE scriba)

target_include_directories(scriba_tests PRIVATE
	${PROJECT_SOURCE_DIR}/include
)