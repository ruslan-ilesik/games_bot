//
// Created by ilesik on 8/29/24.
//

#pragma once
#include <dpp/dpp.h>
#include <src/module/module.hpp>

namespace gb{


class Discord_achievements_processing : public Module{
public:
  Discord_achievements_processing(const std::string &name,
                const std::vector<std::string> &dependencies)
        : Module(name, dependencies){};

  virtual void activate_achievement(const std::string& name, const dpp::snowflake& user_id, const dpp::snowflake& channel_id) = 0;

  virtual bool is_have_achievement(const std::string& name, const dpp::snowflake& user_id) = 0;
};

  typedef std::shared_ptr<Discord_achievements_processing> Discord_achievements_processing_ptr;

} //gb

