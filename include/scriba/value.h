#pragma once

#include <string>
#include <variant>

#include "scriba/object.h"

namespace scriba {
    struct Range {
        double start;
        double end;

        bool operator==(const Range& right) const {
            return start == right.start
                && end == right.end;
        }

        bool operator!=(const Range& right) const {
            return !(*this == right);
        }
    };

    // Forward declare Value as a class
    class Value;
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

        bool operator==(const Value& right) const {
            if (is_number() && right.is_number())
                return as_number() == right.as_number();

            if (is_bool() && right.is_bool())
                return as_bool() == right.as_bool();

            if (is_string() && right.is_string())
                return as_string() == right.as_string();

            if (is_object() && right.is_object())
                return as_object() == right.as_object();

            if (is_range() && right.is_range())
                return as_range() == right.as_range();

            return false;
        }

        bool operator!=(const Value& right) const {
            return !(*this == right);
        }

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

        bool is_range() const {
            return std::holds_alternative<Range>(data);
        }

        Range as_range() const {
            return std::get<Range>(data);
        }

        bool is_object() const {
            return std::holds_alternative<ObjectRef>(data);
        }

        ObjectRef as_object() const {
            return std::get<ObjectRef>(data);
        }
    };
} // namespace scriba