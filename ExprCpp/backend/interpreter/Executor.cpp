#include <iostream>

#include "../ExprCpp/Object.h"
#include "../ExprCpp/backend/interpreter/Executor.h"

namespace backend { namespace interpreter {

using namespace std;

Object Executor::visitProgram(Pcl4Parser::ProgramContext *ctx)
{
	cout << "Starting Program Execution...\n";
    return visit(ctx->block());
}

Object Executor::visitStatement(Pcl4Parser::StatementContext *ctx)
{
    return visitChildren(ctx);
}

Object Executor::visitStatementList(Pcl4Parser::StatementListContext *ctx)
{

    for (Pcl4Parser::StatementContext *stmtCtx : ctx->statement())
    {
        visit(stmtCtx);
    }

    return nullptr;
}

Object Executor::visitCompoundStatement(Pcl4Parser::CompoundStatementContext *ctx)
{
    return visit(ctx->statementList());
}

Object Executor::visitAssignmentStatement(Pcl4Parser::AssignmentStatementContext *ctx)
{
	cout << "Visiting assignment statement\n";

    string variableName = ctx->lhs()->variable()->getText();
    Object rhs_arg  = visit(ctx->rhs()->expression());

    cout << "Assigning " << rhs_arg.as<int>() << " to " << variableName << endl;

    SymtabEntry* varId = symbolTable.lookup(variableName);

    if (varId != nullptr)
    	varId->setValue(rhs_arg.as<int>());

    return rhs_arg;
}

Object Executor::visitRepeatStatement(Pcl4Parser::RepeatStatementContext *ctx)
{
    cout << "Visiting REPEAT statement" << endl;
    return visit(ctx->statementList());
}

Object Executor::visitWriteStatement(Pcl4Parser::WriteStatementContext *ctx)
{
	cout << "Visiting write\n";
    return nullptr;
}

Object Executor::visitWritelnStatement(Pcl4Parser::WritelnStatementContext *ctx)
{
	cout << "Visiting writeLn\n";
    return nullptr;
}

Object Executor::visitExpression(Pcl4Parser::ExpressionContext *ctx)
{
    cout << "Visiting expression" << endl;
    return visitChildren(ctx);
}

Object Executor::visitVariable(Pcl4Parser::VariableContext *ctx)
{
    string variableName = ctx->getText();
    SymtabEntry* varId = symbolTable.lookup(variableName);

    if (varId == nullptr)
		symbolTable.enter(variableName);
    else
    {
    	cout << "Variable not in symbol table!\n";
    	return nullptr;
    }
    return varId->getValue();
}

Object Executor::visitNumber(Pcl4Parser::NumberContext *ctx)
{
    cout << "Visiting number, got value ";
    string text = ctx->unsignedNumber()->integerConstant()
                                       ->INTEGER()->getText();
    int value = stoi(text);
    cout << value << endl;

    return value;
}



// ----- For, If, While, Case -----

Object Executor::visitForStatement(Pcl4Parser::ForStatementContext *ctx)
{
    cout << "Visiting FOR statement" << endl;

    // Get the start and stop values of the for loop
    string var = ctx->variable()->getText();
    int start = stoi(ctx->expression(0)->start->getText());
    int stop = stoi(ctx->expression(1)->start->getText());
    bool dir = 0;

    // Check the direction of the loop
    if (ctx->TO() != 0)
    	dir = 0;
    else if (ctx->DOWNTO() != 0)
    	dir = 1;

    // Visit the statements for every iteration of the loop. Print the current iteration
    if (dir)
    	for (int i = start; i >= stop; i--)
    	{
    		cout << var << " = " << i << endl;
    		visit(ctx->statement());
    	}
    else
    	for (int i = start; i <= stop; i++)
    	{
    		cout << var << " = " << i << endl;
    		visit(ctx->statement());
    	}
    return nullptr;
}

Object Executor::visitIfStatement(Pcl4Parser::IfStatementContext *ctx)
{
    cout << "Visiting IF statement" << endl;

    // Get the true and false statements of the If/Else
    Pcl4Parser::TruestatementContext *trueStmnt = ctx->truestatement();
    Pcl4Parser::FalsestatementContext *falseStmnt = ctx->falsestatement();

    // Check the expression
    bool condition = 1;
    visit(ctx->expression());

    if (condition)
    	visit(trueStmnt);
    else
    	visit(falseStmnt);

    return nullptr;
}

Object Executor::visitWhileStatement(Pcl4Parser::WhileStatementContext *ctx)
{
    cout << "Visiting WHILE statement" << endl;
    return nullptr;
}

Object Executor::visitCaseStatement(Pcl4Parser::CaseStatementContext *ctx)
{
    cout << "Visiting CASE statement" << endl;
    return nullptr;
}
}}
