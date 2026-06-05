#pragma once

#include <vector>
#include <unordered_map>

#include "scriba/token.h"
#include "scriba/storage_manager.h"

#include "scriba/ast/event_block.h"
#include "scriba/ast/expression.h"
#include "scriba/ast/range_expression.h"
#include "scriba/ast/or_expression.h"
#include "scriba/ast/and_expression.h"
#include "scriba/ast/literal_expression.h"
#include "scriba/ast/unary_expression.h"
#include "scriba/ast/binary_expression.h"
#include "scriba/ast/grouping_expression.h"
#include "scriba/ast/identifier_expression.h"
#include "scriba/ast/member_expression.h"
#include "scriba/ast/call_expression.h"
#include "scriba/ast/trigger_expression.h"
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

        std::vector<std::string> event_order;
        std::unordered_map<std::string, EventBlock> events;

        std::vector<IndentLevel> indent_stack;

    public:
        Parser();

        void set_storage_manager(StorageManager* p_storage) {
            storage = p_storage;
        }

        const std::unordered_map<std::string, EventBlock>& get_events() const { return events; }

        const std::vector<std::string>& get_event_names() const { return event_order; }

        void parse_script();

    private:
        EventBlock parse_event_block();

        std::shared_ptr<Statement> parse_block();

        std::vector<std::shared_ptr<Statement>> parse_block_body(const IndentLevel& block_indent);

        std::shared_ptr<Statement> parse_statement();

        std::shared_ptr<Statement> parse_if_statement();

        std::shared_ptr<Statement> parse_trigger_statement();

        std::shared_ptr<Statement> parse_identifier_statement();

        std::shared_ptr<Statement> parse_command_statement(std::shared_ptr<Expression> left_expression);

        std::shared_ptr<Expression> parse_expression();

        std::shared_ptr<Expression> parse_logical_or();

        std::shared_ptr<Expression> parse_logical_and();

        std::shared_ptr<Expression> parse_equality();

        std::shared_ptr<Expression> parse_comparison();

        std::shared_ptr<Expression> parse_range();

        std::shared_ptr<Expression> parse_term();

        std::shared_ptr<Expression> parse_factor();

        std::shared_ptr<Expression> parse_unary();

        std::shared_ptr<Expression> parse_call();

        std::shared_ptr<Expression> parse_postfix();

        std::shared_ptr<Expression> parse_primary();

        std::shared_ptr<ArrayLiteralExpression> parse_array_literal();

        std::shared_ptr<Expression> parse_member_chain(std::shared_ptr<Expression> object);

        std::vector<std::shared_ptr<Expression>> parse_arguments();

        std::shared_ptr<Expression> parse_trigger_expression();

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

        void skip_empty();

        bool is_operator(const TokenType& type);

        bool token_can_start_expression(const TokenType& token) const;

        IndentLevel start_block();

        bool still_in_block(const IndentLevel& block_indent);

        IndentLevel normalize_indent(const Token& indent_token);

        int compare_indent(const IndentLevel& current, const IndentLevel& block);

        void increase_indent(const Token& indent_token);

        void decrease_indent(const Token& indent_token);

        void reset_indentation_stack() {
            indent_stack.clear();
            indent_stack.push_back({ '\0', 0, 0 });
        }
    };
} // namespace scriba