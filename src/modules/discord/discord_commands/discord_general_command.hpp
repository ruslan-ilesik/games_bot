//
// Created by ilesik on 8/30/24.
//

#pragma once
#include <src/module/module.hpp>

namespace gb {

  /**
   * @class Discord_general_command
   * @brief A module that manages general commands in a Discord environment.
   *
   * This class provides functionality to handle the start and end of command execution,
   * ensuring proper synchronization and resource management.
   */
  class Discord_general_command: public Module {
  private:
    /**
     * @brief Counter to keep track of the number of currently active commands.
     */
    std::atomic_uint64_t _cnt = 0;

    /**
     * @brief Condition variable to manage synchronization for command execution.
     */
    std::condition_variable _cv;

  public:
    /**
     * @brief Constructs a Discord_general_command object.
     *
     * @param name The name of the module.
     * @param dependencies A list of module names that this module depends on.
     */
    Discord_general_command(const std::string &name, const std::vector<std::string> &dependencies);

    /**
     * @brief Increments the command counter to signify the start of a command.
     */
    void command_start();

    /**
     * @brief Decrements the command counter and notifies all waiting threads.
     */
    void command_end();

    /**
     * @brief Stops the module, waiting for all active commands to finish.
     */
    virtual void stop();
  };

} // gb
