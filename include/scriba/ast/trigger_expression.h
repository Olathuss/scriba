#pragma once

#include "scriba/ast/expression.h"
#include <memory>
#include <string>
#include <vector>

namespace scriba {
	struct TriggerExpression : Expression {
		std::shared_ptr<Expression> left;
		std::vector<std::shared_ptr<Expression>> arguments;

		TriggerExpression(std::shared_ptr<Expression> in_left, std::vector<std::shared_ptr<Expression>> args)
			: Expression(ExpressionKind::Trigger), left(std::move(in_left)), arguments(std::move(args)) {
		}
	};
} // namespace scriba