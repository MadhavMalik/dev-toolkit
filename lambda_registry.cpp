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
    virtual std::vector<std::pair<std::string, std::string>> call(std::vector<std::any> args) = 0;

    std::vector<std::any> args = {};
    virtual void setArguments(std::vector<std::any> args) = 0;
};

template<typename R, typename... Args>
class View : public BaseView{
public:
    friend class LambdaMap;

    View() {};
    View(const auto& func) {
        this->func = std::function<R(Args...)>(func);
        // this->funcPtr = std::make_shared<std::function<R(Args...)>>(func);
        // this->funcPtr = std::make_shared<View>(func);
    }

    std::vector<std::pair<std::string, std::string>> call () override {
        std::apply(func, arguments);
        return std::vector<std::pair<std::string, std::string>>();
    }

    std::vector<std::pair<std::string, std::string>> call(std::vector<std::any> args) {
        setArguments(args);
        std::apply(func, arguments);
        // TODO: Clear arguments to avoid reuse in future calls, i.e. view->call();
        return std::vector<std::pair<std::string, std::string>>();
    }

    void operator=(auto func) { this->func = std::function<R(Args...)>(func); }

private:
    std::function<R(Args...)> func;
    std::shared_ptr<View> viewPtr;
    // std::shared_ptr<std::function<R(Args...)>> funcPtr;
    // std::shared_ptr<View> funcPtr;

    std::tuple<Args...> arguments;

    template <size_t... Is>
    std::tuple<Args...> unpackArguments(std::vector<std::any>& args, std::index_sequence<Is...>) {
        return std::make_tuple(std::any_cast<Args>(args[Is])...);
    }

    void setArguments(std::vector<std::any> args) override {
        arguments = unpackArguments(args, std::index_sequence_for<Args...>{});
    }

    void setViewPtr () {
        viewPtr = std::make_shared<View>(func);
    }
};

// std::vector<std::shared_ptr<BaseView>> lambdas = {};

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
            std::cerr << "ERROR: View not found";
        }
        return nullptr;
    }
private:
    std::map<std::string, std::shared_ptr<BaseView>> lambdas = {}; //Might change this to std::unordered_map
};


int main() {

    // Return type of lambda functions is still restricted
    // TODO: Make the return type configurable
    using RET = std::vector<std::pair<std::string, std::string>>;

    View<RET, int, int> sum = [] (int i, int j) {
        std::cout << "Sum: " << i + j << std::endl;
        int result = i + j;
        return RET({{"Result", std::to_string(result)}});
    };

    View<RET, int, int, int> mult = [] (int i, int j, int k) {
        std::cout << "Multiplication: " << i * j * k << std::endl;
        int result = i * j * k;
        return RET({{"Result", std::to_string(result)}});
    };

    LambdaMap lm;
    lm.emplace("sum", sum);
    lm.emplace("mult", mult);

    lm["sum"]->call({3, 5});
    lm["mult"]->call({3, 4, 5});
}