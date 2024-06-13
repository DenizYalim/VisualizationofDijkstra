cmake_minimum_required(VERSION 3.24)
project(Dijkstra)

set(CMAKE_CXX_STANDARD 17)

add_executable(Dijkstra main.cpp)

set(SFML_STATIC_LIBRARIES TRUE)
set(SFML_DIR "C:/SFML-2.6.1/lib/cmake/SFML")
find_package(SFML 2.6.1 COMPONENTS system window graphics audio network REQUIRED)

include_directories(C:/SFML-2.6.1/include/SFML)
target_link_libraries(Dijkstra sfml-system sfml-window sfml-graphics sfml-audio)