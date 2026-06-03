#pragma once

#include "scriba/ast/statement.h"
#include "scriba/ast/expression.h"
#include <memory>
#include <string>
#include <vector>

namespace scriba {
	struct TriggerStatement : Statement {
		std::shared_ptr<Expression> trigger;

		TriggerStatement(std::shared_ptr<Expression> in_trigger)
			: trigger(std::move(in_trigger)) {
		}
	};
} // namespace scriba