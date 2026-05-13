cmake_minimum_required(VERSION 3.10)
project(tatlin_sorter)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)

add_executable(sorter
  ${SRC_DIR}/main.cpp
  ${SRC_DIR}/file_tape.cpp
  ${SRC_DIR}/config.cpp
  ${SRC_DIR}/sorter.cpp
  ${SRC_DIR}/utils.cpp
)

target_include_directories(sorter PRIVATE ${SRC_DIR})
find_package(Threads REQUIRED)
target_link_libraries(sorter PRIVATE Threads::Threads)

add_executable(sorter_tests
  ${SRC_DIR}/file_tape.cpp
  ${SRC_DIR}/config.cpp
  ${SRC_DIR}/sorter.cpp
  ${SRC_DIR}/utils.cpp
  ${CMAKE_SOURCE_DIR}/tests/test_runner.cpp
)

target_include_directories(sorter_tests PRIVATE ${SRC_DIR} ${CMAKE_SOURCE_DIR}/tests)
target_link_libraries(sorter_tests PRIVATE Threads::Threads)
