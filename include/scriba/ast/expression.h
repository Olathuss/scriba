#pragma once

#include "scriba/token.h"

namespace scriba {
	enum class ExpressionKind {
		Literal,
		Identifier,
		Member,
		Binary,
		Unary,
		Grouping,
		Array,
		Range,
		And,
		Or,
		Call,
		Trigger,
		Expression
	};

	struct Expression {
		ExpressionKind kind = ExpressionKind::Expression;
		Token token;

		Expression(ExpressionKind in_kind): kind(in_kind) {};
		Expression(const Token& in_token, ExpressionKind in_kind) : token(in_token), kind(in_kind) {};
		virtual ~Expression() = default;
	};
} // namespace scriba