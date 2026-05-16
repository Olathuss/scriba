#pragma once

#include <string_view>
#include <unordered_map>
#include "scriba/token_type.h"

namespace scriba {
	inline TokenType keyword_type(std::string_view word) {
		static const std::unordered_map<std::string_view, TokenType> keywords = {
			{"on", TokenType::ON},
			{"trigger", TokenType::TRIGGER},
			{"if", TokenType::IF},
			{"else", TokenType::ELSE},
			{"true", TokenType::TRUE},
			{"false", TokenType::FALSE}
		};

		if (auto it = keywords.find(word); it != keywords.end()) {
			return it->second;
		}

		return TokenType::IDENTIFIER;
	}

	inline bool is_keyword(TokenType type) {
		switch (type) {
		case TokenType::ON:
		case TokenType::TRIGGER:
		case TokenType::IF:
		case TokenType::ELSE:
		case TokenType::TRUE:
		case TokenType::FALSE:
			return true;
		default:
			return false;
		}
	}

} // namespace scriba