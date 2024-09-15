//
// Created by ilesik on 9/15/24.
//

#pragma once
#include "./discord_chess_command.hpp"

namespace gb {

class Discord_chess_command_impl: public Discord_chess_command {
public:
  Discord_chess_command_impl();

  void run() override;

  void stop() override;

  void init(const Modules &modules) override;
};

  extern "C" Module_ptr create();

} // gb
