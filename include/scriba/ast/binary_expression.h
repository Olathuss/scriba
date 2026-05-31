#pragma once

#include "scriba/token.h"
#include "scriba/ast/expression.h"
#include <memory>

namespace scriba {
	struct BinaryExpression : Expression {
		Token token;
		std::shared_ptr<Expression> left;
		std::shared_ptr<Expression> right;

		BinaryExpression(const Token& in_token, std::shared_ptr<Expression> in_left, std::shared_ptr<Expression> in_right)
			: token(in_token), left(std::move(in_left)), right(std::move(in_right)) {};
	};
} // namespace scriba