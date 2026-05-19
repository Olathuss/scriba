#include "scriba/parser.h"
#include "scriba/keywords.h"
#include "scriba/errors/parse_error.h"

#include <vector>

namespace scriba {
    Parser::Parser()
    {
        events.clear();
        indent_stack.clear();
    }

    void Parser::parse_script()
    {
        indent_stack.push_back({ '\0', 0, 0 });

        while (match(TokenType::NEWLINE)) {}

        while (!is_at_eof()) {
            EventBlock event = parse_event_block();
            auto [it, inserted] = events.emplace(event.event_token.lexeme, std::move(event));
            if (!inserted) {
                throw ParseError("Duplicate event name '" + event.event_token.lexeme + "' forbidden.", event.event_token);
            }

            while (match(TokenType::NEWLINE));
        }
    }

    EventBlock Parser::parse_event_block()
    {
        Token event_indent = consume(TokenType::INDENT, ""); // indent should be empty
        if (event_indent.indent.indent_level != 0) {
            throw ParseError("Event declaration should not be indented.", event_indent);
        }
        consume(TokenType::ON, "Expected 'on' at beginning of event block.");

        Token event_name = consume(TokenType::IDENTIFIER, "Expected event name after 'on'.");

        std::vector<Token> arguments;
        while (check(TokenType::IDENTIFIER)) {
            arguments.push_back(advance());
        }

        consume(TokenType::COLON, "Expected ':' after event declaration.");
        consume(TokenType::NEWLINE, "Expected newline after ':'");

        EventBlock event_block;
        event_block.event_token = event_name;
        event_block.arguments = arguments;
        IndentLevel block_indent = start_block();

        event_block.statements = parse_block_body(block_indent);
        return event_block;
    }

    std::unique_ptr<Statement> Parser::parse_block()
    {
        consume(TokenType::NEWLINE, "Expected newline after ':' and before block.");
        IndentLevel block_indent = start_block();
        auto statements = parse_block_body(block_indent);
        return std::make_unique<BlockStatement>(std::move(statements));
    }

    std::vector<std::unique_ptr<Statement>> Parser::parse_block_body(const IndentLevel& block_indent)
    {
        std::vector<std::unique_ptr<Statement>> statements;
        statements.push_back(parse_statement());

        while (still_in_block(block_indent)) {
            statements.push_back(parse_statement());
        }

        return statements;
    }

    std::unique_ptr<Statement> Parser::parse_statement()
    {
        if (match(TokenType::IF)) {
            return parse_if_statement();
        }

        if (match(TokenType::TRIGGER)) {
            return parse_trigger_statement();
        }
        
        if (match(TokenType::IDENTIFIER)) {
            return parse_identifier_statement();
        }

        throw ParseError("This doesn't start a valid statement.", peek());
    }

    std::unique_ptr<Statement> Parser::parse_if_statement()
    {
        auto condition = parse_expression();
        consume(TokenType::COLON, "Expected ':' after if condition.");
        auto then_branch = parse_block();

        std::unique_ptr<Statement> else_branch = nullptr;
        if (match(TokenType::ELSE)) {
            if (match(TokenType::IF)) {
                else_branch = parse_if_statement();
            } else {
                consume(TokenType::COLON, "Expected ':' after else.");
                else_branch = parse_block();
            }            
        }

        return std::make_unique<IfStatement>(
            std::move(condition),
            std::move(then_branch),
            std::move(else_branch)
        );
    }

    std::unique_ptr<Statement> Parser::parse_trigger_statement()
    {
        std::unique_ptr<Expression> left_expression = std::make_unique<IdentifierExpression>(previous());

        if (match(TokenType::DOT)) {
            left_expression = parse_member_chain(std::move(left_expression));
        }

        std::vector<std::unique_ptr<Expression>> arguments;

        while (token_can_start_expression(peek().get_type())) {
            arguments.push_back(parse_expression());
        }

        consume(TokenType::NEWLINE, "Expected end of trigger command.");

        return std::make_unique<TriggerStatement>(
            std::move(left_expression),
            std::move(arguments)
        );
    }

    std::unique_ptr<Statement> Parser::parse_identifier_statement()
    {
        std::unique_ptr<Expression> left_expression = std::make_unique<IdentifierExpression>(previous());

        if (match(TokenType::DOT)) {
            left_expression = parse_member_chain(std::move(left_expression));
        }

        if (match(TokenType::ASSIGN)) {
            auto right_expression = parse_expression();
            consume(TokenType::NEWLINE, "Expected end of assignment.");
            return std::make_unique<AssignmentStatement>(std::move(left_expression), std::move(right_expression));
        }

        return parse_command_statement(std::move(left_expression));
    }

    std::unique_ptr<Statement> Parser::parse_command_statement(std::unique_ptr<Expression> left_expression)
    {
        std::vector<std::unique_ptr<Expression>> arguments;

        while (token_can_start_expression(peek().get_type())) {
            arguments.push_back(parse_expression());
        }

        consume(TokenType::NEWLINE, "Expected end of command.");

        return std::make_unique<CommandStatement>(
            std::move(left_expression),
            std::move(arguments)
        );
    }

    std::unique_ptr<Expression> Parser::parse_expression()
    {
        return parse_range();
    }

    std::unique_ptr<Expression> Parser::parse_range()
    {
        auto expression = parse_equality();

        while (match(TokenType::DOT) && match(TokenType::DOT)) {
            std::unique_ptr<Expression> right = parse_equality();
            expression = std::make_unique<RangeExpression>(std::move(expression), std::move(right));
        }

        return expression;
    }

    std::unique_ptr<Expression> Parser::parse_equality()
    {
        auto expression = parse_comparison();

        while (match(TokenType::EQUAL) || match(TokenType::NOT_EQUAL)) {
            Token op = previous();
            std::unique_ptr<Expression> right = parse_comparison();
            expression = std::make_unique<BinaryExpression>(op, std::move(expression), std::move(right));
        }

        return expression;
    }

    std::unique_ptr<Expression> Parser::parse_comparison()
    {
        auto expression = parse_term();

        while (match(TokenType::GREATER_EQUAL) || match(TokenType::GREATER_THAN) ||
            match(TokenType::LESS_EQUAL) || match(TokenType::LESS_THAN)) {
            Token op = previous();
            auto right = parse_term();
            expression = std::make_unique<BinaryExpression>(op, std::move(expression), std::move(right));
        }

        return expression;
    }

    std::unique_ptr<Expression> Parser::parse_term()
    {
        auto expression = parse_factor();

        while (match(TokenType::PLUS) || match(TokenType::MINUS)) {
            Token op = previous();
            auto right = parse_factor();
            expression = std::make_unique<BinaryExpression>(op, std::move(expression), std::move(right));
        }

        return expression;
    }

    std::unique_ptr<Expression> Parser::parse_factor()
    {
        auto expression = parse_unary();

        while (match(TokenType::SLASH) || match(TokenType::STAR)) {
            Token op = advance();
            auto right = parse_unary();
            expression = std::make_unique<BinaryExpression>(op, std::move(expression), std::move(right));
        }

        return expression;
    }

    std::unique_ptr<Expression> Parser::parse_unary()
    {
        if (match(TokenType::NOT) || match(TokenType::MINUS)) {
            auto right = parse_unary();
            return std::make_unique<UnaryExpression>(previous(), std::move(right));
        }
        return parse_postfix();
    }

    std::unique_ptr<Expression> Parser::parse_postfix()
    {
        auto expression = parse_primary();

        if (match(TokenType::DOT)) {
            expression = parse_member_chain(std::move(expression));
        }
        return expression;
    }

    std::unique_ptr<Expression> Parser::parse_primary()
    {
        if (match(TokenType::FALSE)) return std::make_unique<LiteralExpression>(previous());
        if (match(TokenType::TRUE)) return std::make_unique<LiteralExpression>(previous());

        if (match(TokenType::NUMBER)) return std::make_unique<LiteralExpression>(previous());
        if (match(TokenType::STRING)) return std::make_unique<LiteralExpression>(previous());

        if (match(TokenType::IDENTIFIER)) return std::make_unique<IdentifierExpression>(previous());

        if (match(TokenType::BRACKET_OPEN)) {
            return parse_array_literal();
        }

        if (match(TokenType::PAREN_OPEN)) {
            Token open = previous();
            auto expression = parse_expression();
            expression = std::make_unique<GroupingExpression>(open, std::move(expression));
            consume(TokenType::PAREN_CLOSE, "Expected ')' at end of expression.");
            return expression;
        }

        throw ParseError("Expected expression.", peek());
    }

    std::unique_ptr<ArrayLiteralExpression> Parser::parse_array_literal()
    {
        Token array_start = previous();
        auto array = std::make_unique<ArrayLiteralExpression>(array_start);

        if (check(TokenType::BRACKET_CLOSE)) {
            advance();
            return array;
        }

        do {
            array->elements.push_back(parse_expression());

            if (match(TokenType::COMMA)) {
                if (check(TokenType::BRACKET_CLOSE)) {
                    throw ParseError("Trailing comma not allowed in array.", peek());
                }
            } else {
                break;
            }
        } while (true);

        consume(TokenType::BRACKET_CLOSE, "Expected ']' after array.");

        return array;
    }

    std::unique_ptr<Expression> Parser::parse_member_chain(std::unique_ptr<Expression> object)
    {
        Token property = consume(TokenType::IDENTIFIER, "Expected identifier after '.'");
        object = std::make_unique<MemberExpression>(std::move(object), property);

        while (match(TokenType::DOT)) {
            Token next_property = consume(TokenType::IDENTIFIER, "Expected identifier after '.'");
            object = std::make_unique<MemberExpression>(std::move(object), next_property);
        }
        return object;
    }

    Token Parser::advance() {
        return storage->advance();
    }

    Token Parser::peek() const {
        return storage->peek();
    }

    Token Parser::peek_next() const {
        return storage->peek_next();
    }

    Token Parser::previous() const {
        return storage->previous();
    }

    bool Parser::is_at_eof() const {
        return storage->peek().get_type() == TokenType::END_OF_FILE;
    }

    bool Parser::check(const TokenType& type) const
    {
        return !storage->is_at_end() && storage->peek().get_type() == type;
    }

    bool Parser::match(const TokenType& type) const
    {
        if (check(type)) {
            storage->advance();
            return true;
        }
        return false;
    }

    Token Parser::consume(const TokenType& type, const std::string& message)
    {
        Token current = storage->advance();
        if (current.get_type() != type) {
            throw ParseError(message, current);
        }
        return current;
    }

    bool Parser::is_operator(const TokenType& type) {
        return false;
    }
    bool Parser::token_can_start_expression(const TokenType& type) const
    {
        switch (type) {
        case TokenType::IDENTIFIER:
        case TokenType::NUMBER:
        case TokenType::STRING:
        case TokenType::ARRAY:
        case TokenType::PAREN_OPEN:
        case TokenType::BRACKET_OPEN:
        case TokenType::MINUS:
        case TokenType::NOT:
        case TokenType::TRUE:
        case TokenType::FALSE:
            return true;
        default:
            return false;
        }
    }

    IndentLevel Parser::start_block()
    {
        increase_indent(consume(TokenType::INDENT, "Expected indent after event declaration."));
        return indent_stack.back();
    }

    bool Parser::still_in_block(const IndentLevel& block_indent)
    {
        if (is_at_eof()) return false;
        if (!check(TokenType::INDENT)) {
            throw ParseError("Indentation expected", peek());
        }

        Token indent_token = peek();
        IndentLevel current_indent = normalize_indent(indent_token);
        int indent_diff = compare_indent(current_indent, block_indent);
        if (indent_diff > 0) {
            throw ParseError("Invalid indentation", indent_token);
        } else if (indent_diff < 0) {
            decrease_indent(indent_token);
            return false;
        }
        advance();
        return true;
    }

    IndentLevel Parser::normalize_indent(const Token& indent_token)
    {
        IndentLevel indent;

        indent.indent_char = indent_token.indent.indent_char;
        int width = indent_token.indent.indent_width;

        // Check if we are on our first indent
        if (indent_stack.back().indent_char == '\0') {
            if (indent.indent_char == ' ') {
                indent.indent_width = width;
                indent.indent_level = 1;
            } else { // indent is tab '\t'
                indent.indent_width = 1;
                indent.indent_level = width;
            }
        } else { // This isn't the first indentation
            if (indent.indent_char != indent_stack.back().indent_char) {
                throw ParseError("Mixed indentation not allowed.", indent_token);
            }

            if (indent.indent_char == ' ') {
                if (width % indent_stack.back().indent_width != 0) {
                    throw ParseError("Indentation not consistent.", indent_token);
                }

                indent.indent_width = indent_stack.back().indent_width;
                indent.indent_level = width / indent.indent_width;
            } else { // indent is tab '\t'
                indent.indent_width = 1;
                indent.indent_level = width; // Same as number of '\t' characters
            }
        }

        return indent;
    }

    int Parser::compare_indent(const IndentLevel& current, const IndentLevel& block)
    {
        if (current.indent_level == block.indent_level) return 0;
        if (current.indent_level > block.indent_level) return 1;
        return -1;
    }

    void Parser::increase_indent(const Token& indent_token)
    {
        IndentLevel indent = normalize_indent(indent_token);

        if (indent.indent_level != indent_stack.back().indent_level + 1) {
            throw ParseError("Invalid indentation, indentation must increase by 1 step only.", indent_token);
        }

        indent_stack.push_back(indent);
    }

    void Parser::decrease_indent(const Token& indent_token)
    {
        IndentLevel indent = normalize_indent(indent_token);

        int target = indent.indent_level;
        int index = -1;
        const int start = (int)indent_stack.size() - 2; // Dedent can't match current level, so skip it with -2
        for (int i = start; i >= 0; --i) {
            if (indent_stack[i].indent_level == target) {
                index = i;
                break;
            }
        }
        if (index == -1) {
            throw ParseError("Indentation does not match outer block.", indent_token);
        }

        indent_stack.resize(index + 1);
    }
} // namespace scriba