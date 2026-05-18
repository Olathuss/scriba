#pragma once

#include "scriba/ast/statement.h"
#include <memory>
#include <vector>

namespace scriba {
	struct BlockStatement : Statement {
		std::vector<std::unique_ptr<Statement>> statements;

		BlockStatement(std::vector<std::unique_ptr<Statement>> in_statements)
			: statements(std::move(in_statements)) {}
	};
} // namespace scriba