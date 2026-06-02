#pragma once

#include <string>
#include <functional>
#include <variant>

namespace scriba {
    struct Range {
        double start;
        double end;
    };

    // Forward declare Value as a class
    class Value;

    struct ObjectTypeInfo {
        std::string name;

        std::function<Value(void* instance, std::string_view property)> get_property;
        std::function<void(void* instance, std::string_view property, const Value&)> set_property;

        std::function<Value(void* instance, std::string_view method, const std::vector<Value>& args)> call_method;
    };

    struct ObjectRef {
        void* instance = nullptr;
        const ObjectTypeInfo* type = nullptr;
    };

    class Value {
    public:
        using Variant = std::variant<
            std::monostate,
            double,
            std::string,
            bool,
            std::vector<Value>,
            Range,
            ObjectRef
        >;

        Variant data;

        Value() = default;
        Value(const Variant& v) : data(v) {}
        Value(Variant&& v) : data(std::move(v)) {}

        // Convenience constructors
        Value(double v) : data(v) {}
        Value(const char* s) : data(std::string(s)) {}
        Value(const std::string& s) : data(s) {}
        Value(bool b) : data(b) {}
        Value(const Range& r) : data(r) {}
        Value(const ObjectRef& o) : data(o) {}

        bool is_number() const {
            return std::holds_alternative<double>(data);
        }

        double as_number() const {
            return std::get<double>(data);
        }

        bool is_string() const {
            return std::holds_alternative<std::string>(data);
        }

        const std::string& as_string() const {
            return std::get<std::string>(data);
        }

        bool is_bool() const {
            return std::holds_alternative<bool>(data);
        }

        bool as_bool() const {
            return std::get<bool>(data);
        }
    };
} // namespace scriba