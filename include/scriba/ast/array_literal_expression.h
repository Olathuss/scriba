#pragma once

#include "scriba/ast/expression.h"
#include <vector>
#include <memory>

namespace scriba {
	struct ArrayLiteralExpression : Expression {
		std::vector<std::shared_ptr<Expression>> elements;

		ArrayLiteralExpression(): Expression(ExpressionKind::Array) {};

		ArrayLiteralExpression(const Token& in_token) : Expression(in_token, ExpressionKind::Array) {}
	};
} // namespace scriba