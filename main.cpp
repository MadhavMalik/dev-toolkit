#include <iostream>

#include "crow.h"


class Server {
public:
    Server();
    ~Server();
    crow::SimpleApp app;
    // std::function<std::vector<std::string>>()> lambda = []() { std::cout << 42 << std::endl; return std::vector<std::string>{"42"};};
    std::function<std::vector<std::string>()> lambda = []() {
        std::cout << 42 << std::endl;
        return std::vector<std::string>{"42"};
    };
    // std::tuple<decltype(lambda)> views = std::make_tuple(lambda);
    std::vector<decltype(lambda)> views = {lambda};
    void start();
    std::function<std::string(std::string)> view = [](std::string param) {
        std::cout << "Default view run";
        return param;
    };
};

Server::Server() {};
Server::~Server() {};

void Server::start() {
    crow::mustache::set_global_base("/Users/madhavmalik/VSCProjects/dev-toolkit/templates"); //turn this into a relative path

    // make this flexible to take in query params
    CROW_ROUTE(app, "/<string>/int")([this](std::string parameter, int i){
        auto page = crow::mustache::load("index.html");
        auto param = view(parameter);
        return page.render({{"parameter",  param}, {"i", i}});
    });
    app.port(18080).multithreaded().run();
}

int main() {
    Server s;

    // goals:
    // 1. Have a vector of lambda functions, and uniquely override some/all of them
    // 2. The user should have control over the url, the input parameters, and the output parameters
    s.view = [](std::string param) { //s.views.find("index") = some_lambda_function
        std::cout << "Overloaded view run";
        return param;
    };
    auto new_func = [] () {std::cout << 43 << std::endl; return std::vector<std::string>{"43"};};
    // s.views = std::tuple_cat(s.views, std::make_tuple(new_func));
    s.views.push_back(new_func);
    // for (auto func : s.views) {
    //     func();
    // }

    s.views[0]();
    s.views[1]();


    // std::get<0>(s.views)();
    // std::get<1>(new_tuple)();

    s.start();

    return 0;
}
