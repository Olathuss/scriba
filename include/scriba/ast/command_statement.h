#pragma once

#include "scriba/ast/statement.h"
#include "scriba/ast/expression.h"
#include <memory>
#include <string>
#include <vector>

namespace scriba {
	struct CommandStatement : Statement {
		std::shared_ptr<Expression> call;

		CommandStatement(std::shared_ptr<Expression> in_call)
			: call(std::move(in_call)) {
		}
	};
} // namespace scriba