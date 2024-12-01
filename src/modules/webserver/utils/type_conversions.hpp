//
// Created by ilesik on 11/4/24.
//

#pragma once
#include <json/value.h>
#include <src/modules/achievements_processing/achievements_processing.hpp>
#include <src/modules/database/database.hpp>
#include <src/modules/discord/discord/discord_command.hpp>

/**
 * @brief Converts a Database_return_t object to a JSON value.
 *
 * @param data The `Database_return_t` object to convert.
 * @return A `Json::Value` representing the input object.
 */
Json::Value to_json(const gb::Database_return_t &data);

/**
 * @brief Converts a Command_data object to a JSON value.
 *
 * @param data The `Command_data` object to convert.
 * @return A `Json::Value` representing the input object.
 */
Json::Value to_json(const gb::Command_data &data);

/**
 * @brief Converts a Discord_command_ptr object to a JSON value.
 *
 * @param data The `Discord_command_ptr` object to convert.
 * @return A `Json::Value` representing the input object.
 */
Json::Value to_json(const gb::Discord_command_ptr &data);

/**
 * @brief Converts a slashcommand object to a JSON value.
 *
 * @param data The `dpp::slashcommand` object to convert.
 * @return A `Json::Value` representing the input object.
 */
Json::Value to_json(const dpp::slashcommand &data);

/**
 * @brief Converts a vector of integers to a JSON array.
 *
 * @param rows_cnt The vector of integers to convert.
 * @return A `Json::Value` array representing the input vector.
 */
Json::Value to_json(const std::vector<int> &rows_cnt);

/**
 * @brief Converts a vector of strings to a JSON array.
 *
 * @param order_by The vector of strings to convert.
 * @return A `Json::Value` array representing the input vector.
 */
Json::Value to_json(const std::vector<std::string> &order_by);

/**
 * @brief Converts an Achievements_report object to a JSON value.
 *
 * @param achievements_report The `Achievements_report` object to convert.
 * @return A `Json::Value` representing the input object.
 */
Json::Value to_json(const gb::Achievements_report &achievements_report);

/**
 * @brief Converts an Achievement object to a JSON value.
 *
 * @param achievements_report The `Achievement` object to convert.
 * @return A `Json::Value` representing the input object.
 */
Json::Value to_json(const gb::Achievement &achievements_report);
