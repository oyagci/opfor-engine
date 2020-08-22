cmake_minimum_required(VERSION 3.5)

set(NAME TestGame)
project(${NAME})

add_subdirectory(../ LazyGL)

add_executable(${NAME} TestGame.cpp)
target_link_libraries(${NAME} LazyGL)
target_include_directories(${NAME} PUBLIC ../includes/)