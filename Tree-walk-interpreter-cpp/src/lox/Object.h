#pragma once

#include <variant>
#include <string>
#include <memory>

class LoxCallable; // forward declare

//(equivalent to java object) can hold one value from a fixed set of types, but only one at a time.

using Object = std::variant<
    std::monostate,
    double,
    std::string,
    bool,
    std::shared_ptr<LoxCallable>
>;
