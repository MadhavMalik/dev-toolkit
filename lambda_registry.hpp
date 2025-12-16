// A type-safe dynamic function registry for C++ (map/vector of lambda functions)
// Provides a unified interface for registering and triggering strongly-typed functions at runtime.

// TODO: Integrate with toolkit event system

// Currently enforces all handlers to return nlohmann::json

#pragma once

#include <iostream>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <any>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

// how the lambda registry library works:
// LambdaMap holds a map of pointers to BaseView objects
// View inherits from BaseView, and is templated with specific return and arguments types
// View stores the actual function, and overrides the BaseView's callImplementation function
// BaseView's call() packs the arguments and forwards them to View's callImplementation

class BaseView {
public:
    // now BaseView has a virtual function callImplementation, that is overwritten by View
    // this allows me to pack args in a vector before calling the function, instead of caller
    // packing them
    template<typename... Args>
    json call(Args&&... args) {
        std::vector<std::any> vectorArgs {std::any(std::forward<Args>(args))...}; // std::forward allows forwarding/passing templated arguments
        return callImplementation(vectorArgs);
    }
    // TODO: explore if I just call the function without packing and unpacking (in View class) the arguments

protected:
    virtual json callImplementation(std::vector<std::any>& args) = 0;
};

template<typename R, typename... Args>
class View : public BaseView{
public:
    friend class LambdaMap;

    View() {};
    View(const auto& func) {
        this->func = std::function<R(Args...)>(func);
    }

    json callImplementation(std::vector<std::any>& args) override {
        setArguments(args);
        // TODO: Clear arguments to avoid reuse in future calls, i.e. view->call();
        return std::apply(func, arguments);
    }

    void operator=(auto func) { this->func = std::function<R(Args...)>(func); }

private:
    std::function<R(Args...)> func;
    std::shared_ptr<View> viewPtr;
    std::tuple<Args...> arguments;

    template <size_t... Is>
    // std::index_sequence<Is...> generates compile-time integer sequence <0, 1, 2 ...>
    std::tuple<Args...> unpackArguments(std::vector<std::any>& args, std::index_sequence<Is...>) {
        return std::make_tuple(std::any_cast<Args>(args[Is])...);
    }

    void setArguments(std::vector<std::any>& args) {
        arguments = unpackArguments(args, std::index_sequence_for<Args...>{});
    }

    void setViewPtr () {
        viewPtr = std::make_shared<View>(func);
    }
};

class LambdaMap {
public:
    LambdaMap() {};

    void emplace(std::string& name, auto& view) {
        view.setViewPtr();
        lambdas.emplace(name, view.viewPtr);
    };

    auto begin() { return lambdas.begin(); }
    auto end() { return lambdas.end(); }

    std::shared_ptr<BaseView> operator[] (std::string key) {
        auto it = lambdas.find(key);
        if (it != lambdas.end()){
            return it->second;
        } else {
            std::cerr << "(core) ERROR: View not found";
        }
        return nullptr;
    }
private:
    std::map<std::string, std::shared_ptr<BaseView>> lambdas = {}; //Might change this to std::unordered_map
};

// Usage:
//
//     View<json, int, int> sum = [] (int i, int j) {
//         std::cout << "Sum: " << i + j << std::endl;
//         return json({{"Result", i + j}});
//     };
//
//     LambdaMap lm;
//     lm.emplace("sum", sum);
//
//     json result = lm["sum"]->call(3, 5);
