#pragma once

namespace scriba {
	struct AndExpression : Expression {
		std::shared_ptr<Expression> left;
		std::shared_ptr<Expression> right;
		AndExpression(std::shared_ptr<Expression> in_left, std::shared_ptr<Expression> in_right)
			: left(std::move(in_left)), right(std::move(in_right)) {
		};
	};
} // namespace scriba