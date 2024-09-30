//
// Created by ilesik on 9/29/24.
//

#pragma once
#include "./discord_2048_command.hpp"

namespace gb {

class Discord_2048_command_impl: public  Discord_2048_command{
public:
  Discord_2048_command_impl();

  void init(const Modules &modules) override;
  void run() override;
  void stop() override;
};


  extern "C" Module_ptr create();

} // gb
