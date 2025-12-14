// Simple Crow server using lambda_registry for route handlers

#include <iostream>

#include "crow.h"
#include "lambda_registry.cpp"


class ServerApp {
public:
    ServerApp();
    ~ServerApp();
    crow::SimpleApp app;
    LambdaMap views;
    void start();
};

ServerApp::ServerApp() {};
ServerApp::~ServerApp() {};

void ServerApp::start() {
    crow::mustache::set_global_base("/Users/madhavmalik/VSCProjects/dev-toolkit/templates"); //turn this into a relative path

    // TODO: make the handler registry dynamic
    CROW_ROUTE(app, "/<string>/<string>")([this](std::string a, std::string b){
        int i = std::stoi(a);
        int j = std::stoi(b);
        json result = views["handler"]->call({i, j});
        auto page = crow::mustache::load("server.html");
        crow::json::wvalue context = crow::json::load(result.dump());
        return page.render(context);
    });

    app.port(18080).multithreaded().run();
}

int main() {
    ServerApp server;

    View<json, int, int> handler = [] (int i, int j) {
        std::cout << "Handler called with: " << i << ", " << j << std::endl;
        std::cout << "Sum: " << i + j << std::endl;
        return json({{"a", i}, {"b", j}, {"sum", i + j}});
    };

    server.views.emplace("handler", handler);
    server.start();
    return 0;
}
