//
// Created by ilesik on 9/30/24.
//

#pragma once
#include <src/module/module.hpp>

namespace gb {

class Discord_statistics_collector: public Module {
public:
  Discord_statistics_collector(const std::string& name, const std::vector<std::string>& dependencies) : Module(name,dependencies){};
};

  typedef std::shared_ptr<Discord_statistics_collector> Discord_statistics_collector_ptr;

} // gb
