//
// Created by ilesik on 10/1/24.
//

#pragma once
#include <src/module/module.hpp>

namespace gb {

  /**
   * @class Discord_reports
   * @brief Abstract base class for reporting Discord bot statistics.
   *
   * The `Discord_reports` class provides an abstract interface for classes responsible for submitting Discord bot
   * statistics, such as server and user counts, to external bot listing services. It inherits from the `Module` class
   * and requires implementation of reporting logic in derived classes.
   */
  class Discord_reports: public Module {
  public:
    /**
     * @brief Constructor for the `Discord_reports` class.
     *
     * Initializes the `Discord_reports` module with a given name and a list of dependencies required for
     * the reporting functionality.
     *
     * @param name The name of the module.
     * @param dependencies A list of dependencies required by the module (e.g., Discord bot, configuration, statistics collector).
     */
    Discord_reports(const std::string& name, const std::vector<std::string>& dependencies)
        : Module(name, dependencies) {};
  };

} // gb
