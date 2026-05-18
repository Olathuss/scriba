#pragma once

#include <vector>
#include <memory>

#include "scriba/token.h"
#include "scriba/ast/statement.h"

namespace scriba {
	struct EventBlock {
		Token event_token;
		std::vector<Token> arguments;
		std::vector<std::unique_ptr<Statement>> statements;
	};
} // namespace scriba