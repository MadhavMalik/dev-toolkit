cmake_minimum_required(VERSION 3.21)
project(dev-toolkit)


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Crow CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
# add_executable(main main.cpp)
# add_executable(server server.cpp)
# add_executable(lambda_registry lambda_registry.hpp)
# add_executable(events events.hpp)
add_library(events INTERFACE)
target_link_libraries(events INTERFACE nlohmann_json::nlohmann_json)

# target_link_libraries(main PRIVATE Crow::Crow asio::asio)
# target_link_libraries(server PRIVATE Crow::Crow asio::asio nlohmann_json::nlohmann_json)
# target_link_libraries(lambda_registry PRIVATE Crow::Crow asio::asio nlohmann_json::nlohmann_json)
add_executable(test test.cpp)
target_link_libraries(test PRIVATE events)
