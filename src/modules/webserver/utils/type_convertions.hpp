//
// Created by ilesik on 11/4/24.
//

#pragma once
#include <json/value.h>
#include <src/modules/database/database.hpp>

Json::Value to_json(const gb::Database_return_t & data);