#include <iostream>
#include <iomanip>
#include <chrono>
#include <string>
#include <vector>
#include <map>

#include "PascalBaseVisitor.h"
#include "antlr4-runtime.h"

#include "../../Object.h"
#include "intermediate/symtab/Predefined.h"
#include "intermediate/symtab/SymtabEntry.h"
#include "intermediate/type/Typespec.h"
#include "StackFrame.h"
#include "Executor.h"

namespace backend { namespace interpreter {

using namespace std;
using namespace std::chrono;

Object Executor::visitProgram(PascalParser::ProgramContext *ctx)
{
    auto start = steady_clock::now();

    StackFrame *programFrame = new StackFrame(programId);
    runtimeStack.push(programFrame);

    visit(ctx->block()->compoundStatement());

    auto end = steady_clock::now();
    long elapsedTime = duration_cast<milliseconds>(end - start).count();
    cout << setfill(' ') << endl;
    cout << setw(20) << executionCount   << " statements executed." << endl;
    cout << setw(20) << error.getCount() << " runtime errors." << endl;
    cout << setw(20) << elapsedTime      << " milliseconds execution time."
                                         << endl;

    return nullptr;
}

Object Executor::visitStatement(PascalParser::StatementContext *ctx)
{
    executionCount++;
    visitChildren(ctx);

    return nullptr;
}

Object Executor::visitAssignmentStatement(
                                PascalParser::AssignmentStatementContext *ctx)
{
    PascalParser::ExpressionContext*exprCtx = ctx->rhs()->expression();
    Object value = visit(exprCtx);
    assignValue(ctx->lhs()->variable(), value, exprCtx->type);

    return nullptr;
}

Cell *Executor::assignValue(PascalParser::VariableContext *varCtx,
                            const Object& value, Typespec *valueType)
{
    Typespec *targetType = varCtx->type;
    Cell *targetCell = visit(varCtx).as<Cell *>();

    assignValue(targetCell, targetType, value, valueType);

    return targetCell;
}

void Executor::assignValue(Cell *targetCell, Typespec *targetType,
                           const Object& value, Typespec *valueType)
{
    // Assign with any necessary type conversions.
    if (   (targetType == Predefined::integerType)
        && (valueType  == Predefined::charType))
    {
        int charValue = value.as<char>();
        targetCell->setValue(charValue);
    }
    else if (targetType == Predefined::realType)
    {
        double doubleValue =
                (valueType == Predefined::integerType) ? value.as<int>()
              : (valueType == Predefined::charType)    ? value.as<char>()
              :                                          value.as<double>();
        targetCell->setValue(doubleValue);
    }
    else if (targetType == Predefined::stringType)
    {
        string stringValue = value.as<string>();
        targetCell->setValue(new string(stringValue));
    }
    else
    {
        targetCell->setValue(value);
    }
}

Object Executor::visitIfStatement(PascalParser::IfStatementContext *ctx)
{
    PascalParser::TrueStatementContext  *trueCtx  = ctx->trueStatement();
    PascalParser::FalseStatementContext *falseCtx = ctx->falseStatement();
    bool value = visit(ctx->expression()).as<bool>();

    if      (value)               visit(trueCtx);
    else if (falseCtx != nullptr) visit(falseCtx);

    return nullptr;
}

Object Executor::visitCaseStatement(PascalParser::CaseStatementContext *ctx)
{
    PascalParser::ExpressionContext *exprCtx = ctx->expression();
    PascalParser::CaseBranchListContext *branchListCtx = ctx->caseBranchList();

    // First time: Create the jump table.
    if (ctx->jumpTable == nullptr)
    {
        ctx->jumpTable = createJumpTable(branchListCtx);
    }

    Object value = visit(exprCtx);
    int intValue =
            (exprCtx->type == Predefined::integerType) ? value.as<int>()
                                                       : value.as<char>();

    // From the jump table obtain the statement corresponding to the value.
    auto *jumpTable = ctx->jumpTable;
    if (jumpTable->find(intValue) != jumpTable->end())
    {
        PascalParser::StatementContext *stmtCtx = (*jumpTable)[intValue];
        visit(stmtCtx);
    }

    return nullptr;
}

/**
 * Create the jump table for a CASE statement.
 * @param branchListCtx the CaseBranchListContext.
 * @return the jump table.
 */
map<int, PascalParser::StatementContext*> *Executor::createJumpTable(
                            PascalParser::CaseBranchListContext *branchListCtx)
{
    auto *table = new map<int, PascalParser::StatementContext*>();
    
    // Loop over the CASE branches.
    for (PascalParser::CaseBranchContext *branchCtx :
                                                branchListCtx->caseBranch())
    {
        PascalParser::CaseConstantListContext *constListCtx =
                                                branchCtx->caseConstantList();
        PascalParser::StatementContext *stmtCtx = branchCtx->statement();
    
        if (constListCtx != nullptr)
        {
            // Loop over the CASE constants of each CASE branch.
            for (PascalParser::CaseConstantContext *caseConstCtx :
                                                constListCtx->caseConstant())
            {
                (*table)[caseConstCtx->value] = stmtCtx;
            }
        }
    }

    return table;
}

Object Executor::visitRepeatStatement(PascalParser::RepeatStatementContext *ctx)
{
    PascalParser::StatementListContext *listCtx = ctx->statementList();
    Object objValue;
    bool value;

    do
    {
        visit(listCtx);
        value = visit(ctx->expression()).as<bool>();
    } while (!value);

    return nullptr;
}

Object Executor::visitWhileStatement(PascalParser::WhileStatementContext *ctx)
{
    PascalParser::StatementContext *stmtCtx = ctx->statement();
    bool value = visit(ctx->expression()).as<bool>();

    while (value)
    {
        visit(stmtCtx);
        value = visit(ctx->expression()).as<bool>();
    }

    return nullptr;
}

Object Executor::visitForStatement(PascalParser::ForStatementContext *ctx)
{
    PascalParser::VariableContext *controlCtx = ctx->variable();
    PascalParser::ExpressionContext *startExprCtx = ctx->expression()[0];
    PascalParser::ExpressionContext *stopExprCtx  = ctx->expression()[1];

    // Initial value.
    Object startValue = visit(startExprCtx);
    assignValue(controlCtx, startValue, startExprCtx->type);

    // Terminal value.
    bool to = ctx->TO() != nullptr;
    Object stopValue = visit(stopExprCtx);
    
    // Integer control values.
    if (controlCtx->type->baseType() == Predefined::integerType)
    {
        int control = startValue.as<int>();
        int stop    = stopValue.as<int>();
        
        if (to)
        {
            while (control <= stop)
            {
                visit(ctx->statement());
                Object nextValue = ++control;
                assignValue(controlCtx, nextValue, Predefined::integerType);
            }
        }
        else  // downto
        {
            while (control >= stop)
            {
                visit(ctx->statement());
                Object nextValue = --control;
                assignValue(controlCtx, nextValue, Predefined::integerType);
            }
        }
    }
    
    // Character control values.
    else
    {
        char control = startValue.as<char>();
        char stop    = stopValue.as<char>();
        
        if (to)
        {
            while (control <= stop)
            {
                visit(ctx->statement());
                Object nextValue = ++control;
                assignValue(controlCtx, nextValue, Predefined::charType);
            }
        }
        else  // downto
        {
            while (control >= stop)
            {
                visit(ctx->statement());
                Object nextValue = --control;
                assignValue(controlCtx, nextValue, Predefined::charType);
            }
        }
    }

    return nullptr;
}

Object Executor::visitProcedureCallStatement(
                            PascalParser::ProcedureCallStatementContext *ctx)
{
    SymtabEntry *routineId = ctx->procedureName()->entry;
    PascalParser::ArgumentListContext *argListCtx = ctx->argumentList();
    StackFrame *newFrame = new StackFrame(routineId);

    // Execute any actual parameters and initialize
    // the formal parameters in the routine's new stack frame.
    if (argListCtx != nullptr)
    {
        vector<SymtabEntry*> *parameters = routineId->getRoutineParameters();
        executeCallArguments(argListCtx, parameters, newFrame);
    }

    // Push the routine's stack frame onto the runtime stack
    // and execute the procedure.
    runtimeStack.push(newFrame);

    // Execute the routine.
//    Object *stmtObj = routineId->getExecutable();
//    PascalParser::CompoundStatementContext *stmtCtx =
//    				(*stmtObj).as<PascalParser::CompoundStatementContext *>();
    Object stmtObj = routineId->getExecutable();
    PascalParser::CompoundStatementContext *stmtCtx =
                    stmtObj.as<PascalParser::CompoundStatementContext *>();
    visit(stmtCtx);

    // Pop off the routine's stack frame.
    runtimeStack.pop();

    return nullptr;
}

void Executor::executeCallArguments(PascalParser::ArgumentListContext *argListCtx,
                                    vector<SymtabEntry*> *parameters,
                                    StackFrame *frame)
{
    // Loop over the parameters.
    for (int i = 0; i < parameters->size(); i++)
    {
        SymtabEntry *parmId = (*parameters)[i];
        string parmName = parmId->getName();
        Kind parmKind = parmId->getKind();
        Cell *parmCell = frame->getCell(parmName);
        PascalParser::ArgumentContext *argCtx = argListCtx->argument()[i];
        Object value = visit(argCtx);
        
        // Value parameter: Copy the argument's value.
        if (parmKind == VALUE_PARAMETER)
        {
            assignValue(parmCell, parmId->getType(),
                        value, argCtx->expression()->type);
        }
        
        // Reference parameter: Copy the argument's cell.
        else
        {
            PascalParser::FactorContext *factorCtx =
                    argCtx->expression()->simpleExpression()[0]
                                                       ->term()[0]->factor()[0];
            PascalParser::VariableContext *varCtx =
                ((PascalParser::VariableFactorContext *) factorCtx)->variable();
            
            Cell *argCell = visitVariable(varCtx).as<Cell*>();
            frame->replaceCell(parmName, argCell);
        }
    }
}

Object Executor::visitExpression(PascalParser::ExpressionContext *ctx)
{
    PascalParser::SimpleExpressionContext *simpleCtx1 =
                                                    ctx->simpleExpression()[0];
    PascalParser::RelOpContext *relOpCtx = ctx->relOp();
    Object operand1 = visit(simpleCtx1);
    Typespec *type1 = simpleCtx1->type;

    // More than one simple expression?
    if (relOpCtx != nullptr)
    {
        string op = relOpCtx->getText();
        PascalParser::SimpleExpressionContext *simpleCtx2 =
                                                    ctx->simpleExpression()[1];
        Object operand2 = visit(simpleCtx2);
        Typespec *type2 = simpleCtx2->type;

        bool integerMode   = false;
        bool realMode      = false;
        bool characterMode = false;

        if (   (type1 == Predefined::integerType)
            && (type2 == Predefined::integerType))
        {
            integerMode = true;
        }
        else if (   (type1 == Predefined::realType)
                 || (type2 == Predefined::realType))
        {
            realMode = true;
        }
        else if (   (type1 == Predefined::charType)
                 && (type2 == Predefined::charType))
        {
            characterMode = true;
        }

        if (integerMode || characterMode)
        {
            int value1 = type1 == Predefined::integerType
                    ? operand1.as<int>() : operand1.as<char>();
            int value2 = type2 == Predefined::integerType
                    ? operand2.as<int>() : operand2.as<char>();
            bool result = false;

            if      (op == "=" ) result = value1 == value2;
            else if (op == "<>") result = value1 != value2;
            else if (op == "<" ) result = value1 <  value2;
            else if (op == "<=") result = value1 <= value2;
            else if (op == ">" ) result = value1 >  value2;
            else if (op == ">=") result = value1 >= value2;

            return result;
        }
        else if (realMode)
        {
            double value1 = type1 == Predefined::integerType
                    ? operand1.as<int>() : operand1.as<double>();
            double value2 = type2 == Predefined::integerType
                    ? operand2.as<int>() : operand2.as<double>();
            bool result = false;

            if      (op == "=" ) result = value1 == value2;
            else if (op == "<>") result = value1 != value2;
            else if (op == "<" ) result = value1 <  value2;
            else if (op == "<=") result = value1 <= value2;
            else if (op == ">" ) result = value1 >  value2;
            else if (op == ">=") result = value1 >= value2;

            return result;
        }
        else  // stringMode)
        {
            string value1 = operand1.as<string>();
            string value2 = operand2.as<string>();
            bool result = false;

            if      (op == "=" ) result = value1 == value2;
            else if (op == "<>") result = value1 != value2;
            else if (op == "<" ) result = value1 <  value2;
            else if (op == "<=") result = value1 <= value2;
            else if (op == ">" ) result = value1 >  value2;
            else if (op == ">=") result = value1 >= value2;

            return result;
        }
    }

    return operand1;
}

Object Executor::visitSimpleExpression(PascalParser::SimpleExpressionContext *ctx)
{
    {
        int count = ctx->term().size();
        bool negate =    (ctx->sign() != nullptr)
                      && (ctx->sign()->getText() == "-");

        // First term.
        PascalParser::TermContext *termCtx1 = ctx->term()[0];
        Object operand1 = visit(termCtx1);
        Typespec *type1 = termCtx1->type;

        if (negate)
        {
            if (type1 == Predefined::charType)
            {
                int value = operand1.as<int>();
                operand1 = -value;
            }
            else if (type1 == Predefined::realType)
            {
                double value = operand1.as<double>();
                operand1 = -value;
            }
        }
        
        // Loop over the subsequent terms.
        for (int i = 1; i < count; i++)
        {
            string op = toLowerCase(ctx->addOp()[i-1]->getText());
            PascalParser::TermContext *termCtx2 = ctx->term()[i];
            Object operand2 = visit(termCtx2);
            Typespec *type2 = termCtx2->type;

            bool integerMode = false;
            bool realMode    = false;
            bool booleanMode = false;

            if (   (type1 == Predefined::integerType)
                && (type2 == Predefined::integerType))
            {
                integerMode = true;
            }
            else if (   (type1 == Predefined::realType)
                     || (type2 == Predefined::realType))
            {
                realMode = true;
            }
            else if (   (type1 == Predefined::booleanType)
                     && (type2 == Predefined::booleanType))
            {
                booleanMode = true;
            }

            if (integerMode)
            {
                int value1 = operand1.as<int>();
                int value2 = operand2.as<int>();
                operand1 = (op == "+") ? value1 + value2
                                       : value1 - value2;
            }
            else if (realMode)
            {
                double value1 = type1 == Predefined::integerType
                        ? operand1.as<int>() : operand1.as<double>();
                double value2 = type2 == Predefined::integerType
                        ? operand2.as<int>() : operand2.as<double>();
                operand1 = (op == "+") ? value1 + value2
                                       : value1 - value2;
            }
            else if (booleanMode)
            {
                operand1 = operand1.as<bool>() || operand2.as<bool>();
            }
            else  // stringMode
            {
                operand1 = operand1.as<string>() + operand2.as<string>();
            }
        }
        
        return operand1;
    }
}

Object Executor::visitTerm(PascalParser::TermContext *ctx)
{
    int count = ctx->factor().size();

    // First factor.
    PascalParser::FactorContext *factorCtx1 = ctx->factor()[0];
    Object operand1 = visit(factorCtx1);
    Typespec *type1 = factorCtx1->type;

    // Loop over the subsequent factors.
    for (int i = 1; i < count; i++)
    {
        string op = toLowerCase(ctx->mulOp()[i-1]->getText());
        PascalParser::FactorContext *factorCtx2 = ctx->factor()[i];
        Object operand2 = visit(factorCtx2);
        Typespec *type2 = factorCtx2->type;

        bool integerMode = false;
        bool realMode    = false;

        if (   (type1 == Predefined::integerType)
            && (type2 == Predefined::integerType))
        {
            integerMode = true;
        }
        else if (   (type1 == Predefined::realType)
                 || (type2 == Predefined::realType))
        {
            realMode = true;
        }
            
        if (integerMode)
        {
            int value1 = operand1.as<int>();
            int value2 = operand2.as<int>();
            
            if (op == "*") operand1 = value1*value2;
            
            else if ((op == "div") || (op == "/") || (op == ("mod")))
            {
                // Check for division by zero.
                if (value2 == 0)
                {
                    error.flag(DIVISION_BY_ZERO, factorCtx2);
                    operand1 = 0;
                }

                if (op == "div")
                {
                    operand1 = value1/value2;
                }
                else if (op == "/")
                {
                    double doubleValue = value1;
                    operand1 = doubleValue/value2;
                }
                else  // mod
                {
                    operand1 = value1 % value2;
                }
            }
        }
        else if (realMode)
        {
            double value1 = type1 == Predefined::integerType
                    ? operand1.as<int>() : operand1.as<double>();
            double value2 = type2 == Predefined::integerType
                    ? operand2.as<int>() : operand2.as<double>();

            if (op == "*") operand1 = value1*value2;

            else if (op == "/")
            {
                // Check for division by zero.
                if (value2 == 0)
                {
                    error.flag(DIVISION_BY_ZERO, factorCtx2);
                    operand1 = 0;
                }
                else operand1 = value1/value2;
            }
        }
        else  // booleanMode
        {
           operand1 = operand1.as<bool>() && operand2.as<bool>();
        }
    }

    return operand1;
}

Object Executor::visitVariableFactor(PascalParser::VariableFactorContext *ctx)
{
    PascalParser::VariableContext *varCtx = ctx->variable();
    Kind kind = varCtx->entry->getKind();

    // Obtain a constant's value from its symbol table entry.
    if ((kind == CONSTANT) || (kind == ENUMERATION_CONSTANT))
    {
        Object value = varCtx->entry->getValue();

        if (varCtx->type == Predefined::booleanType)
        {
            value = value.as<int>() != 0;
        }

        return value;
    }

    // Obtain a variable's value from its memory cell.
    else
    {
        Cell *variableCell = visit(varCtx).as<Cell *>();

        Object value = variableCell->getValue();

        if (ctx->type == Predefined::stringType) return *(value.as<string *>());
        else                                     return value;
    }
}

Object Executor::visitVariable(PascalParser::VariableContext *ctx)
{
    SymtabEntry *variableId = ctx->entry;
    string variableName = variableId->getName();
    Typespec *variableType = variableId->getType();
    int nestingLevel = variableId->getSymtab()->getNestingLevel();

    // Get the variable reference from the appropriate activation record.
    StackFrame *frame = runtimeStack.getTopmost(nestingLevel);
    Cell *variableCell = frame->getCell(variableName);

    // Execute any array subscripts or record fields.
    for (PascalParser::ModifierContext *modCtx : ctx->modifier())
    {
        // Subscripts.
        if (modCtx->indexList() != nullptr)
        {
            // Compute a new reference for each subscript.
            for (PascalParser::IndexContext *indexCtx :
                                                modCtx->indexList()->index())
            {
                Typespec *indexType = variableType->getArrayIndexType();
                int minIndex = 0;

                if (indexType->getForm() == SUBRANGE)
                {
                    minIndex = indexType->getSubrangeMinValue();
                }

                int value = visit(indexCtx->expression()).as<int>();
                int index = value - minIndex;

                vector<Cell *> *array =
                                variableCell->getValue().as<vector<Cell *>*>();
                variableCell = (*array)[index];
                variableType = variableType->getArrayElementType();
            }
        }

        // Record field.
        else
        {
            SymtabEntry *fieldId = modCtx->field()->entry;
            string fieldName = fieldId->getName();

            // Compute a new reference for the field.
            Object cellValue = variableCell->getValue();
            MemoryMap *mmap = cellValue.as<MemoryMap *>();
            variableCell = mmap->getCell(fieldName);
            variableType = fieldId->getType();
        }
    }

    return variableCell;
}

Object Executor::visitNumberFactor(PascalParser::NumberFactorContext *ctx)
{
    Typespec *type = ctx->type;

    if (type == Predefined::integerType)
    {
        return stoi(ctx->getText());
    }
    else  // double
    {
        return stod(ctx->getText());
    }
}

Object Executor::visitCharacterFactor(PascalParser::CharacterFactorContext *ctx)
{
    return ctx->getText()[1];
}

Object Executor::visitStringFactor(PascalParser::StringFactorContext *ctx)
{
    string pascalString = ctx->stringConstant()->STRING()->getText();
    return convertString(pascalString, false);
}

Object Executor::visitFunctionCallFactor(
                                PascalParser::FunctionCallFactorContext *ctx)
{
    PascalParser::FunctionCallContext *callCtx = ctx->functionCall();
    SymtabEntry *routineId = callCtx->functionName()->entry;
    PascalParser::ArgumentListContext *argListCtx = callCtx->argumentList();
    StackFrame *newFrame = new StackFrame(routineId);
    
    // Execute any call arguments and initialize
    // the parameters in the routine's new stack frame.
    if (argListCtx != nullptr)
    {
        vector<SymtabEntry *> *parms = routineId->getRoutineParameters();
        executeCallArguments(argListCtx, parms, newFrame);
    }

    // Push the routine's stack frame onto the runtime stack
    // and execute the procedure.
    runtimeStack.push(newFrame);

    // Execute the routine.
//    Object *stmtObj = routineId->getExecutable();
//    PascalParser::CompoundStatementContext *stmtCtx =
//    				(*stmtObj).as<PascalParser::CompoundStatementContext *>();
    Object stmtObj = routineId->getExecutable();
    PascalParser::CompoundStatementContext *stmtCtx =
                    stmtObj.as<PascalParser::CompoundStatementContext *>();
    visit(stmtCtx);

    // Get the function value from its associated variable.
    string functionName = routineId->getName();
    Cell *valueCell = newFrame->getCell(functionName);
    Object functionValue  = valueCell->getValue();

    // Pop off the routine's stack frame.
    runtimeStack.pop();

    return functionValue;
}

Object Executor::visitNotFactor(PascalParser::NotFactorContext *ctx)
{
    bool value = visit(ctx->factor()).as<bool>();
    return !value;
}

Object Executor::visitParenthesizedFactor(
                                PascalParser::ParenthesizedFactorContext *ctx)
{
    return visit(ctx->expression());
}

Object Executor::visitWritelnStatement(PascalParser::WritelnStatementContext *ctx)
{
    visitChildren(ctx);
    cout << endl;

    return nullptr;
}

Object Executor::visitWriteArguments(PascalParser::WriteArgumentsContext *ctx)
{
    // Loop over each argument.
    for (PascalParser::WriteArgumentContext *argCtx : ctx->writeArgument())
    {
        Typespec *type = argCtx->expression()->type;
        string argText = argCtx->getText();

        // Print any literal strings.
        if (argText[0] == '\'')
        {
            cout << convertString(argText, false);
        }
        
        // For any other expression, print its value with a format specifier.
        else
        {
            Object value = visit(argCtx->expression());
            string format("%");
            
            // Create the format string.
            PascalParser::FieldWidthContext *fwCtx = argCtx->fieldWidth();
            if (fwCtx != nullptr)
            {
                string sign = (   (fwCtx->sign() != nullptr)
                               && (fwCtx->sign()->getText() == "-"))
                            ? "-" : "";
                format += sign + fwCtx->integerConstant()->getText();
                
                PascalParser::DecimalPlacesContext *dpCtx =
                                                    fwCtx->decimalPlaces();
                if (dpCtx != nullptr)
                {
                    format += "." + dpCtx->integerConstant()->getText();
                }
            }
            
            // Use the format string with printf.
            if (type == Predefined::integerType)
            {
                format += "d";
                printf(format.c_str(), value.as<int>());
            }
            else if (type == Predefined::realType)
            {
                format += "f";
                printf(format.c_str(), value.as<double>());
            }
            else if (type == Predefined::booleanType)
            {
                format += "d";
                printf(format.c_str(), value.as<bool>());
            }
            else if (type == Predefined::charType)
            {
                format += "c";
                printf(format.c_str(), value.as<char>());
            }
            else  // string
            {
                format += "s";
                string text = value.as<string>();
                printf(format.c_str(), text.c_str());
            }
        }
    }

    return nullptr;
}

Object Executor::visitReadlnStatement(PascalParser::ReadlnStatementContext *ctx)
{
    visitChildren(ctx);
    cin.ignore(4096, '\n');

    return nullptr;
}

Object Executor::visitReadArguments(PascalParser::ReadArgumentsContext *ctx)
{
    int size = ctx->variable().size();

    // Loop over read arguments.
    for (int i = 0; i < size; i++)
    {
        PascalParser::VariableContext *varCtx = ctx->variable()[i];
        Typespec *varType = varCtx->type;
        
        if (varType == Predefined::integerType)
        {
            int value;
            cin >> value;
            assignValue(varCtx, value, Predefined::integerType);
        }
        else if (varType == Predefined::realType)
        {
            double value;
            cin >> value;
            assignValue(varCtx, value, Predefined::realType);
        }
        else if (varType == Predefined::booleanType)
        {
            bool value;
            cin >> boolalpha >> value;
            assignValue(varCtx, value, Predefined::booleanType);
        }
        else if (varType == Predefined::charType)
        {
            char value = getchar();
            assignValue(varCtx, value, Predefined::charType);
        }
        else  // string
        {
            string value;
            cin >> value;
            assignValue(varCtx, value, Predefined::stringType);
        }
    }

    return nullptr;
}

}} // namespace backend::converter
