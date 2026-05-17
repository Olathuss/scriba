#include "scriba/scanner.h"
#include "scriba/keywords.h"
#include "scriba/storagemanager.h"
#include "scriba/errors/scanner_error.h"

namespace scriba {
    scriba::Scanner::Scanner(std::string_view in_source)
        : source(in_source)
    {
    }

    std::vector<Token> scriba::Scanner::scan_tokens()
    {
        while (!is_at_end()) {
            start = current;
            scan_token();
        }

        tokens.emplace_back(TokenType::END_OF_FILE, "", LiteralValue{}, line, column);
        return tokens;
    }

    void scriba::Scanner::scan_token()
    {
        handle_whitespace();

        start = current;

        if (is_at_end()) return;

        char current_char = advance();

        if (is_alpha(current_char)) {
            identifier();
            return;
        }

        if (is_digit(current_char)) {
            number();
            return;
        }

        if (current_char == '.'
            && !is_digit(previous_char())
            && previous_char() != '.'
            && is_digit(peek())) {
            number_starting_with_dot();
            return;
        }

        switch (current_char) {
            case '"': string_literal(); break;
            case ':': add_token(TokenType::COLON); break;
            case '.': add_token(TokenType::DOT); break;
            
            case '=':
                add_token(match('=') ? TokenType::EQUAL : TokenType::ASSIGN);
                break;

            case '!':
                add_token(match('=') ? TokenType::NOT_EQUAL : TokenType::NOT);
                break;

            case '<':
                add_token(match('=') ? TokenType::LESS_EQUAL : TokenType::LESS_THAN);
                break;

            case '>':
                add_token(match('=') ? TokenType::GREATER_EQUAL : TokenType::GREATER_THAN);
                break;

            case '+': add_token(TokenType::PLUS); break;
            case '-': add_token(TokenType::MINUS); break;
            case '*': add_token(TokenType::STAR); break;
            case '/': add_token(TokenType::SLASH); break;

            case '(': add_token(TokenType::PAREN_OPEN); break;
            case ')': add_token(TokenType::PAREN_CLOSE); break;

            case '#':
                while (peek() != '\n' && !is_at_end()) advance();
                break;

            case '[':
                add_token(TokenType::BRACKET_OPEN); break;
            case ']':
                add_token(TokenType::BRACKET_CLOSE); break;

            case ',':
                add_token(TokenType::COMMA); break;

            case '\n':
                handle_newline();
                break;

            default:
                add_token(TokenType::ERROR);
                break;
        }
    }

    char scriba::Scanner::advance()
    {
        char current_char = source[current++];
        column++;
        return current_char;
    }

    char scriba::Scanner::peek() const
    {
        return is_at_end() ? '\0' : source[current];
    }

    char scriba::Scanner::peek_next() const
    {
        return (current + 1 >= source.size() ? '\0' : source[current + 1]);
    }

    char Scanner::previous_char() const
    {
        if (current == 0) return '\0';
        return source[current - 1];
    }

    bool scriba::Scanner::match(char expected)
    {
        if (is_at_end()) return false;
        if (source[current] != expected) return false;

        current++;
        column++;
        return true;
    }

    bool scriba::Scanner::is_at_end() const
    {
        return current >= source.size();
    }

    void scriba::Scanner::add_token(TokenType in_type)
    {
        std::string_view lex = source.substr(start, current - start);
        tokens.emplace_back(in_type, std::string(lex), LiteralValue{}, line, column);
    }

    void scriba::Scanner::add_token(TokenType in_type, std::string_view in_lexeme)
    {
        tokens.emplace_back(in_type, std::string(in_lexeme), LiteralValue{}, line, column);
    }

    void Scanner::add_indent_token(IndentLevel in_indent)
    {
        Token new_token = Token(TokenType::INDENT, "", LiteralValue{}, line, column);
        new_token.indent = in_indent;
        tokens.emplace_back(new_token);
    }

    void scriba::Scanner::identifier()
    {
        while (is_alphanumeric(peek())) advance();

        std::string_view text = source.substr(start, current - start);
        TokenType type = keyword_type(text);

        add_token(type, text);
    }

    void scriba::Scanner::number()
    {
        while (is_digit(peek())) advance();

        if (peek() == '.' && is_digit(peek_next())) {
            advance();

            while (is_digit(peek())) advance();
        }

        std::string_view text = source.substr(start, current - start);
        add_token(TokenType::NUMBER, text);
    }

    void Scanner::number_starting_with_dot()
    {
        // Context:
        // - start points at '.'
        // - current points just after '.'
        // - peek() is guaranteed to be a digit (checked in scan_token)

        while (is_digit(peek())) advance();
        std::string_view text = source.substr(start, current - start);
        add_token(TokenType::NUMBER, text);
    }

    void scriba::Scanner::string_literal()
    {
        while (peek() != '"' && !is_at_end()) {
            if (peek() == '\n') handle_newline();
            advance();
        }

        if (is_at_end()) {
            add_token(TokenType::ERROR);
            return;
        }

        advance();

        std::string_view text = source.substr(start, current - start);
        add_token(TokenType::STRING, text);
    }

    void scriba::Scanner::handle_whitespace()
    {
        while (!is_at_end()) {
            char current_char = peek();
            if (current_char == ' ' || current_char == '\t') {
                if (at_line_start) {
                    handle_indentation();
                    return;
                }
                else {
                    advance();
                }                
            }
            else if (current_char == '\r') {
                advance();
            }
            else {
                at_line_start = false;
                return;
            }
        }
    }

    void Scanner::handle_indentation()
    {
        at_line_start = false;
        IndentLevel indent;

        while (!is_at_end()) {
            char current_char = peek();
            if (current_char == ' ' || current_char == '\t') {
                if (indent.indent_char == '\0') {
                    indent.indent_char = current_char;
                }
                else if (indent.indent_char != current_char) {
                    throw ScannerError("Mixed indentation not allowed.", Token(TokenType::INDENT, "", LiteralValue{}, line, column));
                    break;
                }

                indent.indent_width++;
                advance();
                continue;
            }
            break;
        }

        add_indent_token(indent);
    }

    void scriba::Scanner::handle_newline()
    {
        at_line_start = true;
        line++;
        column = 1;
        add_token(TokenType::NEWLINE, "");
    }

    bool scriba::Scanner::is_alpha(char in_char)
    {
        return (in_char >= 'a' && in_char <= 'z') ||
            (in_char >= 'A' && in_char <= 'Z') ||
            in_char == '_';
    }

    bool scriba::Scanner::is_digit(char in_char)
    {
        return in_char >= '0' && in_char <= '9';
    }

    bool scriba::Scanner::is_alphanumeric(char in_char)
    {
        return is_alpha(in_char) || is_digit(in_char);
    }
}