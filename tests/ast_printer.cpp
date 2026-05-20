#include "ast_printer.h"

string print(const unique_ptr<Expression>& in_expr)
{
    if (auto expr = dynamic_cast<const MemberExpression*>(in_expr.get())) {
        return "(Member " +
            print(expr->object) + " " +
            expr->property.lexeme + ")";
    }

    if (auto expr = dynamic_cast<const ArrayLiteralExpression*>(in_expr.get())) {
        string out = "(Array";
        for (const auto& element : expr->elements) {
            out += " " + print(element);
        }
        out += ")";
        return out;
    }

    if (auto expr = dynamic_cast<const GroupingExpression*>(in_expr.get())) {
        return "(Group " + print(expr->inner) + ")";
    }

    if (auto expr = dynamic_cast<const IdentifierExpression*>(in_expr.get())) {
        return "(Ident " + expr->token.lexeme + ")";
    }

    if (auto expr = dynamic_cast<const LiteralExpression*>(in_expr.get())) {
        return "(Literal " + expr->token.lexeme + ")";
    }

    if (auto expr = dynamic_cast<const UnaryExpression*>(in_expr.get())) {
        return "(Unary " + expr->token.lexeme + " " +
            print(expr->operand) + ")";
    }

    if (auto expr = dynamic_cast<const BinaryExpression*>(in_expr.get())) {
        return "(Binary " + expr->token.lexeme + " " +
            print(expr->left) + " " +
            print(expr->right) + ")";
    }

    if (auto expr = dynamic_cast<const AndExpression*>(in_expr.get())) {
        return "(And " +
            print(expr->left) + " " +
            print(expr->right) + ")";
    }

    if (auto expr = dynamic_cast<const OrExpression*>(in_expr.get())) {
        return "(Or " +
            print(expr->left) + " " +
            print(expr->right) + ")";
    }

    if (auto expr = dynamic_cast<const RangeExpression*>(in_expr.get())) {
        return "(Range " +
            print(expr->left) + " " +
            print(expr->right) + ")";
    }

    return "(UnknownExpression)";
}

string print(const unique_ptr<Statement>& in_stmnt)
{
    if (auto statement = dynamic_cast<const IfStatement*>(in_stmnt.get())) {
        string out = "(If\n";
        out += "\t(Condition " + print(statement->condition) + ")\n";
        out += "\t(Then " + print(statement->then_branch) + ")\n";

        if (statement->else_branch) {
            out += "\t(Else " + print(statement->else_branch) + ")\n";
        } else {
            out += "\t(Else (NoElse))\n";
        }
        out += ")";
        return out;
    }

    if (auto statement = dynamic_cast<const CommandStatement*>(in_stmnt.get())) {
        string out = "(Command " + print(statement->left);
        for (auto& argument : statement->arguments) {
            out += " " + print(argument);
        }
        out += ")";
        return out;
    }

    if (auto statement = dynamic_cast<const AssignmentStatement*>(in_stmnt.get())) {
        return "(Assignment " +
            print(statement->left) + " " +
            print(statement->right) + ")";
    }
    
    if (auto statement = dynamic_cast<const TriggerStatement*>(in_stmnt.get())) {
        string out = "(Trigger " + print(statement->left);
        for (auto& argument : statement->arguments) {
            out += " " + print(argument);
        }
        out += ")";
        return out;
    }

    if (auto statement = dynamic_cast<const BlockStatement*>(in_stmnt.get())) {
        string out = "(Block\n";
        for (auto& block_statement : statement->statements) {
            out += "\t" + print(block_statement) + "\n";
        }
        out += ")";
        return out;
    }

    return "(UnknownStatement)";
}

string print(const EventBlock& block)
{
    string out = "(Event " + block.event_token.lexeme;

    for (auto& argument : block.arguments) {
        out += " " + argument.lexeme;
    }

    out += ":\n";

    for (auto& block_statement : block.statements) {
        out += "\t" + print(block_statement);
    }

    out += "\n)";
    return out;
}
