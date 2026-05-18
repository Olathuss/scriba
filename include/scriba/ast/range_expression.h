namespace scriba {
	struct RangeExpression : Expression {
		std::unique_ptr<Expression> left;
		std::unique_ptr<Expression> right;
		RangeExpression(std::unique_ptr<Expression> in_left, std::unique_ptr<Expression> in_right)
			: left(std::move(in_left)), right(std::move(in_right)) {
		};
	};
} // namespace scriba