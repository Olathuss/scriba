#pragma once

#include "scriba/ast/expression.h"
#include <string>

namespace scriba {
	struct IdentifierExpression : Expression {
		Token token;
		IdentifierExpression(const Token& in_token) 
			: token(in_token) {};
	};
} // namespace scriba