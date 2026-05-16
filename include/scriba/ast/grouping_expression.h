#include "scriba/ast/expression.h"
#include "scriba/token.h"

#include <memory>

namespace scriba {
    struct GroupingExpression : Expression {
        std::unique_ptr<Expression> inner;

        GroupingExpression(const Token& token, std::unique_ptr<Expression> expr)
            : Expression(token), inner(std::move(expr)) {
        }
    };
} //namespace scriba