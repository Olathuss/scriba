#include "scriba/environment.h"
#include "scriba/script_instance.h"
#include "scriba/value.h"

namespace scriba {
    Environment::Environment(Environment* in_parent)
        : parent(in_parent)
    {
    }

    Value Environment::get(const std::string& var_name)
    {
        auto it = table.find(var_name);
        if (it != table.end()) {
            return it->second;
        }
        return Value();
    }

    void Environment::set(const std::string& var_name, Value var_value)
    {
        if (table.contains(var_name)) {
            table[var_name] = var_value;
            return;
        }

        if (parent && parent->exists(var_name)) {
            parent->set(var_name, var_value);
            return;
        }

        table[var_name] = var_value;
    }

    bool Environment::exists(const std::string& var_name) {
        if (table.contains(var_name)) return true;
        if (parent) return parent->exists(var_name);
        return false;
    }

    Environment Environment::make_child()
    {
        return Environment(this);
    }
} // namespace scriba
