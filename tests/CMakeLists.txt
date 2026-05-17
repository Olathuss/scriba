# tests/CMakeLists.txt

add_executable(scriba_tests
	test_main.cpp
	scanner_tests.cpp
	parser_tests.cpp
)

target_link_libraries(scriba_tests PRIVATE scriba)

target_include_directories(scriba_tests PRIVATE
	${PROJECT_SOURCE_DIR}/include
)