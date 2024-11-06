//
// Created by ilesik on 11/4/24.
//

#include "type_conversions.hpp"

Json::Value to_json(const gb::Database_return_t &data) {
    Json::Value ja(Json::arrayValue);

    for (const auto &mapItem: data) {
        Json::Value jo(Json::objectValue);

        for (const auto &[key, value]: mapItem) {
            jo[key] = value;
        }

        ja.append(jo);
    }

    return ja;
}

Json::Value to_json(const gb::Command_data &data) {
    Json::Value value;
    value["help_text"] = data.help_text;
    Json::Value categories = Json::Value(Json::arrayValue);
    for (auto &i: data.category) {
        categories.append(i);
    }
    value["categories"] = categories;
    return value;
}


Json::Value to_json(const gb::Discord_command_ptr &data) {
    Json::Value value = to_json(data->get_command_data());
    value["command"] = to_json(data->get_command());

    return value;
}

Json::Value to_json(const dpp::slashcommand &data) {
    auto command_option_type_to_string = [](dpp::command_option_type type) -> std::string {
        switch (type) {
            case dpp::co_sub_command: return "sub_command";
            case dpp::co_sub_command_group: return "sub_command_group";
            case dpp::co_string: return "string";
            case dpp::co_integer: return "integer";
            case dpp::co_boolean: return "boolean";
            case dpp::co_user: return "user";
            case dpp::co_channel: return "channel";
            case dpp::co_role: return "role";
            case dpp::co_mentionable: return "mentionable";
            case dpp::co_number: return "number";
            case dpp::co_attachment: return "attachment";
            default: throw std::invalid_argument("Invalid command_option_type value");
        }
    };


    std::function<Json::Value(const dpp::command_option &)> json_option = [&](const dpp::command_option &option) {
        Json::Value option_value{};
        option_value["name"] = option.name;
        option_value["description"] = option.description;
        option_value["type"] = command_option_type_to_string(option.type);
        option_value["required"] = option.required;
        option_value["options"] = Json::Value(Json::arrayValue);
        for (const auto &sub_option: option.options) {
            option_value["options"].append(json_option(sub_option));
        }
        return option_value;
    };

    Json::Value value;
    value["name"] = data.name;
    value["description"] = data.description;
    value["options"] = Json::Value(Json::arrayValue);
    for (const auto &option: data.options) {
        value["options"].append(json_option(option));
    }

    return value;
}
