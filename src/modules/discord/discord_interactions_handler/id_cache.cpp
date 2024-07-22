//
// Created by ilesik on 7/21/24.
//

#include "./id_cache.hpp"

namespace gb {

    Id_cache::Id_cache() {
        static std::random_device _rd;
        _rng = std::mt19937(_rd());
    }

    std::vector<uint64_t> Id_cache::component_innit(dpp::message &m) {
        std::vector<uint64_t> occupied_ids;
        auto manage_id = [this, &occupied_ids](dpp::component &component) {
            std::unique_lock lk(_mutex);
            uint64_t id;
            do {
                id = _dist(_rng);
            } while (_ids_map.contains(id));

            _ids_map[id] = component.custom_id;
            occupied_ids.push_back(id);
            component.custom_id = std::to_string(id);
        };

        auto selectmenu = [this, &occupied_ids](dpp::component &component) {
            for (auto &i: component.options) {
                std::unique_lock lk(_mutex);
                uint64_t id;
                do {
                    id = _dist(_rng);
                } while (_ids_map.contains(id));
                _ids_map[id] = i.value;
                occupied_ids.push_back(id);
                i.value = std::to_string(id);
            }
        };

        for (auto &cmp: m.components) {
            if (cmp.type == dpp::cot_action_row) {
                for (auto &cmp2: cmp.components) {
                    if (cmp2.type == dpp::cot_selectmenu) {
                        selectmenu(cmp2);
                    }
                    manage_id(cmp2);
                }
            } else {
                if (cmp.type == dpp::cot_selectmenu) {
                    selectmenu(cmp);
                }
                manage_id(cmp);
            }
        }
        return occupied_ids;
    }

    void Id_cache::clear_ids(const std::vector<uint64_t> &ids) {
        std::unique_lock lk(_mutex);
        for (auto &i: ids) {
            _ids_map.erase(i);
        }
        _cv.notify_all();
    }

    void Id_cache::stop() {
        std::mutex m;
        std::unique_lock lk(m);
        _cv.wait(lk, [this]() { return _ids_map.empty(); });
    }

    std::string Id_cache::get_value(uint64_t key) {
        std::shared_lock lk(_mutex);
        return _ids_map.at(key);
    }

} //gb