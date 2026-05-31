#pragma once

#include "scriba/token.h"
#include "scriba/ast/expression.h"
#include <string>

namespace scriba {
	struct UnaryExpression : Expression {
		Token token;
		std::shared_ptr<Expression> operand;
		UnaryExpression(const Token& in_token, std::shared_ptr<Expression> in_operand) : token(in_token), operand(std::move(in_operand)) {};
	};
} // namespace scriba