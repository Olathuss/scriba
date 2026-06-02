#pragma once

#include "scriba/token.h"
#include "scriba/ast/expression.h"
#include <string>

namespace scriba {
	struct UnaryExpression : Expression {
		std::shared_ptr<Expression> operand;
		UnaryExpression(const Token& in_token, std::shared_ptr<Expression> in_operand)
			: Expression(in_token, ExpressionKind::Unary), operand(std::move(in_operand)) {};
	};
} // namespace scriba