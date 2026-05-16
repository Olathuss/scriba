#pragma once

#include <stdexcept>
#include <string>
#include "scriba/token.h"

namespace scriba {
	class ParseError : public std::runtime_error {
	public:
		ParseError(const std::string& message, const Token& token)
			: std::runtime_error(format(message, token)) {}

	private:
		static std::string format(const std::string& message, const Token& token) {
			return message + " at line: " +
				std::to_string(token.get_line()) +
				", column: " +
				std::to_string(token.get_column());
		}
	};
} // namespace scriba