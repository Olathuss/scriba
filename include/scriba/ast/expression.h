#pragma once

namespace scriba {
	struct Expression {
		Token token;

		Expression() {};
		Expression(const Token& in_token) : token(in_token) {};
		virtual ~Expression() = default;
	};
} // namespace scriba