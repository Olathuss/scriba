#pragma once

namespace scriba {
	struct OrExpression : Expression {
		std::shared_ptr<Expression> left;
		std::shared_ptr<Expression> right;
		OrExpression(std::shared_ptr<Expression> in_left, std::shared_ptr<Expression> in_right)
			: Expression(ExpressionKind::Or), left(std::move(in_left)), right(std::move(in_right)) {
		};
	};
} // namespace scriba