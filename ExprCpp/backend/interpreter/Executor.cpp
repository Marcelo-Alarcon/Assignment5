#include <iostream>

#include "../ExprCpp/Object.h"
#include "../ExprCpp/backend/interpreter/Executor.h"

namespace backend { namespace interpreter {

using namespace std;

Object Executor::visitProgram(Pcl4Parser::ProgramContext *ctx)
{
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
    else {
    	symbolTable.enter(variableName);
    	varId = symbolTable.lookup(variableName);
    	varId->setValue(rhs_arg.as<int>());
    }

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
    // Check the number of simple expression children held by the expression node
    int num_of_expressions = ctx->simpleExpression().size();

    // Test relational expressions
    if (num_of_expressions > 1)
    {
    	cout << "Visiting expression" << endl;
    	Object lhs = visit(ctx->simpleExpression(0)->term(0));
    	Object rhs = visit(ctx->simpleExpression(1)->term(0));
    	string op = ctx->relOp()->children[0]->toString();

    	if (op == "=")
    		return lhs.as<int>() == rhs.as<int>();
    	else if (op == "<>")
    		return lhs.as<int>() != rhs.as<int>();
    	else if (op == "<")
    		return lhs.as<int>() < rhs.as<int>();
    	else if (op == ">")
    		return lhs.as<int>() == rhs.as<int>();
    	else if (op == "<=")
    		return lhs.as<int>() <= rhs.as<int>();
    	else if (op == ">=")
    		return lhs.as<int>() >= rhs.as<int>();
    	else
    		cout << "Invalid operator\n";

    	return 0;
    } else {

    	// Non-relational expression
    	cout << "Visiting simple expression\n";
    	return visitSimpleExpression(ctx->simpleExpression(0));
    }
}

Object Executor::visitSimpleExpression(Pcl4Parser::SimpleExpressionContext *ctx)
{
	// Check the number of terms and operators of the simple expression node
	int num_of_terms = ctx->term().size();
	int num_of_addops = ctx->addOp().size();
	int lhs, rhs, result;

	vector<Object> terms;
	vector<Object> factors;
	vector<string> add_ops;
	vector<string> mul_ops;

	// Extract operands
	for (int i=0; i<num_of_terms; i++)
	{

		// Check the number of factors and operators of each term
		int num_of_factors = ctx->term(i)->factor().size();
		int num_of_mulops = ctx->term(i)->mulOp().size();

		// Check if any terms need simplification
		if (num_of_factors > 1)
		{


			// ***Check if there are any parenthetic expressions within the factors***


			// Extract all factors and operators
			for (int j=0; j< num_of_factors; j++)
				factors.push_back(visit(ctx->term(i)->factor(j)));

			// Extract operators
			for (int j=0; j<num_of_mulops; j++)
				mul_ops.push_back(ctx->term(i)->mulOp(j)->children[0]->toString());

			int x = 0;
			int y = 0;

			// Current term needs to be multiplied/divided from left to right
			while (num_of_factors > 1)
			{
				lhs = factors[x].as<double>();
				rhs = factors[x+1].as<double>();

				if (mul_ops[y] == "*")
					result = lhs*rhs;
				 else if (mul_ops[y] == "/")
				{
					if (rhs != 0)
						result = lhs/rhs;
					else
						cout << "ERROR: Dividing by zero!\n";
				} else
					cout << "Invalid operation\n";

				factors[x+1] = result;
				x++;
				y++;
				num_of_factors--;
			}

			terms.push_back(factors[x]);
		} else
			terms.push_back(visit(ctx->term(i)->factor(0)).as<int>());

	}


	// Extract operators
	for (int i=0; i<num_of_addops; i++)
		add_ops.push_back(ctx->addOp(i)->children[0]->toString());

	int x = 0;
	int y = 0;

	// Continue adding terms from left to right until a single term is left
	while (num_of_terms > 1)
	{
		lhs = terms[x].as<int>();
		rhs = terms[x+1].as<int>();

		if (add_ops[y] == "+")
			result = lhs + rhs;
		else if (add_ops[y] == "-")
			result = lhs - rhs;
		else
			cout << "Invalid op\n";

		terms[x+1] = result;
		x++;
		y++;
		num_of_terms--;
	}

	// Return the simplified term
	return terms[x];

}

Object Executor::visitVariable(Pcl4Parser::VariableContext *ctx)
{
	cout << "visiting variable..";

    string variableName = ctx->getText();
    SymtabEntry* varId = symbolTable.lookup(variableName);
    int val = varId->getValue();
    Object varObject = val;

    cout << " got value " << varId->getValue() << endl;

    return varObject;
}

Object Executor::visitNumber(Pcl4Parser::NumberContext *ctx)
{
	bool sign = 0;

	// Check for a negative sign
	if (ctx->children.size() == 2 || ctx->parent->parent->parent->children.size() == 2)
		sign = 1;

    cout << "Visiting number, got value ";
    string text = ctx->unsignedNumber()->integerConstant()->INTEGER()->getText();
    int value = stoi(text);

    if (sign)
    	value *= -1;

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
    Object expr = visit(ctx->expression());
    bool condition = expr.as<bool>();

    // Evaluate either statement
    if (condition)
    	visit(trueStmnt);
    else
    	visit(falseStmnt);

    return nullptr;
}

Object Executor::visitWhileStatement(Pcl4Parser::WhileStatementContext *ctx)
{
    cout << "Visiting WHILE statement" << endl;

    // Test the conditional expression
    Object expr = visit(ctx->expression());
    bool condition = expr.as<bool>();

    // Loop if condition is true
    while (condition)
    {
    	// Visit statement nodes
    	visit(ctx->statement());

    	// Re-test the expression
    	expr = visit(ctx->expression());
    	condition = expr.as<bool>();
    }

    return nullptr;
}

Object Executor::visitCaseStatement(Pcl4Parser::CaseStatementContext *ctx)
{
    cout << "Visiting CASE statement" << endl;
    return nullptr;
}
}}
