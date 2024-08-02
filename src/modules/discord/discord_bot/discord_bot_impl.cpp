//
// Created by ilesik on 7/9/24.
//

#include "discord_bot_impl.hpp"

namespace gb {

    Discord_bot_impl::Discord_bot_impl() : Discord_bot("discord_bot", {"config"}) {}

    Discord_bot_impl::~Discord_bot_impl() {
        // Memory cleanup to prevent memory leak, originally created in init method.
        delete _bot;
    }

    void Discord_bot_impl::innit(const Modules &modules) {
        _config = std::static_pointer_cast<Config>(modules.at("config"));
    }

    void Discord_bot_impl::run() {
        if (_bot != nullptr) {
            throw std::runtime_error("Bot variable is not nullptr, memory leak possible");
        }
        _bot = new Discord_cluster(_config->get_value("discord_bot_token"));

        // Run all pre-requirements.
        {
            std::unique_lock<std::mutex> lock(_mutex);
            for (auto &i: _pre_requirements) {
                i();
            }
        }
        _bot->start(dpp::st_return);
    }

    void Discord_bot_impl::stop() {
        if (_bot == nullptr) {
            throw std::runtime_error("Bot is nullptr, no way to stop it");
        }
        _bot->shutdown();
    }

    Discord_cluster *Discord_bot_impl::get_bot() {
        return _bot;
    }

    void Discord_bot_impl::add_pre_requirement(const std::function<void()> &func) {
        std::unique_lock<std::mutex> lock(_mutex);
        if (_bot == nullptr) {
            _pre_requirements.push_back(func);
        } else {
            func();
        }
    }

    void Discord_bot_impl::reply(const dpp::slashcommand_t &event, const dpp::message &message,
                                 const dpp::command_completion_event_t &callback) {

        event.reply(message_preprocessing(message), callback);
    }

    dpp::message Discord_bot_impl::message_preprocessing(dpp::message message) {
        for (auto &embed: message.embeds) {
            embed.set_timestamp(time(nullptr));
        }
        return message;
    }

    void Discord_bot_impl::reply(const dpp::select_click_t &event, const dpp::message &message,
                                 const dpp::command_completion_event_t &callback) {
        event.reply(dpp::ir_update_message, message_preprocessing(message), callback);
    }

    void Discord_bot_impl::reply(const dpp::button_click_t &event, const dpp::message &message,
        const dpp::command_completion_event_t &callback) {
        event.reply(dpp::ir_update_message, message_preprocessing(message), callback);
    }

    void Discord_bot_impl::reply_new(const dpp::button_click_t &event, const dpp::message &message,
        const dpp::command_completion_event_t &callback) {
        event.reply(message_preprocessing(message), callback);
    }

    Module_ptr create() {
        return std::dynamic_pointer_cast<Module>(std::make_shared<Discord_bot_impl>());
    }

} // gb
