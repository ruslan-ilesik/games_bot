//
// Created by ilesik on 10/3/24.
//

#pragma once
#include <src/modules/discord/premium_manager/premium_manager.hpp>


#include "./discord_hangman_command.hpp"
namespace gb {

class Discord_hangman_command_impl: public Discord_hangman_command{
  Premium_manager_ptr _premium;
public:
  Discord_hangman_command_impl();

  virtual ~Discord_hangman_command_impl() = default;

  void run() override;
  void stop() override;
  void init(const Modules &modules) override;
};


  extern "C" Module_ptr create();
} // gb
