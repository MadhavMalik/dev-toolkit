// Goal: Enable event driven programming: execute functions (listeners) when an event is triggered
// Allows trigger blocking (listener functions run on the same thread) and trigger non-blocking (listener functions run on a new thread)

#include <iostream>
#include <vector>
#include <map>
#include <memory>
#include <functional>

class baseListener {
public:
    virtual std::vector<std::pair<std::string, std::string>> call() = 0;
};

// Very restricted right now, doesn't allow arguments for listeners
class Listener : public baseListener {
public:
    Listener();
    Listener(auto listener) {
        this->listener = std::function<void()>(listener);
    }
    std::vector<std::pair<std::string, std::string>> call() override {
        listener();
        return std::vector<std::pair<std::string, std::string>>();
    }
private:
    std::function<void()> listener;
};

class Event {
public:
    Event() {};
    std::string name;
    std::map<std::string, std::shared_ptr<baseListener>> listeners;

    // trigger blocking
    void trigger() {
        for (auto listener : this->listeners) {
            listener.second->call();
        }
    }

    void addListener(std::string name, std::shared_ptr<baseListener> listener) {
        listeners.emplace(name, listener);
    }
};

int main() {
    Event readData;
    auto onReadData = [] () {
        std::cout << "Running onReadData" << std::endl;
    };
    readData.addListener("onReadData", std::make_shared<Listener>(onReadData));
    readData.trigger(); // change this to globalEvents 
    return 0;
}