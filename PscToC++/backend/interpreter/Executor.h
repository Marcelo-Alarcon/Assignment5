#ifndef EXECUTOR_H_
#define EXECUTOR_H_

#include "PascalBaseVisitor.h"
#include "antlr4-runtime.h"

#include "intermediate/symtab/SymtabStack.h"
#include "intermediate/symtab/SymtabEntry.h"
#include "intermediate/type/Typespec.h"
#include "RuntimeStack.h"
#include "RuntimeErrorHandler.h"

namespace backend { namespace interpreter {

using namespace std;
using namespace intermediate::symtab;
using namespace intermediate::type;

/**
 * Execute Pascal programs.
 */
class Executor : public PascalBaseVisitor
{
private:
    int executionCount;         // count of executed statements
    SymtabEntry *programId;     // program identifier's symbol table entry
    RuntimeStack runtimeStack;  // runtime stack
    RuntimeErrorHandler error;  // runtime error handler

public:
    Executor(SymtabEntry *programId) : executionCount(0), programId(programId) {}

    Object visitProgram(PascalParser::ProgramContext *ctx) override;
    Object visitStatement(PascalParser::StatementContext *ctx) override;
    Object visitAssignmentStatement(PascalParser::AssignmentStatementContext *ctx) override;
    Object visitIfStatement(PascalParser::IfStatementContext *ctx) override;
    Object visitCaseStatement(PascalParser::CaseStatementContext *ctx) override;
    Object visitRepeatStatement(PascalParser::RepeatStatementContext *ctx) override;
    Object visitWhileStatement(PascalParser::WhileStatementContext *ctx) override;
    Object visitForStatement(PascalParser::ForStatementContext *ctx) override;
    Object visitProcedureCallStatement(PascalParser::ProcedureCallStatementContext *ctx) override;
    Object visitExpression(PascalParser::ExpressionContext *ctx) override;
    Object visitSimpleExpression(PascalParser::SimpleExpressionContext *ctx) override;
    Object visitTerm(PascalParser::TermContext *ctx) override;
    Object visitVariableFactor(PascalParser::VariableFactorContext *ctx) override;
    Object visitVariable(PascalParser::VariableContext *ctx) override;
    Object visitNumberFactor(PascalParser::NumberFactorContext *ctx) override;
    Object visitCharacterFactor(PascalParser::CharacterFactorContext *ctx) override;
    Object visitStringFactor(PascalParser::StringFactorContext *ctx) override;
    Object visitFunctionCallFactor(PascalParser::FunctionCallFactorContext *context) override;
    Object visitNotFactor(PascalParser::NotFactorContext *ctx) override;
    Object visitParenthesizedFactor(PascalParser::ParenthesizedFactorContext *ctx) override;
    Object visitWritelnStatement(PascalParser::WritelnStatementContext *ctx) override;
    Object visitWriteArguments(PascalParser::WriteArgumentsContext *ctx) override;
    Object visitReadlnStatement(PascalParser::ReadlnStatementContext *ctx) override;
    Object visitReadArguments(PascalParser::ReadArgumentsContext *ctx) override;

private:
    /**
     * Assign a value to a target variable's memory cell.
     * @param varCtx the VariableContext of the target.
     * @param value the value to assign.
     * @param valueType the datatype of the value.
     * @return the target variable's memory cell.
     */
    Cell *assignValue(PascalParser::VariableContext *varCtx,
                      const Object& value, Typespec *valueType);

    /**
     * Assign a value to a target variable's memory cell.
     * @param targetCell the target variable's memory cell.
     * @param targetType the datatype of the target variable.
     * @param value the value to assign.
     * @param valueType the datatype of the value.
     */
    void assignValue(Cell *targetCell, Typespec *targetType,
                     const Object& value, Typespec *valueType);

    /**
     * Create the jump table for a CASE statement.
     * @param branchListCtx the CaseBranchListContext.
     * @return the jump table.
     */
    map<int, PascalParser::StatementContext*> *createJumpTable(
                            PascalParser::CaseBranchListContext *branchListCtx);

    /**
     * Execute procedure and function call arguments.
     * @param argListCtx the ArgumentListContext
     * @param parameters the routine's parameters.
     * @param frame the routine's stack frame.
     */
    void executeCallArguments(PascalParser::ArgumentListContext *argListCtx,
                              vector<SymtabEntry*> *parameters,
                              StackFrame *frame);
};

}}  // namespace backend::interpreter

#endif /* EXECUTOR_H_ */
