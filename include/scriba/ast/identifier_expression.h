#pragma once

#include "scriba/ast/expression.h"
#include <string>

namespace scriba {
	struct IdentifierExpression : Expression {
		IdentifierExpression(const Token& in_token) 
			: Expression(in_token, ExpressionKind::Identifier) {};
	};
} // namespace scriba