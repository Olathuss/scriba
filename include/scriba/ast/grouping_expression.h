#pragma once

#include "scriba/ast/expression.h"
#include "scriba/token.h"

#include <memory>

namespace scriba {
    struct GroupingExpression : Expression {
        ExpressionKind kind = ExpressionKind::Grouping;
        std::shared_ptr<Expression> inner;

        GroupingExpression(const Token& token, std::shared_ptr<Expression> expr)
            : Expression(token, ExpressionKind::Grouping), inner(std::move(expr)) {
        }
    };
} //namespace scriba