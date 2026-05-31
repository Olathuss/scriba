#pragma once

#include "scriba/token.h"
#include "scriba/ast/expression.h"
#include <string>
#include <memory>

namespace scriba {
	struct MemberExpression : Expression {
		std::shared_ptr<Expression> object;
		Token property;
		MemberExpression(std::shared_ptr<Expression> in_object, const Token& in_property) : object(std::move(in_object)), property((in_property)) {};
	};
} // namespace scriba