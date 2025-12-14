project(dev-toolkit)

cmake_minimum_required(VERSION 3.8)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Crow CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
add_executable(main main.cpp)
add_executable(server server.cpp)

target_link_libraries(main PRIVATE Crow::Crow asio::asio)
target_link_libraries(server PRIVATE Crow::Crow asio::asio nlohmann_json::nlohmann_json)
