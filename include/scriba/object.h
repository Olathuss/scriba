#include <string>
#include <functional>

#include "scriba/value.h"
#include "scriba/errors/runtime_error.h"
#include "scriba/token.h"

namespace scriba {
    struct PropertyInfo {
        std::function<Value(void* instance)> getter;
        std::function<void(void* instance, const Value&)> setter;
    };

    struct MethodInfo {
        std::function<Value(void* instance, const std::vector<Value>& args)> invoke;
    };

    struct ObjectTypeInfo {
        std::string name;

        std::unordered_map<std::string, PropertyInfo> properties;
        std::unordered_map<std::string, MethodInfo> methods;

        const PropertyInfo& get_property(const std::string& name, const Token& token) const {
            auto it = properties.find(name);
            if (it == properties.end())
                throw RuntimeError("Invalid property on object \"" + name + "\"", token);
            return it->second;
        }

        const MethodInfo& get_method(const std::string& name, const Token& token) const {
            auto it = methods.find(name);
            if (it == methods.end())
                throw RuntimeError("Invalid method on object \"" + name + "\"", token);
            return it->second;
        }
    };

    struct ObjectRef {
        void* instance = nullptr;
        const ObjectTypeInfo* type = nullptr;
    };
} // namespace scriba