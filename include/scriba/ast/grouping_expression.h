#pragma once

#include "scriba/ast/expression.h"
#include "scriba/token.h"

#include <memory>

namespace scriba {
    struct GroupingExpression : Expression {
        std::shared_ptr<Expression> inner;

        GroupingExpression(const Token& token, std::shared_ptr<Expression> expr)
            : Expression(token), inner(std::move(expr)) {
        }
    };
} //namespace scriba