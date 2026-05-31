#pragma once

#include <functional>
#include <memory>
#include <variant>
#include <string>

#include "scriba/ast/event_block.h"

namespace scriba {

    class StorageManager;
    class Parser;
    class Scanner;

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
    };

    class ScriptInstance {

    public:
        using OutputCallback = std::function<void(const std::string&)>;

        ScriptInstance();
        ~ScriptInstance();

        void set_output_callback(OutputCallback cd);
        bool load_script(const std::string& source);
        bool trigger_event(const std::string& name,
            const std::vector<Value>& args = {});

    private:
        OutputCallback output;

        std::vector<std::string> event_order;
        std::unordered_map<std::string, EventBlock> events;

        std::string error;
    };
} // namespace scriba
