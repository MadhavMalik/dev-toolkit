// A type-safe dynamic function registry for C++ (map/vector of lambda functions)
// Provides a unified interface for registering and triggering strongly-typed functions at runtime.

// TODO: Integrate with Crow routes and toolkit event system

#include <iostream>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <any>

class BaseView {
public:
    virtual std::vector<std::pair<std::string, std::string>> call() = 0;
    std::vector<std::any> args = {};
    virtual void setArguments(std::vector<std::any> args) = 0;
};

template<typename R, typename... Args>
class View : public BaseView{
public:
    View() {};
    View(auto func) {
        this->func = std::function<R(Args...)>(func);
    }
    void setArguments(std::vector<std::any> args) override {
        arguments = unpackArguments(args, std::index_sequence_for<Args...>{});
    }
    std::vector<std::pair<std::string, std::string>> call () override {
        // func(1, 2);
        std::apply(func, arguments);
        return std::vector<std::pair<std::string, std::string>>();
    }

private:
    std::function<R(Args...)> func;
    std::tuple<Args...> arguments;

    template <size_t... Is>
    std::tuple<Args...> unpackArguments(std::vector<std::any>& args, std::index_sequence<Is...>) {
        return std::make_tuple(std::any_cast<Args>(args[Is])...);
    }

};

// std::vector<std::shared_ptr<BaseView>> lambdas = {};
std::map<std::string, std::shared_ptr<BaseView>> lambdas = {}; //Might change this to std::unordered_map

// Interface needs to be abstracted for easy integration
int main() {
    using RET = std::vector<std::pair<std::string, std::string>>;

    //   Lambda Syntax:
    //   std::make_shared<View<RET, argument_types...>>(
    //       [capture_list](argument_declarations) {
    //           // function body
    //           return RET({ { "key", "value" }, ... });
    //       }
    //   );

    //   Examples:
    std::shared_ptr<View<RET, int, int>> sum;
    sum = std::make_shared<View<RET, int, int>>([] (int i, int j) {
        std::cout << "Sum: " << i + j << std::endl;
        int result = i + j;

        return RET({{"Result", std::to_string(result)}});
    });

    std::shared_ptr<View<RET, int, int, int>> mult;
    mult = std::make_shared<View<RET, int, int, int>>([] (int i, int j, int k) {
        std::cout << "Multiplication: " << i * j  * k << std::endl;
        int result = i * j  * k;

        return RET({{"Result", std::to_string(result)}});
    });
    
    // Register functions
    lambdas.emplace("sum", sum);
    lambdas.emplace("mult", mult);

    // Call functions
    lambdas["sum"]->setArguments({std::any(3), std::any(4)});
    lambdas["sum"]->call();

    lambdas["mult"]->setArguments({std::any(3), std::any(4), std::any(5)});
    lambdas["mult"]->call();
}