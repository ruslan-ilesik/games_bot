//
// Created by ilesik on 7/21/24.
//

#pragma once
#include <random>
#include <shared_mutex>
#include <dpp/dpp.h>

namespace gb {

    /**
     * @brief A class to manage unique component IDs for DPP messages.
     *
     * The Id_cache class generates unique IDs for DPP message components, ensures
     * that they do not collide, and maintains a mapping from the generated IDs
     * to the original component custom IDs. It also provides functionality to
     * clear these IDs when they are no longer needed.
     */
    class Id_cache {
    protected:
        /// Map to store the association between generated IDs and custom component IDs.
        std::map<uint64_t, std::string> _ids_map;

        /// Random number generator.
        std::mt19937 _rng;

        /// Uniform distribution for generating random IDs.
        std::uniform_int_distribution<std::mt19937::result_type> _dist{0, std::numeric_limits<uint64_t>::max()};

        /// Mutex for thread-safe access to _ids_map.
        std::shared_mutex _mutex;

        /// Condition variable to notify when _ids_map is empty.
        std::condition_variable _cv;

    public:
        /**
         * @brief Constructor initializes the random number generator.
         */
        Id_cache();

        /**
         * @brief Destructor for Id_cache.
         */
        virtual ~Id_cache() = default;

        /**
         * @brief Initializes the component IDs for a given DPP message.
         *
         * This function generates unique IDs for each component in the message
         * and stores the mapping in _ids_map.
         *
         * @param m The DPP message whose components need IDs.
         * @return A vector of generated IDs that were assigned to the components.
         */
        std::vector<uint64_t> component_innit(dpp::message& m);

        /**
         * @brief Clears the stored IDs from the map.
         *
         * This function removes the specified IDs from the _ids_map, indicating
         * that they are no longer in use.
         *
         * @param ids A vector of IDs to be cleared.
         */
        void clear_ids(const std::vector<uint64_t>& ids);

        /**
         * @brief Waits for the ID map to be empty before proceeding.
         *
         * This function blocks until all IDs in the _ids_map have been cleared.
         */
        void stop();

        /**
         * @brief Retrieves the custom component ID associated with a given generated ID.
         *
         * @param key The generated ID for which to retrieve the custom component ID.
         * @return The custom component ID associated with the given generated ID.
         */
        std::string get_value(uint64_t key);
    };

} // gb
