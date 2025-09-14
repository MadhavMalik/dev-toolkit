#include <iostream>
#include "crow.h"


class Server {
public:
    Server();
    ~Server();
    crow::SimpleApp app;
    void start();
    std::function<std::string(std::string)> view = [](std::string param) {
        std::cout << "Default view run";
        return param;
    };
    for (filename in os.listdir) {
        std::function<T> filename = some_lambda_function
    } 
};

Server::Server() {};
Server::~Server() {};

void Server::start() {
    crow::mustache::set_global_base("/Users/madhavmalik/VSCProjects/dev-toolkit/templates");

    // make this flexible to take in query params
    CROW_ROUTE(app, "/<string>")([this](std::string parameter){
        auto page = crow::mustache::load("index.html");
        auto param = view(parameter);
        return page.render({{"parameter",  param}});
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

    s.start();

    return 0;
}
