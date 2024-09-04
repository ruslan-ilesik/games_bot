//
// Created by ilesik on 9/4/24.
//

#pragma once

#include "./discord_dominoes_command.hpp"

namespace gb {

class Discord_dominoes_command_impl: public Discord_dominoes_command {
public:
  Discord_dominoes_command_impl();

  void init(const Modules &modules) override;
  void stop() override;
  void run() override;
};

  extern "C" Module_ptr create();

} // gb
