add_library(discord_command_balance_transactions SHARED
        ../../../../../module/module.cpp
        ./discord_commands_balance_transactions_impl.cpp
        ../../discord_general_command.cpp
)


find_package(dpp REQUIRED CONFIG)

target_link_libraries(discord_command_balance_transactions PRIVATE dpp)

target_include_directories(discord_command_balance_transactions PRIVATE ${DPP_INCLUDE_DIR})