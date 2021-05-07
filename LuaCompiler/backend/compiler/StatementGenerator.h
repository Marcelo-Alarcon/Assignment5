#ifndef STATEMENTGENERATOR_H_
#define STATEMENTGENERATOR_H_

#include <vector>
#include <map>

#include "CodeGenerator.h"

namespace backend { namespace compiler {

using namespace std;

class StatementGenerator : public CodeGenerator
{
public:
    /**
     * Constructor.
     * @param parent the parent code generator.
     * @param compiler the compiler to use.
     */
    StatementGenerator(CodeGenerator *parent, Compiler *compiler)
        : CodeGenerator(parent, compiler) {}

    /**
     * Emit code for an assignment statement.
     * @param ctx the AssignmentStatementContext.
     */


    /**
     * Emit code for an IF statement.
     * @param ctx the IfStatementContext.
     */


    /**
     * Emit code for a CASE statement.
     * @param ctx the CaseStatementContext.
     */


    /**
     * Emit code for a REPEAT statement.
     * @param ctx the RepeatStatementContext.
     */


    /**
     * Emit code for a WHILE statement.
     * @param ctx the WhileStatementContext.
     */


    /**
     * Emit code for a FOR statement.
     * @param ctx the ForStatementContext.
     */


    /**
     * Emit code for a procedure call statement.
     * @param ctx the ProcedureCallStatementContext.
     */


    /**
     * Emit code for a function call statement.
     * @param ctx the FunctionCallContext.
     */


    /**
     * Emit code for a WRITE statement.
     * @param ctx the WriteStatementContext.
     */


    /**
     * Emit code for a WRITELN statement.
     * @param ctx the WritelnStatementContext.
     */


    /**
     * Emit code for a READ statement.
     * @param ctx the ReadStatementContext.
     */


    /**
     * Emit code for a READLN statement.
     * @param ctx the ReadlnStatementContext.
     */


private:
    /**
     * Process the CASE map from the CASE branches.
     * @param branchListCtx the CaseBranchListContext.
     * @param branchLabels the branch labels.
     * @return the array list of table entries.
     */


    /**
     * Emit code for the LOOKUPSWITCH instruction.
     * @param entries the table entries.
     * @param branchLabels the branch labels.
     */
    void emitLookupSwitch(map<int, Label *> *labelMap,
                          vector<Label *>& branchLabels);

    /**
     * Emit code for the branch statements.
     * @param branchListCtx the CaseBranchListContext.
     * @param branchLabels the branch labels
     * @throws PascalCompilerException if an error occurred.
     */


    /**
     * Emit a call to a procedure or a function.
     * @param routineId the routine name's symbol table entry.
     * @param argListCtx the ArgumentListContext.
     */


    /**
     * Emit code for a call to WRITE or WRITELN.
     * @param argsCtx the WriteArgumentsContext.
     * @param needLF true if need a line feed.
     */


    /**
     * Create the printf format string.
     * @param argsCtx the WriteArgumentsContext.
     * @param format the format string to create.
     * @return the count of expression arguments.
     */


    /**
     * Emit the printf arguments array.
     * @param argsCtx
     * @param exprCount
     */


    /**
     * Generate code for a call to READ or READLN.
     * @param argsCtx the ReadArgumentsContext.
     * @param needSkip true if need to skip the rest of the input line.
     */

};

}} // namespace backend::compiler

#endif /* STATEMENTGENERATOR_H_ */
