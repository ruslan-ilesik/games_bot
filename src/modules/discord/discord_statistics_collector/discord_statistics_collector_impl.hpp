//
// Created by ilesik on 9/30/24.
//

#pragma once
#include <src/modules/config/config.hpp>
#include <src/modules/database/database.hpp>
#include <src/modules/discord/discord_bot/discord_bot.hpp>


#include "./discord_statistics_collector.hpp"
namespace gb {

class Discord_statistics_collector_impl: public Discord_statistics_collector {
  Config_ptr _config;
  Discord_bot_ptr _bot;
  Database_ptr _db;

  unsigned long _on_guild_create_handler;
  unsigned long _on_guild_delete_handler;
  unsigned long _on_guild_member_add_handler;
  unsigned long _on_guild_member_remove_handler;

  Prepared_statement _add_guild_stmt;
  Prepared_statement _remove_guild_stmt;
  Prepared_statement _add_user_stmt;
  Prepared_statement _remove_user_stmt;

  dpp::timer _report_timer;
  Prepared_statement _historic_report_stmt;

public:
  Discord_statistics_collector_impl();

  void run() override;
  void stop() override;
  void init(const Modules &modules) override;
};

extern "C" Module_ptr create();

} // gb
