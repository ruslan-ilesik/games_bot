//
// Created by ilesik on 7/20/24.
//

#include "discord_command_help_impl.hpp"

namespace gb {
    std::map<std::string, std::string> category_emojis = {
        {"Other", "🔄"},
        {"Game",  "🎮"}
    };


    void Discord_command_help_impl::innit(const Modules &modules) {
        this->_discord_bot = std::static_pointer_cast<Discord_bot>(modules.at("discord_bot"));
        this->_command_handler = std::static_pointer_cast<Discord_command_handler>(
            modules.at("discord_command_handler"));
        auto discord = std::static_pointer_cast<Discord>(modules.at("discord"));
        _select_menu_handler = std::static_pointer_cast<Discord_select_menu_handler>(
            modules.at("discord_select_menu_handler"));
        _discord_bot->add_pre_requirement([this, discord]() {
            dpp::slashcommand command("help", "Command to get help about commands", _discord_bot->get_bot()->me.id);

            _command_handler->register_command(discord->create_discord_command(
                command,
                [this](const dpp::slashcommand_t &event) -> dpp::task<void> {
                    std::shared_lock<std::shared_mutex> lock(_mutex);
                    co_await help_command(event);
                    co_return;
                },
                {
                    "Help command provides you ability to get information about every command in bot sorted by categories.",
                    {"other"}
                }
            ));
        });
    }

    void Discord_command_help_impl::stop() {
        _command_handler->remove_command("help");
        //wait until all running executions of command will stop;
        std::unique_lock<std::shared_mutex> lock(_mutex);
    }

    Discord_command_help_impl::Discord_command_help_impl() : Discord_command_help("discord_command_help",
                                                                                  {"discord_command_handler",
                                                                                   "discord_bot", "discord",
                                                                                   "discord_select_menu_handler"}) {}

    void Discord_command_help_impl::run() {}

    dpp::task<void> Discord_command_help_impl::help_command(const dpp::slashcommand_t &event) {
        std::shared_lock lock(_mutex);
        auto coma_sep = [](std::string a, std::string b) {
            return std::move(a) + ", " + std::move(b);
        };

        auto commands = _command_handler->get_commands();
        std::map<std::string, std::vector<Discord_command_ptr>> categorised;
        for (auto &[k, v]: commands) {
            for (auto cat: v->get_command_data().category) {
                cat[0] = toupper(cat[0]);
                categorised[cat].push_back(v);
            }
        }

        dpp::embed embed;
        embed.set_title("HELP").set_description("Choose category below.").set_color(dpp::colours::blue);

        dpp::component comp;
        comp.set_type(dpp::cot_selectmenu).set_placeholder("Choose category").set_id("help");

        for (auto &[k, v]: categorised) {


            auto tmp = std::ranges::transform_view(v,
                                                   [](const Discord_command_ptr &command) { return command->get_name(); });
            std::string s = std::accumulate(std::next(tmp.begin()), tmp.end(),
                                            tmp[0], // start with first element
                                            coma_sep);
            auto emoji = category_emojis[k];
            embed.add_field((emoji != "" ? emoji + "| " : "") + k, s);

            dpp::select_option option{k, k};
            if (emoji != "") {
                option.set_emoji(emoji);
            }
            comp.add_select_option(option);

        }
        dpp::message m;
        m.add_embed(embed).add_component(dpp::component().add_component(comp));

        auto task = _select_menu_handler->wait_for(m, {event.command.usr.id}, 600);
        _discord_bot->reply(event, m);
        Select_menu_return select = co_await task;
        if (select.second) {
            co_return;
        }
        auto select_cat = select.first;

        //select command
        dpp::message m2;
        dpp::embed commands_embed;
        commands_embed.set_title("HELP").set_description(
            std::format("Category: {}\nChoose command below.", select_cat.values[0])).set_color(dpp::colours::blue);
        dpp::component commands_comp;
        commands_comp.set_type(dpp::cot_selectmenu).set_placeholder("Choose command").set_id("help_command");

        std::map<std::string, Discord_command_ptr> commands_of_selected_category;
        //show specific category
        for (auto &i: categorised[select_cat.values[0]]) {
            commands_embed.add_field(i->get_name(), i->get_command().description);
            commands_comp.add_select_option({i->get_name(), i->get_name()});
            commands_of_selected_category[i->get_name()] = i;
        }
        m2.add_embed(commands_embed).add_component(dpp::component().add_component(commands_comp));
        categorised.clear(); // no more need for all commands
        task = _select_menu_handler->wait_for(m2, {event.command.usr.id}, 600);
        _discord_bot->reply(select_cat, m2);
        select = co_await task;
        if (select.second) {
            co_return;
        }
        select_cat = select.first;


        dpp::message m3;
        dpp::embed command_embed;
        command_embed.set_title("HELP").set_description(
            std::format("Command information.", select_cat.values[0])).set_color(dpp::colours::blue);
        auto &command = commands_of_selected_category[select_cat.values[0]];


        auto cat = command->get_command_data().category;
        std::string s = std::accumulate(std::next(cat.begin()), cat.end(),
                                        cat[0], // start with first element
                                        coma_sep);
        command_embed.add_field(select_cat.values[0],
                                std::format("Description: {}\n\nDetailed: {}\n\nCategories: **{}**",
                                            command->get_command().description, command->get_command_data().help_text,
                                            s));
        if (!command->get_command().options.empty()) {
            command_embed.add_field("Arguments:", "");
        } else {
            command_embed.add_field("This command has no arguments:", "");
        }

        for (auto &i: command->get_command().options) {
            if (i.options.empty()) {
                command_embed.add_field(i.name, std::format("{}\n Mandatory: {}", i.description, i.required));
            }
        }
        m3.add_embed(command_embed);
        _discord_bot->reply(select_cat, m3);
        co_return;
    }

    Module_ptr create() {
        return std::dynamic_pointer_cast<Module>(std::make_shared<Discord_command_help_impl>());
    }
} // gb