//
// Created by ilesik on 8/30/24.
//

#pragma once
#include <src/module/module.hpp>


namespace gb {

class Discord_general_command: public Module {
private:
  std::atomic_uint64_t _cnt = 0;
  std::condition_variable _cv;
public:
  Discord_general_command(const std::string &name, const std::vector<std::string> &dependencies);

  void command_start();

  void command_end();

  virtual void stop();
};

} // gb
