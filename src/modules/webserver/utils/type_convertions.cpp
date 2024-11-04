//
// Created by ilesik on 11/4/24.
//

#include "type_convertions.hpp"

Json::Value to_json(const gb::Database_return_t & data) {
    Json::Value ja(Json::arrayValue);

    for (const auto& mapItem : data) {
        Json::Value jo(Json::objectValue);

        for (const auto& [key, value] : mapItem) {
            jo[key] = value;
        }

        ja.append(jo);
    }

    return ja;
}