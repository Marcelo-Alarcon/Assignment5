#ifndef CONVERTER_H_
#define CONVERTER_H_

#include <map>
#include <vector>

#include "LuaBaseVisitor.h"
#include "antlr4-runtime.h"

#include "intermediate/symtab/SymtabStack.h"
#include "intermediate/symtab/SymtabEntry.h"
#include "intermediate/type/Typespec.h"
#include "CodeGenerator.h"

namespace backend { namespace converter {

using namespace std;
using namespace intermediate::symtab;
using namespace intermediate::type;

class Converter : public LuaBaseVisitor
{
private:
    CodeGenerator code;
    bool programVariables;
    bool recordFields;
    string currentSeparator;

public:
    Converter()
        : programVariables(true), recordFields(false),
          currentSeparator("")
    {
        typeNameTable["integer"] = "int";
        typeNameTable["real"]    = "double";
        typeNameTable["boolean"] = "bool";
        typeNameTable["char"]    = "char";
        typeNameTable["string"]  = "string";
    }

    /**
     * Get the name of the object (Java) file.
     * @return the name.
     */
    string getObjectFileName() const { return code.getObjectFileName(); }

    // ----------------- Overrite visit functions here ------------------


private:
    // Map a Pascal datatype name to the C++ datatype name.
    map<string, string> typeNameTable;

    /**
     * Map a Pascal type name to an equivalent C++ type name.
     */
    string mapTypeName(string pascalTypeName);

    /**
     * Convert a Pascal type name to the equivalent C++ type.
     * @param pascalType the datatype name.
     * @return the C++ type name.
     */
    string typeName(Typespec*pascalType);

    /**
     * Emit a variable declaration with allocation for an array or record.
     * @param type the datatype of the variable.
     * @param varId the symbol table entry of the variable.
     */
    void emitVariableDeclaration(Typespec *type, SymtabEntry *varId);

    /**
     * Emit code to allocate data for structured (array or record) variables.
     * @param lhsPrefix the prefix for the target variable name.
     * @param symtab the symbol table containing the variable names.
     */
    void emitAllocateStructuredVariables(string lhsPrefix, Symtab *symtab);

    /**
     * Emit code to allocate structured (array or record) data.
     * @param lhsPrefix the prefix for the target variable name.
     * @param variableId the symbol table entry of the target variable.
     */
    void emitAllocateStructuredData(string lhsPrefix, SymtabEntry *variableId);

    /**
     * Emit a string of bracketed dimension sizes for the array datatype.
     * @param type the array datatype.
     */
    void emitArrayDimensions(Typespec *type);

    /**
     * Emit code to allocate an array element.
     * @param lhsPrefix the prefix for the target variable name.
     * @param variableName the name of the target variable.
     * @param elmtType the element's datatype.
     */
    void emitNewArrayElement(string lhsPrefix, string variableName,
                             Typespec *elmtType);

    /**
     * Emit code to allocate a new record.
     * @param lhsPrefix the prefix for the target variable name.
     * @param variableName the name of the target variable.
     * @param recordType the record's datatype.
     */
    void emitNewRecord(string lhsPrefix, string variableName,
                       Typespec recordType);

    /**
     * Emit code to allocate a record's fields.
     * @param lhsPrefix the prefix for the target variable name.
     * @param recordType the record's datatype.
     */
    void emitNewRecordFields(string lhsPrefix, Typespec *recordType);

    /**
     * Emit a record type definition for an unnamed record.
     * @param symtab the symbol table that can contain unnamed records.
     */
    void emitUnnamedRecordDefinitions(Symtab *symtab);

    /**
     * Emit the record fields of a record.
     * @param symtab the symbol table of the unnamed record.
     */
    void emitRecordFields(Symtab *symtab);

    /**
     * Determine the datatype of a variable that can have modifiers.
     * @param varCtx the VariableContext.
     * @param varType the variable's datatype without the modifiers.
     * @return the datatype with any modifiers.
     */
    Typespec *variableDatatype(LuaParser::Var_Context *varCtx,
                               Typespec *varType);

    /**
     * Create the printf format string.
     * @param ctx the WriteArgumentsContext.
     * @return the format string.
     */


    /**
     * Create the string of write arguments.
     * @param ctx the WriteArgumentsContext.
     * @return the string of arguments.
     */

};

}} // namespace backend::converter

#endif /* CONVERTER_H_ */
