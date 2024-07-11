#include "terminal_command_impl.hpp"

namespace gb {

    Terminal_command_impl::Terminal_command_impl(const std::string& name, const std::string& description,
                                                 const std::string& help, const Terminal_command_callback& callback)
        : _name(name), _description(description), _help(help), _callback(callback) {}

    std::string Terminal_command_impl::get_name() const {
        return _name;
    }

    std::string Terminal_command_impl::get_description() const {
        return _description;
    }

    std::string Terminal_command_impl::get_help() const {
        return _help;
    }

    void Terminal_command_impl::operator()(const std::vector<std::string>& arguments) {
        _callback(arguments);
    }

} // namespace gb
