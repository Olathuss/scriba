#include "scriba/evaluator.h"
#include "scriba/errors/runtime_error.h"

namespace scriba {
	Value scriba::Evaluator::evaluate_expression(const Expression& exp, Environment& env)
	{
		switch (exp.kind) {
		case ExpressionKind::Literal:
			return ev_literal_expression(static_cast<const LiteralExpression&>(exp), env);
		case ExpressionKind::And:
			return ev_and_expression(static_cast<const AndExpression&>(exp), env);
		case ExpressionKind::Or:
			return ev_or_expression(static_cast<const OrExpression&>(exp), env);
		case ExpressionKind::Unary:
			return ev_unary_expression(static_cast<const UnaryExpression&>(exp), env);
		case ExpressionKind::Array:
			return ev_array_expression(static_cast<const ArrayLiteralExpression&>(exp), env);
		case ExpressionKind::Grouping:
			return ev_grouping_expression(static_cast<const GroupingExpression&>(exp), env);
		case ExpressionKind::Binary:
			return ev_binary_expression(static_cast<const BinaryExpression&>(exp), env);
		case ExpressionKind::Identifier:
			return ev_identifier_expression(static_cast<const IdentifierExpression&>(exp), env);
		}
	}

	void Evaluator::execute_statement(const Statement& statement, Environment& env)
	{

	}

	void Evaluator::execute_block(const std::vector<Statement>& block, Environment env)
	{
	}

	Value Evaluator::ev_and_expression(const AndExpression& exp, Environment& env)
	{
		Value left = evaluate_expression(*exp.left, env);

		if (!to_bool(left))
			return Value(false);

		Value right = evaluate_expression(*exp.right, env);
		return Value(to_bool(right));
	}

	Value Evaluator::ev_or_expression(const OrExpression& exp, Environment& env)
	{
		Value left = evaluate_expression(*exp.left, env);

		if (to_bool(left))
			return Value(true);

		Value right = evaluate_expression(static_cast<const Expression>(*exp.right.get()), env);
		return Value(to_bool(right));
	}

	Value Evaluator::ev_array_expression(const ArrayLiteralExpression& exp, Environment& env)
	{
		std::vector<Value> array;
		array.reserve(exp.elements.size());

		for (auto& e : exp.elements) {
			array.push_back(evaluate_expression(*e, env));
		}

		return Value(array);
	}

	Value Evaluator::ev_binary_expression(const BinaryExpression& exp, Environment& env)
	{
		Value left = evaluate_expression(*exp.left, env);
		Value right = evaluate_expression(*exp.left, env);

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

	Value Evaluator::ev_grouping_expression(const GroupingExpression& exp, Environment& env)
	{
		return evaluate_expression(*exp.inner, env);
	}

	Value Evaluator::ev_identifier_expression(const IdentifierExpression& exp, Environment& env)
	{
		const std::string& name = exp.token.lexeme;

		if (!env.exists(name))
			throw RuntimeError("Undefined variable: '" + name + "'", exp.token);

		return env.get(name);
	}

	Value Evaluator::ev_member_expression(const IdentifierExpression& exp, Environment& env)
	{
		return Value();
	}

	Value Evaluator::ev_range_expression(const RangeExpression& exp, Environment& env)
	{
		Value left = evaluate_expression(*exp.left, env);
		Value right = evaluate_expression(*exp.right, env);

		if (!left.is_number() || !right.is_number())
			throw RuntimeError("Range values must be valid numbers", exp.token);

		Range range;
		range.start = left.as_number();
		range.end = right.as_number();
		return Value(range);
	}

	Value Evaluator::ev_literal_expression(const LiteralExpression& exp, Environment& env)
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

	Value Evaluator::ev_unary_expression(const UnaryExpression& exp, Environment& env)
	{
		Value operand = evaluate_expression(*exp.operand, env);
		
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
} // namespace scriba