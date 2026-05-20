#include <string>

#include "scriba/ast/expression.h"
#include "scriba/ast/literal_expression.h"
#include "scriba/ast/grouping_expression.h"
#include "scriba/ast/array_literal_expression.h"
#include "scriba/ast/member_expression.h"
#include "scriba/ast/identifier_expression.h"
#include "scriba/ast/unary_expression.h"
#include "scriba/ast/binary_expression.h"
#include "scriba/ast/and_expression.h"
#include "scriba/ast/or_expression.h"
#include "scriba/ast/range_expression.h"
#include "scriba/ast/statement.h"
#include "scriba/ast/if_statement.h"
#include "scriba/ast/assignment_statement.h"
#include "scriba/ast/block_statement.h"
#include "scriba/ast/command_statement.h"
#include "scriba/ast/trigger_statement.h"
#include "scriba/ast/event_statement.h"
#include "scriba/ast/event_block.h"

using namespace std;
using namespace scriba;

string print(const unique_ptr<Expression>& in_expr);
string print(const unique_ptr<Statement>& in_stmnt);
string print(const EventBlock& in_block);