#pragma once

#include <vector>
#include <unordered_map>

#include "scriba/token.h"
#include "scriba/storagemanager.h"

#include "scriba/ast/event_block.h"
#include "scriba/ast/expression.h"
#include "scriba/ast/range_expression.h"
#include "scriba/ast/literal_expression.h"
#include "scriba/ast/unary_expression.h"
#include "scriba/ast/binary_expression.h"
#include "scriba/ast/grouping_expression.h"
#include "scriba/ast/identifier_expression.h"
#include "scriba/ast/member_expression.h"
#include "scriba/ast/array_literal_expression.h"
#include "scriba/ast/statement.h"
#include "scriba/ast/if_statement.h"
#include "scriba/ast/block_statement.h"
#include "scriba/ast/command_statement.h"
#include "scriba/ast/trigger_statement.h"
#include "scriba/ast/assignment_statement.h"

namespace scriba {
    class Parser {
    private:
        StorageManager* storage = nullptr;

        std::unordered_map<std::string, EventBlock> events;

        std::vector<IndentLevel> indent_stack;

    public:
        Parser();

        void set_storage_manager(StorageManager* p_storage) {
            storage = p_storage;
        }

        const std::unordered_map<std::string, EventBlock>& get_events() const { return events; }

        void parse_script();

        EventBlock parse_event_block();

        std::unique_ptr<Statement> parse_block();

        std::vector<std::unique_ptr<Statement>> parse_block_body(const IndentLevel& block_indent);

        std::unique_ptr<Statement> parse_statement();

        std::unique_ptr<Statement> parse_if_statement();

        std::unique_ptr<Statement> parse_trigger_statement();

        std::unique_ptr<Statement> parse_identifier_statement();

        std::unique_ptr<Statement> parse_command_statement(std::unique_ptr<Expression> left_expression);

        std::unique_ptr<Expression> parse_expression();

        std::unique_ptr<Expression> parse_range();

        std::unique_ptr<Expression> parse_equality();

        std::unique_ptr<Expression> parse_comparison();

        std::unique_ptr<Expression> parse_term();

        std::unique_ptr<Expression> parse_factor();

        std::unique_ptr<Expression> parse_unary();

        std::unique_ptr<Expression> parse_primary();

        std::unique_ptr<ArrayLiteralExpression> parse_array_literal();

        std::unique_ptr<Expression> parse_member_chain(std::unique_ptr<Expression> object);

        bool is_comparison_operator(const Token& token) const { return token.is_comparison(); }
        bool is_unary_expression(const Token& token) const { return token.is_unary(); }

        bool check(const TokenType& type) const;

        bool match(const TokenType& type) const;

        Token advance();

        Token peek() const;

        Token peek_next() const;

        Token previous() const;

        bool is_at_eof() const;

        Token consume(const TokenType& type, const std::string& message);

        bool is_operator(const TokenType& type);

        bool token_can_start_expression(const TokenType& token) const;

        IndentLevel start_block();

        bool still_in_block(const IndentLevel& block_indent);

        IndentLevel normalize_indent(const Token& indent_token);

        int compare_indent(const IndentLevel& current, const IndentLevel& block);

        void increase_indent(const Token& indent_token);

        void decrease_indent(const Token& indent_token);
    };
} // namespace scriba