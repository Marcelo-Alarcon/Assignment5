/**
 * <h1>Commander</h1>
 *
 * <p>Process interactive debugger commands.</p>
 *
 * <p>Copyright (c) 2020 by Ronald Mak</p>
 * <p>For instructional purposes only.  No warranties.</p>
 */
#ifndef COMMANDER_H_
#define COMMANDER_H_

#include <vector>
#include <set>
#include <utility>

#include "antlr4-runtime.h"
#include "PascalBaseVisitor.h"
#include "CommanderBaseVisitor.h"

#include "intermediate/symtab/Symtab.h"
#include "intermediate/symtab/SymtabEntry.h"
#include "intermediate/symtab/SymtabStack.h"
#include "backend/interpreter/RuntimeStack.h"

namespace backend { namespace debugger {

using namespace std;
using namespace antlr4;
using namespace intermediate::symtab;
using namespace intermediate::type;
using namespace backend::interpreter;

class Commander : public CommanderBaseVisitor
{
private:
    RuntimeStack *runtimeStack;
    set<int>           breakpoints;
    set<SymtabEntry *> watchpoints;
    bool singleStepping;

public:
    /**
     * Constructor
     * @param runtimeStack the runtime stack.
     */
    Commander(RuntimeStack *runtimeStack)
        : runtimeStack(runtimeStack), singleStepping(false) {}

    /**
     * Start reading commands.
     * @param ctx the current context.
     */
    void start(ParserRuleContext *ctx);

    /**
     * Debug a statement if single stepping or the statement is a breakpoint.
     * @param ctx the statement context.
     */
    void processStatement(PascalParser::StatementContext *ctx);

    /**
     * Debug an assignment if the target variable is a watchpoint.
     * @param ctx the target variable context.
     * @param value the value to assign.
     */
    void processAssignment(PascalParser::VariableContext *ctx,
                           const Object& value, Typespec *type);

    /**
     * Debug a variable factor if the variable is a watchpoint.
     * @param ctx the variable context.
     * @param value the variable's value.
     */
    void processVariableFactor(PascalParser::VariableContext *ctx,
                               const Object& value, Typespec *type);

    Object visitGoCommand(CommanderParser::GoCommandContext *ctx);
    Object visitStepCommand(CommanderParser::StepCommandContext *ctx);
    Object visitQuitCommand(CommanderParser::QuitCommandContext *ctx);
    Object visitBreakCommand(CommanderParser::BreakCommandContext *ctx);
    Object visitUnbreakCommand(CommanderParser::UnbreakCommandContext *ctx);
    Object visitShowCommand(CommanderParser::ShowCommandContext *ctx);
    Object visitStackCommand(CommanderParser::StackCommandContext *ctx);
    Object visitWatchCommand(CommanderParser::WatchCommandContext *ctx);
    Object visitUnwatchCommand(CommanderParser::UnwatchCommandContext *ctx);

private:
    /**
     * Read a debugger command from the console.
     * @param ctx the current context.
     */
    void readCommands(ParserRuleContext *ctx);

    /**
     * Print the current line number.
     * @param lineNumber the line number.
     */
    void printLineNumber(int lineNumber);

    /**
     * Get a variable's cell and datatype from its stack frame.
     * @param variableCtx the variable context.
     * @return the pair<Cell*, Typespec*>.
     */
    pair<Cell*, Typespec*> getCellTypePair(
                                CommanderParser::VariableContext *variableCtx);

    /**
     * Get the symbol table entry of a variable.
     * @param variableName the variable's name.
     * @return the symbol table entry.
     */
    SymtabEntry *getSymtabEntry(string variableName);

    /**
     * Print a value.
     * @paarm value the value to print.
     * @param type the value's datatype.
     */
    void printValue(const Object& value, Typespec *type);
};

}}  // namespace backend::debugger

#endif /* COMMANDER_H_ */
