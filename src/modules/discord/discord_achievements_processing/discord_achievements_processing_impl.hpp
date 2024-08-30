//
// Created by ilesik on 8/29/24.
//

#pragma once

#include <src/modules/achievements_processing/achievements_processing.hpp>
#include <src/modules/discord/discord_bot/discord_bot.hpp>


#include "./discord_achievements_processing.hpp"

namespace gb {

class Discord_achievements_processing_impl : public Discord_achievements_processing {

  Achievements_processing_ptr _achievements_processing;

  Discord_bot_ptr _bot;

public:
  Discord_achievements_processing_impl();

  void stop() override;

  void run() override;

  void init(const Modules &modules) override;

  bool is_have_achievement(const std::string &name, const dpp::snowflake &user_id) override;

  void activate_achievement(const std::string& name, const dpp::snowflake& user_id, const dpp::snowflake& channel_id) override;
};

extern "C" Module_ptr create() {
  return std::dynamic_pointer_cast<Module>(std::make_shared<Discord_achievements_processing_impl>());
}

} // gb
