/**
 * <h1>Commander</h1>
 *
 * <p>Process interactive debugger commands.</p>
 *
 * <p>Copyright (c) 2020 by Ronald Mak</p>
 * <p>For instructional purposes only.  No warranties.</p>
 */
#include <iostream>
#include <vector>
#include <map>
#include <utility>

#include "antlr4-runtime.h"
#include "PascalBaseVisitor.h"
#include "CommanderBaseVisitor.h"
#include "CommanderLexer.h"
#include "CommanderParser.h"

#include "intermediate/symtab/Symtab.h"
#include "intermediate/symtab/SymtabEntry.h"
#include "intermediate/symtab/SymtabStack.h"
#include "intermediate/symtab/Predefined.h"
#include "backend/interpreter/RuntimeStack.h"
#include "Commander.h"

namespace backend { namespace debugger {

using namespace std;
using namespace antlr4;
using namespace intermediate::symtab;
using namespace intermediate::type;
using namespace backend::interpreter;

void Commander::start(ParserRuleContext *ctx)
{
    readCommands(ctx);
}

void Commander::processStatement(PascalParser::StatementContext *ctx)
{
    int lineNumber = ctx->getStart()->getLine();

    if (singleStepping || breakpoints.find(lineNumber) != breakpoints.end())
    {
        readCommands(ctx);
    }
}

void Commander::processAssignment(PascalParser::VariableContext *ctx,
                                  const Object& value, Typespec *type)
{
    if (watchpoints.find(ctx->entry) != watchpoints.end())
    {
        printLineNumber(ctx->getStart()->getLine());
        cout << ": " << ctx->entry->getName() << " <= ";
        printValue(value, type);
        cout << endl;
    }
}

void Commander::processVariableFactor(PascalParser::VariableContext *ctx,
                                      const Object& value, Typespec *type)
{
    if (watchpoints.find(ctx->entry) != watchpoints.end())
    {
        printLineNumber(ctx->getStart()->getLine());
        cout << ": " << ctx->entry->getName() << " :: ";
        printValue(value, type);
        cout << endl;
    }
}

Object Commander::visitGoCommand(CommanderParser::GoCommandContext *ctx)
{
    singleStepping = false;
    return true;
}

Object Commander::visitStepCommand(CommanderParser::StepCommandContext *ctx)
{
    singleStepping = true;
    return true;
}

Object Commander::visitQuitCommand(CommanderParser::QuitCommandContext *ctx)
{
    cout << "Program terminated." << endl;
    exit(0);

    return false;
}

Object Commander::visitBreakCommand(CommanderParser::BreakCommandContext *ctx)
{
    // Record the list of breakpoints.
    if (ctx->lineNumberList() != nullptr)
    {
        for (CommanderParser::LineNumberContext *lineNumberCtx :
                                            ctx->lineNumberList()->lineNumber())
        {
            int lineNumber = stoi(lineNumberCtx->INTEGER()->getText());
            breakpoints.insert(lineNumber);
        }
    }
    else
    {
        cout << "Breakpoints:";

        // Print the list of breakpoints.
        if (breakpoints.size() > 0)
        {
            for (int lineNumber : breakpoints)
            {
                printf(" %03d", lineNumber);
            }
            cout << endl;
        }
        else
        {
            cout << " none" << endl;
        }
    }

    return false;
}

Object Commander::visitUnbreakCommand(CommanderParser::UnbreakCommandContext *ctx)
{
    if (ctx->lineNumberList() != nullptr)
    {
        for (CommanderParser::LineNumberContext *lineNumberCtx :
                                            ctx->lineNumberList()->lineNumber())
        {
            int lineNumber = stoi(lineNumberCtx->INTEGER()->getText());
            breakpoints.erase(lineNumber);
        }
    }

    return false;
}

Object Commander::visitShowCommand(CommanderParser::ShowCommandContext *ctx)
{
    for (CommanderParser::VariableContext *variableCtx :
                                            ctx->variableList()->variable())
    {
        string variableName = variableCtx->IDENTIFIER()->getText();
        pair<Cell*, Typespec*> cellTypePair = getCellTypePair(variableCtx);
        Cell     *variableCell = cellTypePair.first;
        Typespec *variableType = cellTypePair.second;

        // Print the variable name and the value.
        if ((variableCell != nullptr) && (variableType != nullptr))
        {
            cout << variableName + " :: ";

            Object value = variableCell->getValue();
            if (variableType == Predefined::stringType)
            {
                value = *(value.as<string *>());
            }

            printValue(value, variableType);
            cout << endl;
        }
        else
        {
            cout << "*** Undeclared or invalid: " << variableName << endl;
        }
    }

    return false;
}

Object Commander::visitStackCommand(CommanderParser::StackCommandContext *ctx)
{
    vector<SymtabEntry *> callStack;

    // Loop over the stack frames on the runtime stack
    // starting at the top of stack.
    vector<StackFrame *> *records = runtimeStack->records();
    for (int i = records->size() - 1; i >= 0; --i)
    {
        StackFrame *frame = (*records)[i];
        SymtabEntry *routineId = frame->getRoutineId();
        cout << (routineId->getKind() == FUNCTION ? "FUNCTION " : "PROCEDURE ");
        cout << routineId->getName() + ":" << endl;

        // Add the symbol table entry of the procedure or function.
        callStack.push_back(routineId);

        // Print the value of each local variable.
        for (pair<string, Cell*> p : frame->getAllPairs())
        {
            string variableName = p.first;
            Object value = p.second->getValue();
            SymtabEntry *routineId = frame->getRoutineId();
            Symtab *routineSymtab = routineId->getRoutineSymtab();
            SymtabEntry *variableId = routineSymtab->lookup(variableName);
            Typespec *variableType = variableId->getType();

            if (variableType == Predefined::stringType)
            {
                value = *(value.as<string *>());
            }

            cout << "  " << variableName << " :: ";
            printValue(value, variableType);
            cout << endl;
        }
    }

    return false;
}

Object Commander::visitWatchCommand(CommanderParser::WatchCommandContext *ctx)
{
    // Record the list of watchpoints.
    if (ctx->simpleVariableList() != nullptr)
    {
        for (CommanderParser::SimpleVariableContext *variableCtx :
                                    ctx->simpleVariableList()->simpleVariable())
        {
            string variableName = variableCtx->IDENTIFIER()->getText();
            SymtabEntry *id = getSymtabEntry(variableName);

            if (id != nullptr) watchpoints.insert(id);
            else cout << "*** Undeclared: " + variableName << endl;
        }
    }
    else
    {
        cout << "Watchpoints:";

        // Print the list of watchpoints.
        if (watchpoints.size() > 0)
        {
            for (SymtabEntry *id : watchpoints)
            {
                printf(" %s", id->getName().c_str());
            }
            cout << endl;
        }
        else
        {
            cout << " none" << endl;
        }
    }

    return false;
}

Object Commander::visitUnwatchCommand(CommanderParser::UnwatchCommandContext *ctx)
{
    // Record the list of watchpoints.
    if (ctx->simpleVariableList() != nullptr)
    {
        for (CommanderParser::SimpleVariableContext *variableCtx :
                                    ctx->simpleVariableList()->simpleVariable())
        {
            string variableName = variableCtx->IDENTIFIER()->getText();
            SymtabEntry *id = getSymtabEntry(variableName);

            if (id != nullptr) watchpoints.erase(id);
            else cout << "*** Undeclared: " + variableName << endl;
        }
    }

    return false;
}

void Commander::readCommands(ParserRuleContext *ctx)
{
    cout << endl;
    printLineNumber(ctx->getStart()->getLine());
    cout << endl;

    bool resume = false;

    do
    {
        cout << "Command? ";

        string command;
        getline(cin, command);
        command += '\n';

        // Create the character stream from the input string.
        ANTLRInputStream inputStream(command);

        // Create a lexer which scans the character stream
        // to create a token stream.
        CommanderLexer lexer(&inputStream);
        CommonTokenStream tokens(&lexer);

        // Create a parser which parses the token stream.
        CommanderParser parser(&tokens);

        // Parse the command and visit the parse tree.
        tree::ParseTree *tree = parser.command();
        resume = visit(tree).as<bool>();
    } while (!resume);
}

void Commander::printLineNumber(int lineNumber)
{
    printf("At line %03d", lineNumber);
}

pair<Cell*, Typespec*> Commander::getCellTypePair(
                                CommanderParser::VariableContext *variableCtx)
{
    string variableName = variableCtx->IDENTIFIER()->getText();
    int currentLevel = runtimeStack->currentNestingLevel();
    StackFrame *frame = nullptr;
    Cell *cell = nullptr;

    // Find the variable's cell in the runtime stack.
    for (int level = currentLevel; (cell == nullptr) && (level > 0); level--)
    {
        frame = runtimeStack->getTopmost(level);
        cell = frame->getCell(variableName);
    }

    if (cell == nullptr) return make_pair(nullptr, nullptr);

    // Get the variable's datatype.
    SymtabEntry *routineId = frame->getRoutineId();
    Symtab *routineSymtab = routineId->getRoutineSymtab();
    SymtabEntry *variableId = routineSymtab->lookup(variableName);
    Typespec *variableType = variableId->getType();

    // Execute any array subscripts or record fields.
    for (CommanderParser::ModifierContext *modCtx : variableCtx->modifier())
    {
        // Subscripts.
        if (modCtx->indexList() != nullptr)
        {
            if (variableType->getForm() == ARRAY)
            {
                // Compute a new reference for each subscript.
                for (CommanderParser::IndexContext *indexCtx :
                                                modCtx->indexList()->index())
                {
                    Typespec *indexType = variableType->getArrayIndexType();
                    int minIndex = 0;

                    if (indexType->getForm() == SUBRANGE)
                    {
                        minIndex = indexType->getSubrangeMinValue();
                    }

                    int value = stoi(indexCtx->INTEGER()->getText());
                    int index = value - minIndex;

                    vector<Cell *> *array =
                                        cell->getValue().as<vector<Cell *>*>();
                    cell = (*array)[index];
                    variableType = variableType->getArrayElementType();
                }
            }
            else
            {
                cout << "*** Not an array." << endl;
                return make_pair(nullptr, nullptr);
            }
        }

        // Record field.
        else
        {
            if (variableType->getForm() == RECORD)
            {
                Symtab *symtab = variableType->getRecordSymtab();
                string fieldName =
                        toLowerCase(modCtx->field()->IDENTIFIER()->getText());
                SymtabEntry *fieldId = symtab->lookup(fieldName);

                if (fieldId != nullptr)
                {
                    // Compute a new reference for the field.
                    Object cellValue = cell->getValue();
                    MemoryMap *mmap = cellValue.as<MemoryMap *>();
                    cell = mmap->getCell(fieldName);
                    variableType = fieldId->getType();
                }
                else
                {
                    cout << "*** Invalid field: " << fieldName << endl;
                    return make_pair(nullptr, nullptr);
                }
            }
            else
            {
                cout << "*** Not a record." << endl;
                return make_pair(nullptr, nullptr);
            }
        }
    }

    return make_pair(cell, variableType);
}

SymtabEntry *Commander::getSymtabEntry(string variableName)
{
    SymtabEntry *variableId = nullptr;
    int currentLevel = runtimeStack->currentNestingLevel();

    // Look for the stack frame.
    for (int level = currentLevel;
         (variableId == nullptr) && (level > 0);
         level--)
    {
        // Get the symbol table of the stack frame's routine.
        StackFrame *frame = runtimeStack->getTopmost(level);
        SymtabEntry *routineId = frame->getRoutineId();
        Symtab *routineSymtab = routineId->getRoutineSymtab();

        // Look up the variable in the routine's symbol table.
        variableId = routineSymtab->lookup(variableName);
    }

    return variableId;
}

void Commander::printValue(const Object& value, Typespec *type)
{
    try
    {
        if      (type == Predefined::integerType) cout << value.as<int>();
        else if (type == Predefined::realType)    cout << value.as<double>();
        else if (type == Predefined::booleanType) cout << value.as<bool>();
        else if (type == Predefined::charType)    cout << value.as<char>();
        else if (type == Predefined::stringType)  cout << value.as<string>();

        else if (type->getForm() == ARRAY)  cout << "<array>";
        else if (type->getForm() == RECORD) cout << "<record>";
        else                                cout << "<?>";
    }
    catch (bad_cast& ex)
    {
        cout << "<!>";
    }
}

}}  // namespace backend::debugger
