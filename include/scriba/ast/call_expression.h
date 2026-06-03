#pragma once

#include "scriba/ast/expression.h"
#include "scriba/ast/member_expression.h"
#include <vector>
#include <memory>

namespace scriba {
	struct CallExpression : Expression {
		std::shared_ptr<Expression> callee;
		std::vector<std::shared_ptr<Expression>> arguments;

		CallExpression() : Expression(ExpressionKind::Call) {};

		CallExpression(const std::shared_ptr<Expression> in_callee,
			const std::vector<std::shared_ptr<Expression>> in_arguments)
			: Expression(ExpressionKind::Call), callee(in_callee), arguments(in_arguments) {}
	};
} // namespace scriba