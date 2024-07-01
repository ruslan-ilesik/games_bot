//
// Created by ilesik on 6/30/24.
//

#include "module.hpp"


namespace gb {

    std::string Module::get_name() {
        return _name;
    }

    std::vector<std::string> Module::get_dependencies() {
        return _dependencies;
    }

    void Module::run(const Modules &modules) {
        throw Not_implemented_exception("Method run is not implemented in Module");
    }

    void Module::stop() {
        throw Not_implemented_exception("Method run is not implemented in Module");
    }

    Module::Module(const std::string &name, const std::vector<std::string> &_dependencies) {
        this->_name = name;
        this->_dependencies = _dependencies;
    }

    Not_implemented_exception::Not_implemented_exception(const std::string &error) : std::logic_error(error) {
    }

} //gb



