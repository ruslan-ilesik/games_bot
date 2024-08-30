//
// Created by ilesik on 8/28/24.
//

#pragma once
#include <nlohmann/json.hpp>
#include <src/modules/admin_terminal/admin_terminal.hpp>
#include <src/modules/database/database.hpp>

#include "./achievements_processing.hpp"


namespace gb {

class Achievements_processing_impl: public Achievements_processing {

  Admin_terminal_ptr _admin_terminal;

  Database_ptr _db;

  Prepared_statement _activate_achievement_stmt;

  Prepared_statement _check_achievement_stmt;

  std::shared_mutex _mutex;

  std::map<std::string, Achievement> _achievements;

  void load_from_file();

public:
  Achievements_processing_impl();

  void stop() override;

  void run() override;

  void init(const Modules &modules) override;

  Achievement activate_achievement(const std::string& name, const std::string& user_id) override;

  bool is_have_achievement(const std::string& name, const std::string& user_id) override;
};

extern "C" Module_ptr create();

} // gb
