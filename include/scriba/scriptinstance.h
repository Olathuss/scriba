#pragma once

#include <functional>
#include <memory>
#include <variant>
#include <string>

#include "scriba/value.h"
#include "scriba/ast/event_block.h"

namespace scriba {

    class StorageManager;
    class Parser;
    class Scanner;

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
