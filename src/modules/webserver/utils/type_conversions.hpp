//
// Created by ilesik on 11/4/24.
//

#pragma once
#include <json/value.h>
#include <src/modules/achievements_processing/achievements_processing.hpp>
#include <src/modules/database/database.hpp>
#include <src/modules/discord/discord/discord_command.hpp>

Json::Value to_json(const gb::Database_return_t & data);

Json::Value to_json(const gb::Command_data& data);

Json::Value to_json(const gb::Discord_command_ptr& data);

Json::Value to_json(const dpp::slashcommand& data);

Json::Value to_json(const std::vector<int> &rows_cnt);

Json::Value to_json(const std::vector<std::string> &order_by);

Json::Value to_json(const gb::Achievements_report & achievements_report);

Json::Value to_json(const gb::Achievement & achievements_report);