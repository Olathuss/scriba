#include "scriba/evaluator.h"
#include "scriba/errors/runtime_error.h"


#include <cassert>

namespace scriba {
	Value scriba::Evaluator::evaluate_expression(const Expression& exp)
	{
		switch (exp.kind) {
		case ExpressionKind::Literal:
			return ev_literal_expression(static_cast<const LiteralExpression&>(exp));
		case ExpressionKind::And:
			return ev_and_expression(static_cast<const AndExpression&>(exp));
		case ExpressionKind::Or:
			return ev_or_expression(static_cast<const OrExpression&>(exp));
		case ExpressionKind::Unary:
			return ev_unary_expression(static_cast<const UnaryExpression&>(exp));
		case ExpressionKind::Array:
			return ev_array_expression(static_cast<const ArrayLiteralExpression&>(exp));
		case ExpressionKind::Grouping:
			return ev_grouping_expression(static_cast<const GroupingExpression&>(exp));
		case ExpressionKind::Binary:
			return ev_binary_expression(static_cast<const BinaryExpression&>(exp));
		case ExpressionKind::Identifier:
			return ev_identifier_expression(static_cast<const IdentifierExpression&>(exp));
		case ExpressionKind::Call:
			return ev_call_expression(static_cast<const CallExpression&>(exp));
		case ExpressionKind::Member:
			return ev_member_expression(static_cast<const MemberExpression&>(exp));
		case ExpressionKind::Range:
			return ev_range_expression(static_cast<const RangeExpression&>(exp));
		case ExpressionKind::Trigger:
			return ev_trigger_expression(static_cast<const TriggerExpression&>(exp));
		}

		throw RuntimeError("Unknown expression kind.", exp.token);
	}

	void Evaluator::execute_statement(const Statement& statement)
	{
		switch (statement.kind) {
		case StatementKind::Command:
			exe_command_statement(static_cast<const CommandStatement&>(statement));
			return;
		case StatementKind::Trigger:
			exe_trigger_statement(static_cast<const TriggerStatement&>(statement));
			return;
		case StatementKind::Assignment:
			exe_assignment_statement(static_cast<const AssignmentStatement&>(statement));
			return;
		case StatementKind::If:
			exe_if_statement(static_cast<const IfStatement&>(statement));
			return;
		}
	}

	void Evaluator::execute_block(const std::vector<std::shared_ptr<Statement>>& block)
	{
		for (auto& statement : block) {
			execute_statement(*statement);
		}
	}

	Value Evaluator::ev_and_expression(const AndExpression& exp)
	{
		Value left = evaluate_expression(*exp.left);

		if (!to_bool(left))
			return Value(false);

		Value right = evaluate_expression(*exp.right);
		return Value(to_bool(right));
	}

	Value Evaluator::ev_or_expression(const OrExpression& exp)
	{
		Value left = evaluate_expression(*exp.left);

		if (to_bool(left))
			return Value(true);

		Value right = evaluate_expression(*exp.right);
		return Value(to_bool(right));
	}

	Value Evaluator::ev_array_expression(const ArrayLiteralExpression& exp)
	{
		std::vector<Value> array;
		array.reserve(exp.elements.size());

		for (auto& e : exp.elements) {
			array.push_back(evaluate_expression(*e));
		}

		return Value(array);
	}

	Value Evaluator::ev_binary_expression(const BinaryExpression& exp)
	{
		Value left = evaluate_expression(*exp.left);
		Value right = evaluate_expression(*exp.right);

		switch (exp.token.get_type()) {
		case TokenType::PLUS:
			return eval_plus(left, right, exp.token);
		case TokenType::MINUS:
			return eval_minus(left, right, exp.token);
		case TokenType::STAR:
			return eval_mul(left, right, exp.token);
		case TokenType::SLASH:
			return eval_div(left, right, exp.token);
		default:
			throw RuntimeError("Unknown binary operator", exp.token);
		}
	}

	Value Evaluator::ev_grouping_expression(const GroupingExpression& exp)
	{
		return evaluate_expression(*exp.inner);
	}

	Value Evaluator::ev_identifier_expression(const IdentifierExpression& exp)
	{
		const std::string& name = exp.token.lexeme;

		if (!environment.exists(name))
			throw RuntimeError("Undefined variable: '" + name + "'", exp.token);

		return environment.get(name);
	}

	Value Evaluator::ev_member_expression(const MemberExpression& exp)
	{
		const auto [path, object_ref] = get_object_ref(exp);
		const ObjectTypeInfo& type_info = *object_ref.type;
		const PropertyInfo& property = type_info.get_property(path, exp.token);

		if (!property.getter)
			throw RuntimeError("Property '" + path + "' cannot be read", exp.token);

		return property.getter(object_ref.instance);
	}

	Value Evaluator::ev_range_expression(const RangeExpression& exp)
	{
		Value left = evaluate_expression(*exp.left);
		Value right = evaluate_expression(*exp.right);

		if (!left.is_number() || !right.is_number())
			throw RuntimeError("Range values must be valid numbers", exp.token);

		Range range;
		range.start = left.as_number();
		range.end = right.as_number();
		return Value(range);
	}

	Value Evaluator::ev_literal_expression(const LiteralExpression& exp)
	{
		const auto& literal = exp.token.literal;

		if (std::holds_alternative<double>(literal))
			return Value(std::get<double>(literal));

		if (std::holds_alternative<std::string>(literal))
			return Value(std::get<std::string>(literal));

		if (std::holds_alternative<bool>(literal))
			return Value(std::get<bool>(literal));

		if (std::holds_alternative<std::monostate>(literal))
			return Value();

		throw RuntimeError("Invalid literal expression", exp.token);
	}

	Value Evaluator::ev_call_expression(const CallExpression& exp)
	{
		auto& callee = *exp.callee;

		if (callee.kind != ExpressionKind::Member)
			// Call will work on identifiers, but for now only allow members
			throw RuntimeError("Can only assign values to object properties", callee.token);

		const auto [path, object_ref] = get_object_ref(static_cast<MemberExpression&>(callee));
		const ObjectTypeInfo& type_info = *object_ref.type;
		const MethodInfo& method = type_info.get_method(path, callee.token);

		if (!object_ref.instance) {
			auto identifier = exp.token.lexeme;
			auto pos = identifier.find('.');
			if (pos != std::string::npos)
				identifier = identifier.substr(0, pos);
			throw RuntimeError("'" + identifier + "' is not a valid object", exp.token);
		}

		if (!method.invoke)
			throw RuntimeError("'" + path + "' is not a valid method", exp.token);

		std::vector<Value> arguments;
		arguments.reserve(exp.arguments.size());
		for (auto& arg : exp.arguments) {
			auto value = evaluate_expression(*arg);
			arguments.push_back(value);
		}

		return method.invoke(object_ref.instance, arguments);
	}

	Value Evaluator::ev_trigger_expression(const TriggerExpression& exp)
	{
		return Value();
	}

	void Evaluator::exe_command_statement(const CommandStatement& statement)
	{
		evaluate_expression(*statement.call);
	}

	void Evaluator::exe_trigger_statement(const TriggerStatement& statement)
	{
		evaluate_expression(*statement.trigger);
	}

	void Evaluator::exe_assignment_statement(const AssignmentStatement& statement)
	{
		auto& left = *statement.left;

		if (left.kind != ExpressionKind::Member) {
			// Assignment does not support identifiers or custom variables at the mmoment
			throw RuntimeError("Can only assign values to object properties", left.token);
		}

		const auto [path, object_ref] = get_object_ref(static_cast<MemberExpression&>(left));
		const ObjectTypeInfo& type_info = *object_ref.type;
		const PropertyInfo& property = type_info.get_property(path, left.token);

		if (!property.setter)
			throw RuntimeError("Property '" + path + "' is read-only", left.token);

		Value right = evaluate_expression(*statement.right);
		property.setter(object_ref.instance, right);
	}

	void Evaluator::exe_if_statement(const IfStatement& statement)
	{
		Value result = evaluate_expression(*statement.condition);
		bool truth = result.is_bool() && result.as_bool();

		if (truth)
		{
			auto* block = dynamic_cast<BlockStatement*>(statement.then_branch.get());
			if (!block)
				throw RuntimeError("If then-branch is not a block", statement.condition->token);
			execute_block(block->statements);
			return;
		}

		if (statement.else_branch) {
			auto* block = dynamic_cast<BlockStatement*>(statement.else_branch.get());
			if (!block)
				throw RuntimeError("If else-branch is not a block", statement.condition->token);
			execute_block(block->statements);
			return;
		}

		throw RuntimeError("If statement has invalid block statement", Token());
	}

	Value Evaluator::ev_unary_expression(const UnaryExpression& exp)
	{
		Value operand = evaluate_expression(*exp.operand);
		
		switch (exp.token.get_type()) {
		case TokenType::NOT:
			if (!operand.is_bool())
				throw RuntimeError("Unary '!' expects a boolean", exp.token);
			return Value(!operand.as_bool());
		case TokenType::MINUS:
				if (!operand.is_number())
					throw RuntimeError("Unary '-' expects a number", exp.token);
				return Value(-operand.as_number());
		default:
			throw RuntimeError("Invalid unary operator", exp.token);
		}
	}

	bool Evaluator::to_bool(const Value& v)
	{
		return v.is_bool() && v.as_bool();
	}

	Value Evaluator::eval_plus(const Value& left, const Value& right, const Token& op)
	{
		if (left.is_number() && right.is_number())
			return Value(left.as_number() + right.as_number());

		if (left.is_string() && right.is_string())
			return Value(left.as_string() + right.as_string());

		throw RuntimeError("Invalid operands for '+'", op);
	}

	Value Evaluator::eval_minus(const Value& left, const Value& right, const Token& op)
	{
		if (left.is_number() && right.is_number())
			return Value(left.as_number()  - right.as_number());

		throw RuntimeError("Invalid operands for '-'", op);
	}

	Value Evaluator::eval_mul(const Value& left, const Value& right, const Token& op)
	{
		if (left.is_number() && right.is_number())
			return Value(left.as_number() * right.as_number());

		throw RuntimeError("Invalid operands for '*'", op);
	}

	Value Evaluator::eval_div(const Value& left, const Value& right, const Token& op)
	{
		if (left.is_number() && right.is_number()) {
			double r = right.as_number();
			if (r == 0)
				throw RuntimeError("Illegal division by 0", op);
			return Value(left.as_number() / r);
		}

		throw RuntimeError("Invalid operands for '/'", op);
	}

	const std::pair<std::string, ObjectRef> Evaluator::get_object_ref(const MemberExpression& exp)
	{
		std::string path = exp.token.lexeme;
		auto* next = dynamic_cast<MemberExpression*>(exp.object.get());
		auto* last = next;
		while (next) {
			last = next;
			path = next->token.lexeme + "." + path;
			next = dynamic_cast<MemberExpression*>(next->object.get());
		}

		if (!last || !last->object)
			throw RuntimeError("Invalid member chain.", exp.token);

		auto* ident = dynamic_cast<IdentifierExpression*>(last->object.get());
		if (!ident)
			throw RuntimeError("Invalid member chain", exp.token);

		Value identifier = evaluate_expression(*ident);

		if (!identifier.is_object()) {
			throw RuntimeError("Member is not valid object.", ident->token);
		}

		return { path, identifier.as_object() };
	}
} // namespace scriba