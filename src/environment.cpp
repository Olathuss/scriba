#include "scriba/environment.h"
#include "scriba/scriptinstance.h"
#include "scriba/value.h"

namespace scriba {
    Value Environment::get(std::string var_name)
    {
        auto it = table.find(var_name);
        if (it != table.end()) {
            return it->second;
        }
        return Value();
    }

    void Environment::set(std::string var_name, Value var_value)
    {
        table[var_name] = var_value;
    }
} // namespace scriba
