// Goal: Enable event driven programming: execute functions (listeners) when an event is triggered
// Allows trigger blocking (listener functions run on the same thread) and trigger non-blocking (listener functions run on a new thread)

#include <iostream>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <nlohmann/json.hpp>
#include <thread>
#include "lambda_registry.hpp"

using json = nlohmann::json;

// template <typename... Args>
class Event {
public:
    Event() {};
    std::string name;
    LambdaMap listeners;

    // trigger blocking
    template <typename... Args>
    void trigger(Args&&... args) {
        for (auto listener : this->listeners) {
            listener.second->call(args...);
        }
    }

    // This is still a blocking implementation
    // TODO: use thread pool instead
    template <typename... Args>
    void triggerNonBlocking(Args&&... args) {
        std::vector<std::thread> threads; // Use std::jthread instead

        for (auto listener : this->listeners) {
            threads.emplace_back([l = listener.second, args...] {
                l->call(args...);
            });
        }

        // blocking
        for (auto& t : threads) {
            if (t.joinable()) t.join();
        }
    }

    void addListener(std::string name, auto& listener) {
        listeners.emplace(name, listener);
    }
};

// int main() {
//     Event readData;
//     View<json, int> onReadData = [] (int sensorValue) {
//         std::cout << "Running onReadData with sensorValue: " << sensorValue << std::endl;
//         return json({{"Result", sensorValue/2}});
//     };
//     readData.addListener("onReadData", onReadData);
//     readData.trigger(42); // change this to globalEvents 
//     // readData.triggerNonBlocking(42); 

//     return 0;
// }