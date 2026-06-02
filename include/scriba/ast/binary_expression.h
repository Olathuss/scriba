#pragma once

#include "scriba/token.h"
#include "scriba/ast/expression.h"
#include <memory>

namespace scriba {
	struct BinaryExpression : Expression {
		ExpressionKind kind = ExpressionKind::Binary;
		std::shared_ptr<Expression> left;
		std::shared_ptr<Expression> right;

		BinaryExpression(const Token& in_token, std::shared_ptr<Expression> in_left, std::shared_ptr<Expression> in_right)
			: Expression(in_token, ExpressionKind::Binary), left(std::move(in_left)), right(std::move(in_right)) {};
	};
} // namespace scriba