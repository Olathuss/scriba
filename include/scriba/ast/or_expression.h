#pragma once

namespace scriba {
	struct OrExpression : Expression {
		std::unique_ptr<Expression> left;
		std::unique_ptr<Expression> right;
		OrExpression(std::unique_ptr<Expression> in_left, std::unique_ptr<Expression> in_right)
			: left(std::move(in_left)), right(std::move(in_right)) {
		};
	};
} // namespace scriba