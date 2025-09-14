project(dev-toolkit)

cmake_minimum_required(VERSION 3.8)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Crow CONFIG REQUIRED)
add_executable(main main.cpp)

target_link_libraries(main PRIVATE Crow::Crow asio::asio)