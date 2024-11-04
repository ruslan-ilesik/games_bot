//
// Created by ilesik on 10/27/24.
//

#pragma once
#include <src/module/module.hpp>

namespace gb {

class Webserver: public Module {
public:
  Webserver(const std::string &name,
              const std::vector<std::string> &dependencies): Module(name,dependencies){};
};

} // gb
