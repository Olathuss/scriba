#pragma once

#include "scriba/ast/expression.h"
#include "scriba/token.h"

namespace scriba {
	struct LiteralExpression : Expression {
		LiteralExpression(const Token& in_token)
			: Expression(in_token, ExpressionKind::Literal) {};
	};
} // namespace scriba