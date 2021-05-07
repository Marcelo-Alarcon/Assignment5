#ifndef RUNTIMEERRORHANDLER_H_
#define RUNTIMEERRORHANDLER_H_

#include <iostream>
#include <string>
#include <map>

#include "antlr4-runtime.h"

namespace backend { namespace interpreter {

using namespace std;

/**
 * Runtime error codes.
 */
enum class Error
{
    UNINITIALIZED_VALUE,
    VALUE_RANGE,
    INVALID_CASE_EXPRESSION_VALUE,
    DIVISION_BY_ZERO,
    INVALID_STANDARD_FUNCTION_ARGUMENT,
    INVALID_INPUT,
    STACK_OVERFLOW,
    UNIMPLEMENTED_FEATURE
};

constexpr Error UNINITIALIZED_VALUE = Error::UNINITIALIZED_VALUE;
constexpr Error VALUE_RANGE = Error::VALUE_RANGE;
constexpr Error INVALID_CASE_EXPRESSION_VALUE =
                                    Error::INVALID_CASE_EXPRESSION_VALUE;
constexpr Error DIVISION_BY_ZERO  = Error::DIVISION_BY_ZERO;
constexpr Error INVALID_STANDARD_FUNCTION_ARGUMENT =
                                    Error::INVALID_STANDARD_FUNCTION_ARGUMENT;
constexpr Error INVALID_INPUT = Error::INVALID_INPUT;
constexpr Error STACK_OVERFLOW = Error::STACK_OVERFLOW;
constexpr Error UNIMPLEMENTED_FEATURE = Error::UNIMPLEMENTED_FEATURE;

class RuntimeErrorHandler
{
private:
    int count;
    map<Error, string> RUNTIME_ERROR_MESSAGES;

    static const int MAX_ERRORS = 5;

public:
    RuntimeErrorHandler() : count(0)
    {
        RUNTIME_ERROR_MESSAGES[UNINITIALIZED_VALUE] =
                "Undeclared value";
        RUNTIME_ERROR_MESSAGES[VALUE_RANGE] =
                "Value out of range";
        RUNTIME_ERROR_MESSAGES[INVALID_CASE_EXPRESSION_VALUE] =
                "Invalid CASE expression value";
        RUNTIME_ERROR_MESSAGES[DIVISION_BY_ZERO] =
                "Division by zero";
        RUNTIME_ERROR_MESSAGES[INVALID_STANDARD_FUNCTION_ARGUMENT] =
                "Invalid standard function argument";
        RUNTIME_ERROR_MESSAGES[INVALID_INPUT] =
                "Invalid input";
        RUNTIME_ERROR_MESSAGES[STACK_OVERFLOW] =
                "Runtime stack overflow";
        RUNTIME_ERROR_MESSAGES[UNIMPLEMENTED_FEATURE] =
                "Unimplemented runtime feature";
    }

    int getCount() const { return count; }

    /**
     * Flag a runtime error.
     * @param node the root node of the offending statement or expression.
     * @param errorCode the runtime error code.
     * @param backend the backend processor.
     */
    void flag(Error error, antlr4::ParserRuleContext *ctx)
    {
        printf("\n*** RUNTIME ERROR at line %03d: %s\n",
               (int) ctx->getStart()->getLine(),
               RUNTIME_ERROR_MESSAGES[error].c_str());

        if (++count > MAX_ERRORS)
        {
            cout << "*** ABORTED AFTER TOO MANY RUNTIME ERRORS." << endl;
            exit(-1);
        }
    }
};

}} // namespace backend::interpreter

#endif /* RUNTIMEERRORHANDLER_H_ */
