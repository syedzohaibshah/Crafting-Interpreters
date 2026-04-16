

#pragma once 
#include "LoxCallable.h"


#include <chrono>
class Interpreter;

class ClockFunction : public LoxCallable {
public:
    int arity() override {
        return 0;
    }

    Object call(std::shared_ptr<Interpreter> interpreter,
                const std::vector<Object>& arguments) override {

        using namespace std::chrono;

        auto now = system_clock::now();
        auto seconds = duration_cast<milliseconds>(now.time_since_epoch()).count() / 1000.0;

        return seconds; // double
    }

    std::string toString() const override {
        return "<native fn>";
    }
};