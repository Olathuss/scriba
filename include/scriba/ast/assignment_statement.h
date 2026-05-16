#include "scriba/ast/statement.h"
#include "scriba/ast/expression.h"
#include <memory>
#include <string>

namespace scriba {
	struct AssignmentStatement : Statement {
		std::unique_ptr<Expression> left;
		std::unique_ptr<Expression> right;

		AssignmentStatement(std::unique_ptr<Expression> in_left, std::unique_ptr<Expression> in_right)
			: left(std::move(in_left)), right(std::move(in_right)) {}
	};
} // namespace scriba