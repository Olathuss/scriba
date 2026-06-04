#pragma once

#include <memory>

#include "scriba/ast/expression.h"
#include "scriba/ast/and_expression.h"
#include "scriba/ast/or_expression.h"
#include "scriba/ast/binary_expression.h"
#include "scriba/ast/grouping_expression.h"
#include "scriba/ast/identifier_expression.h"
#include "scriba/ast/range_expression.h"
#include "scriba/ast/member_expression.h"
#include "scriba/ast/literal_expression.h"
#include "scriba/ast/unary_expression.h"
#include "scriba/ast/array_literal_expression.h"
#include "scriba/ast/statement.h"
#include "scriba/value.h"
#include "scriba/environment.h"

namespace scriba {
	class Evaluator {
	public:
		Evaluator() = default;

		~Evaluator() = default;

		Value evaluate_expression(const Expression& exp, Environment& env);
		void execute_statement(const Statement& statement, Environment& env);
		void execute_block(const std::vector<Statement>& block, Environment env);

	private:
		Value ev_and_expression(const AndExpression& exp, Environment& env);
		Value ev_or_expression(const OrExpression& exp, Environment& env);
		Value ev_array_expression(const ArrayLiteralExpression& exp, Environment& env);
		Value ev_binary_expression(const BinaryExpression& exp, Environment& env);
		Value ev_grouping_expression(const GroupingExpression& exp, Environment& env);
		Value ev_identifier_expression(const IdentifierExpression& exp, Environment& env);
		Value ev_member_expression(const MemberExpression& exp, Environment& env);
		Value ev_range_expression(const RangeExpression& exp, Environment& env);
		Value ev_unary_expression(const UnaryExpression& exp, Environment& env);
		Value ev_literal_expression(const LiteralExpression& exp, Environment& env);

		bool to_bool(const Value& v);
		Value eval_plus(const Value& left, const Value& right, const Token& op);
		Value eval_minus(const Value& left, const Value& right, const Token& op);
		Value eval_mul(const Value& left, const Value& right, const Token& op);
		Value eval_div(const Value& left, const Value& right, const Token& op);
	};
} // namespace scriba