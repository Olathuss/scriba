#pragma once

#include "scriba/ast/statement.h"
#include "scriba/ast/expression.h"
#include <memory>

namespace scriba {
	struct IfStatement : Statement {
		std::shared_ptr<Expression> condition;
		std::shared_ptr<Statement> then_branch;
		std::shared_ptr<Statement> else_branch;

		IfStatement(std::shared_ptr<Expression> in_condition,
			std::shared_ptr<Statement> in_then,
			std::shared_ptr<Statement> in_else)
			: condition(std::move(in_condition)),
			then_branch(std::move(in_then)),
			else_branch(std::move(in_else)) {}
	};
} // namespace scriba