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
#include "scriba/ast/call_expression.h"
#include "scriba/ast/trigger_expression.h"
#include "scriba/ast/array_literal_expression.h"
#include "scriba/ast/statement.h"
#include "scriba/ast/command_statement.h"
#include "scriba/ast/assignment_statement.h"
#include "scriba/ast/trigger_statement.h"
#include "scriba/ast/if_statement.h"
#include "scriba/ast/block_statement.h"
#include "scriba/value.h"
#include "scriba/environment.h"
#include "scriba/type_registry.h"
#include "scriba/object.h"

namespace scriba {
	class TypeRegistry;

	class Evaluator {
	public:
		Evaluator(TypeRegistry& in_registry, Environment& in_environment)
			: type_registry(in_registry), environment(in_environment) {};

		~Evaluator() = default;

		Value evaluate_expression(const Expression& exp);
		void execute_statement(const Statement& statement);
		void execute_block(const std::vector<std::shared_ptr<Statement>>& block);

	private:
		Value ev_and_expression(const AndExpression& exp);
		Value ev_or_expression(const OrExpression& exp);
		Value ev_array_expression(const ArrayLiteralExpression& exp);
		Value ev_binary_expression(const BinaryExpression& exp);
		Value ev_grouping_expression(const GroupingExpression& exp);
		Value ev_identifier_expression(const IdentifierExpression& exp);
		Value ev_member_expression(const MemberExpression& exp);
		Value ev_range_expression(const RangeExpression& exp);
		Value ev_unary_expression(const UnaryExpression& exp);
		Value ev_literal_expression(const LiteralExpression& exp);
		Value ev_call_expression(const CallExpression& exp);
		Value ev_trigger_expression(const TriggerExpression& exp);

		void exe_command_statement(const CommandStatement& statement);
		void exe_trigger_statement(const TriggerStatement& statement);
		void exe_assignment_statement(const AssignmentStatement& statement);
		void exe_if_statement(const IfStatement& statement);

		bool to_bool(const Value& v);
		Value eval_plus(const Value& left, const Value& right, const Token& op);
		Value eval_minus(const Value& left, const Value& right, const Token& op);
		Value eval_mul(const Value& left, const Value& right, const Token& op);
		Value eval_div(const Value& left, const Value& right, const Token& op);

		const std::pair<std::string, ObjectRef> get_object_ref(const MemberExpression& exp);

	private:
		TypeRegistry& type_registry;
		Environment& environment;
	};
} // namespace scriba