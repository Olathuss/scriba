#pragma once

#include <memory>
#include <string>
#include "scriba/scanner.h"
#include "scriba/parser.h"
#include "scriba/storage_manager.h"
#include "scriba/object.h"
#include "scriba/environment.h"

using namespace scriba;

namespace testutils {

    struct TestObject {
        int x = 10;
        int y = 20;

        TestObject() = default;
        TestObject(int in_x, int in_y) : x(in_x), y(in_y) {};

        int add(int a, int b) { return a + b; }
    };

    inline std::unique_ptr<StorageManager> make_storage(const std::string& source) {
        Scanner scanner(source);
        auto tokens = scanner.scan_tokens();

        auto storage = std::make_unique<StorageManager>();
        for (auto& t : tokens)
            storage->add_token(t);

        return storage;
    }

    inline Parser make_parser(const std::string& source) {
        Parser parser;
        auto storage = make_storage(source);
        parser.set_storage_manager(storage.get());
        return parser;
    }

    inline std::shared_ptr<Expression> parse_expr(const std::string& source) {
        Parser parser;
        auto storage = make_storage(source);
        parser.set_storage_manager(storage.get());
        return parser.parse_expression_for_test();
    }

    inline std::shared_ptr<Statement> parse_stmt(const std::string& source) {
        Parser parser;
        auto storage = make_storage(source);
        parser.set_storage_manager(storage.get());
        return parser.parse_statement_for_test();
    }

    ObjectTypeInfo& make_test_object_type();

    ObjectRef make_test_object(int x = 10, int y = 20);

    Environment make_env_with_test_object();

} // namespace testutils
