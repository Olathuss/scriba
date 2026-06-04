#pragma once

namespace scriba {
	enum class StatementKind {
		Command,
		Trigger,
		Assignment,
		Block,
		If,
		Event,
		Statement
	};

	struct Statement {
		StatementKind kind;

		Statement(StatementKind in_kind): kind(in_kind) {};
		virtual ~Statement() = default;
	};
} // namespace scriba