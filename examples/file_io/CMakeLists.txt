cmake_minimum_required(VERSION 3.20)

add_executable(file_io
	main.cpp
)

target_link_libraries(file_io PRIVATE scriba)

target_include_directories(file_io PRIVATE
	${PROJECT_SOURCE_DIR}/include
)