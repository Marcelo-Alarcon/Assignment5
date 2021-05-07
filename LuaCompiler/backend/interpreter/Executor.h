#ifndef EXECUTOR_H_
#define EXECUTOR_H_

#include "LuaBaseVisitor.h"
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
 * Execute Lua programs.
 */
class Executor : public LuaBaseVisitor
{
private:
    int executionCount;         // count of executed statements
    SymtabEntry *programId;     // program identifier's symbol table entry
    RuntimeStack runtimeStack;  // runtime stack
    RuntimeErrorHandler error;  // runtime error handler

public:
    Executor(SymtabEntry *programId) : executionCount(0), programId(programId) {}

   // Override visit statements

private:
    /**
     * Assign a value to a target variable's memory cell.
     * @param varCtx the VariableContext of the target.
     * @param value the value to assign.
     * @param valueType the datatype of the value.
     * @return the target variable's memory cell.
     */


    /**
     * Assign a value to a target variable's memory cell.
     * @param targetCell the target variable's memory cell.
     * @param targetType the datatype of the target variable.
     * @param value the value to assign.
     * @param valueType the datatype of the value.
     */


    /**
     * Create the jump table for a CASE statement.
     * @param branchListCtx the CaseBranchListContext.
     * @return the jump table.
     */


    /**
     * Execute procedure and function call arguments.
     * @param argListCtx the ArgumentListContext
     * @param parameters the routine's parameters.
     * @param frame the routine's stack frame.
     */

};

}}  // namespace backend::interpreter

#endif /* EXECUTOR_H_ */
