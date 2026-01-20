# dev-toolkit

Internal tooling to speed up development and deployment.

## Features

- **Lambda Registry** - store and call lambda functions with different signatures in a single map
- **Events** - event-driven programming with blocking and non-blocking triggers
- **Mini Crow Server** - a way for external software to invoke functions or communicate with my C++ code via POST/GET requests

## Lambda Registry

### The Problem

In C++ lambda functions cannot be stored in a vector or map. Even if two lambdas have the exact same signature, their types are fixed at compile time and each lambda gets its own unique type:

```cpp
auto add = [](int a, int b) { return a + b; };
auto multiply = [](int a, int b) { return a * b; };

std::vector<???> functions = {add, multiply}; // what is the type that goes in this vector?
```

Easy solution: Wrap them in `std::function`, but then you can only store lambdas with the same signature. I wanted to create a way to store lambdas with different signatures in the same container, and call them by name at runtime.

### The Solution

The tricky part is that C++ needs to know types at compile time, but we want to store functions with different types in the same container at runtime.

My Implementation: I created a `BaseView` class and a templated `View<R, Args...>` class that inherits from it. `View` is templated with the return type and argument types - this is where the actual lambda gets stored. Since all views inherit from `BaseView`, we can store pointers to them in a `std::map<std::string, std::shared_ptr<BaseView>>`.

When you call a view, the arguments go through the base class interface, get converted to the right types using the template info stored in the derived `View`, and then the lambda executes with proper type safety.

```cpp
View<json, int, int> sum = [] (int i, int j) {
    std::cout << "Sum: " << i + j << std::endl;
    return json({{"Result", i + j}});
};

View<json, int, int, int> product = [] (int i, int j, int k) {
    std::cout << "Product: " << i * j * k << std::endl;
    return json({{"Result", i * j * k}});
};

// store lambdas in a map-like structure
LambdaMap lm;
lm.emplace("sum", sum);
lm.emplace("product", product);

json sumResult = lm["sum"]->call(3, 5);
json productResult = lm["product"]->call(3, 5, 8);
```

## Events

While working with C++, I have often wanted different parts of my code to react when something happens (e.g. reading a value from a sensor), without tightly coupling everything together. This can be done with event-driven programming. My implementation for supporting events and listeners is built on top of the lambda registry - as it allows registering/storing listeners to events and triggering them all at once.

```cpp
Event readData;

View<json, int> onReadData = [] (int sensorValue) {
    std::cout << "Running onReadData with sensorValue: " << sensorValue << std::endl;
    return json({{"Result", sensorValue/2}});
};

readData.addListener("onReadData", onReadData);
readData.trigger(42);
// readData.triggerNonBlocking(42);
```

`trigger()` runs listeners on the calling thread. `triggerNonBlocking()` spawns a thread for each listener.

## Upcoming Features

- **Thread Pool** - right now `triggerNonBlocking()` just spawns threads manually, which is not a good idea. I am going to build a thread pool that can handle task scheduling.
- **Async Loggers** - non-blocking logging with different debug levels so logs don't slow things down.
- **Automatic Docker Deployment** - run a single command and it sets up a C++ project with this internal toolkit, with boilerplate code ready to go, all in a docker container. This removes all setup/integration time. 

## Building

```bash
./build.sh
```

Dependencies: nlohmann/json, Crow
