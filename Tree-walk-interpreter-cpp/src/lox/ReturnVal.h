#pragma once
#include "Object.h"


class ReturnVal : public std::runtime_error {
public:
  const Object value;

  ReturnVal(const Object value,std::string message)
                  :std::runtime_error(message),value(value) {}

};
