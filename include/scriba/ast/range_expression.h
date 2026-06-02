#pragma once

namespace scriba {
	struct RangeExpression : Expression {
		std::shared_ptr<Expression> left;
		std::shared_ptr<Expression> right;
		RangeExpression(std::shared_ptr<Expression> in_left, std::shared_ptr<Expression> in_right)
			: Expression(ExpressionKind::Range), left(std::move(in_left)), right(std::move(in_right)) {
		};
	};
} // namespace scriba