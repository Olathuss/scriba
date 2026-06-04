#pragma once

#include "scriba/ast/statement.h"
#include "scriba/ast/expression.h"
#include <memory>
#include <string>

namespace scriba {
	struct AssignmentStatement : Statement {
		std::shared_ptr<Expression> left;
		std::shared_ptr<Expression> right;

		AssignmentStatement(std::shared_ptr<Expression> in_left, std::shared_ptr<Expression> in_right)
			: Statement(StatementKind::Assignment), left(std::move(in_left)), right(std::move(in_right)) {}
	};
} // namespace scriba