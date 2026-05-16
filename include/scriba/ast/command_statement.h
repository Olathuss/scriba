#include "scriba/ast/statement.h"
#include "scriba/ast/expression.h"
#include <memory>
#include <string>
#include <vector>

namespace scriba {
	struct CommandStatement : Statement {
		std::unique_ptr<Expression> left;
		std::vector<std::unique_ptr<Expression>> arguments;

		CommandStatement(std::unique_ptr<Expression> in_left, std::vector<std::unique_ptr<Expression>> args)
			: left(std::move(in_left)), arguments(std::move(args)) {
		}
	};
} // namespace scriba