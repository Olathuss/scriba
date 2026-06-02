#include "ast_printer.h"

string print(const shared_ptr<Expression>& in_expr, string prepend)
{
    if (auto expr = dynamic_cast<const MemberExpression*>(in_expr.get())) {
        return prepend + "(Member " +
            print(expr->object) + " " +
            expr->token.lexeme + ")";
    }

    if (auto expr = dynamic_cast<const ArrayLiteralExpression*>(in_expr.get())) {
        string out = prepend + "(Array";
        for (const auto& element : expr->elements) {
            out += " " + print(element);
        }
        out += ")";
        return out;
    }

    if (auto expr = dynamic_cast<const GroupingExpression*>(in_expr.get())) {
        return prepend + "(Group " + print(expr->inner) + ")";
    }

    if (auto expr = dynamic_cast<const IdentifierExpression*>(in_expr.get())) {
        return prepend + "(Ident " + expr->token.lexeme + ")";
    }

    if (auto expr = dynamic_cast<const LiteralExpression*>(in_expr.get())) {
        return prepend + "(Literal " + expr->token.lexeme + ")";
    }

    if (auto expr = dynamic_cast<const UnaryExpression*>(in_expr.get())) {
        return prepend + "(Unary " + expr->token.lexeme + " " +
            print(expr->operand) + ")";
    }

    if (auto expr = dynamic_cast<const BinaryExpression*>(in_expr.get())) {
        return prepend + "(Binary " + expr->token.lexeme + " " +
            print(expr->left) + " " +
            print(expr->right) + ")";
    }

    if (auto expr = dynamic_cast<const AndExpression*>(in_expr.get())) {
        return prepend + "(And " +
            print(expr->left) + " " +
            print(expr->right) + ")";
    }

    if (auto expr = dynamic_cast<const OrExpression*>(in_expr.get())) {
        return prepend + "(Or " +
            print(expr->left) + " " +
            print(expr->right) + ")";
    }

    if (auto expr = dynamic_cast<const RangeExpression*>(in_expr.get())) {
        return prepend + "(Range " +
            print(expr->left) + " " +
            print(expr->right) + ")";
    }

    return prepend + "(UnknownExpression)";
}

string print(const shared_ptr<Statement>& in_stmnt, string prepend)
{
    if (auto statement = dynamic_cast<const IfStatement*>(in_stmnt.get())) {
        string out = prepend + "(If\n";
        out += prepend + "\t(Condition " + print(statement->condition) + ")\n";

        out += prepend + "\t(Then\n";
        out += print(statement->then_branch, prepend + "\t\t") + "\n";
        out += prepend + "\t)\n";

        if (statement->else_branch) {
            out += prepend + "\t(Else\n";
            out += print(statement->else_branch, prepend + "\t\t") + "\n";
            out += prepend + "\t)\n";
        } else {
            out += prepend + "\t(Else (NoElse))\n";
        }

        out += prepend + ")";
        return out;
    }


    if (auto statement = dynamic_cast<const CommandStatement*>(in_stmnt.get())) {
        string out = prepend + "(Command " + print(statement->left);
        for (auto& argument : statement->arguments) {
            out += " " + print(argument);
        }
        out += ")";
        return out;
    }

    if (auto statement = dynamic_cast<const AssignmentStatement*>(in_stmnt.get())) {
        return prepend + "(Assignment " +
            print(statement->left) + " " +
            print(statement->right) + ")";
    }
    
    if (auto statement = dynamic_cast<const TriggerStatement*>(in_stmnt.get())) {
        string out = prepend + "(Trigger " + print(statement->left);
        for (auto& argument : statement->arguments) {
            out += " " + print(argument);
        }
        out += ")";
        return out;
    }

    if (auto statement = dynamic_cast<const BlockStatement*>(in_stmnt.get())) {
        string out = prepend + "(Block\n";
        for (auto& block_statement : statement->statements) {
            out += print(block_statement, prepend + "\t") + "\n";
        }
        out += prepend + ")";
        return out;
    }

    return "(UnknownStatement)";
}

string print(const EventBlock& block, string prepend)
{
    string out = "(Event " + block.event_token.lexeme;

    for (auto& argument : block.arguments) {
        out += " " + argument.lexeme;
    }

    out += ":";

    for (auto& block_statement : block.statements) {
        out += "\n" + print(block_statement, "\t");
    }

    out += "\n)";
    return out;
}
