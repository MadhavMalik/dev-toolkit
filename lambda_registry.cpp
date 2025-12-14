// A type-safe dynamic function registry for C++ (map/vector of lambda functions)
// Provides a unified interface for registering and triggering strongly-typed functions at runtime.

// TODO: Integrate with toolkit event system
// Currently enforces all handlers to return nlohmann::json

#include <iostream>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <any>

class BaseView {
public:
    virtual json call() = 0;
    virtual json call(std::vector<std::any> args) = 0;
};

template<typename R, typename... Args>
class View : public BaseView{
public:
    friend class LambdaMap;

    View() {};
    View(const auto& func) {
        this->func = std::function<R(Args...)>(func);
    }

    json call() override {
        return std::apply(func, arguments);
    }

    json call(std::vector<std::any> args) override {
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
    std::tuple<Args...> unpackArguments(std::vector<std::any>& args, std::index_sequence<Is...>) {
        return std::make_tuple(std::any_cast<Args>(args[Is])...);
    }

    void setArguments(std::vector<std::any> args) {
        arguments = unpackArguments(args, std::index_sequence_for<Args...>{});
    }

    void setViewPtr () {
        viewPtr = std::make_shared<View>(func);
    }
};

class LambdaMap {
public:
    LambdaMap() {};

    void emplace(std::string name, auto& view) {
        view.setViewPtr();
        lambdas.emplace(name, view.viewPtr);
    };

    std::shared_ptr<BaseView> operator[] (std::string key) {
        if (lambdas.find(key) != lambdas.end()){
            return lambdas[key];
        } else {
            std::cerr << "(core) ERROR: View not found";
        }
        return nullptr;
    }
private:
    std::map<std::string, std::shared_ptr<BaseView>> lambdas = {}; //Might change this to std::unordered_map
};


// int main() {

//     View<json, int, int> sum = [] (int i, int j) {
//         std::cout << "Sum: " << i + j << std::endl;
//         int result = i + j;
//         return json({{"Result", result}});
//     };

//     View<json, int, int, int> mult = [] (int i, int j, int k) {
//         std::cout << "Multiplication: " << i * j * k << std::endl;
//         int result = i * j * k;
//         return json({{"Result", result}});
//     };

//     LambdaMap lm;
//     lm.emplace("sum", sum);
//     lm.emplace("mult", mult);

//     json result = lm["sum"]->call({3, 5});
//     lm["mult"]->call({3, 4, 5});
// }
