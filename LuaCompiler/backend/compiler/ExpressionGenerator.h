#ifndef EXPRESSIONGENERATOR_H_
#define EXPRESSIONGENERATOR_H_

#include "CodeGenerator.h"

namespace backend { namespace compiler {

class ExpressionGenerator : public CodeGenerator
{
public:
    /**
     * Constructor.
     * @param parent the parent code generator.
     * @param compiler the compiler to use.
     */
    ExpressionGenerator(CodeGenerator *parent, Compiler *compiler)
        : CodeGenerator(parent, compiler) {}

    /**
     * Emit code for an expression.
     * @param ctx the ExpressionContext.
     */


    /**
     * Emit code for a simple expression.
     * @param ctx the SimpleExpressionContext.
     */


    /**
     * Emit code for a term.
     * @param ctx the TermContext.
     */


    /**
     * Emit code for NOT.
     * @param ctx the NotFactorContext.
     */


    /**
     * Emit code to load a scalar variable's value
     * or a structured variable's address.
     * @param ctx the VariableContext.
     */


    /**
     * Emit code to load a scalar variable's value
     * or a structured variable's address.
     * @param variableNode the variable node.
     */


    /**
     * Emit code to load an integer constant.
     * @parm intCtx the IntegerConstantContext.
     */


    /**
     * Emit code to load real constant.
     * @parm intCtx the IntegerConstantContext.
     */


private:

    /**
     * Emit code to access an array element by loading the array address
     * and the subscript value. This can subsequently be followed by code
     * to load the array element's value or to store into the array element.
     * @param subscriptsNode the SUBSCRIPTS node.
     * @param elmtType the array element type.
     * @param lastModifier true if this is the variable's last modifier.
     * @return the type of the element.
     */


    /**
     * Emit a load of an array element's value.
     * @param elmtType the element type if character, else nullptr.
     */



    /**
     * Emit code to load the address or value of a record field.
     * @param fieldCtx the FieldContext.
     * @param last true if this is the variable's last field, else false.
     * @return the type of the field.
     */

};

}} // namespace backend::compiler

#endif /* EXPRESSIONGENERATOR_H_ */
