#pragma once

#include <functional>
#include <memory>
#include <string>

namespace scriba {

    class StorageManager;
    class Parser;
    class Scanner;

    class ScriptInstance {

    public:
        using OutputCallback = std::function<void(const std::string&)>;

        ScriptInstance(std::string_view source);
        ~ScriptInstance();

        void set_output_callback(OutputCallback cd);
        void run_script(const std::string& source);

    private:
        OutputCallback output;

        std::unique_ptr<StorageManager> storage;
        std::unique_ptr<Parser>         parser;
        std::unique_ptr<Scanner>        scanner;
    };
} // namespace scriba
