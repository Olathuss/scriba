#pragma once

#include "scriba/ast/statement.h"
#include <vector>
#include <string>
#include <memory>

namespace scriba {
	struct EventStatement : Statement {
		std::string name;
		std::vector<std::string> arguments;
		std::vector<std::unique_ptr<Statement>> body;

		EventStatement() {}
	};
} // namespace scriba