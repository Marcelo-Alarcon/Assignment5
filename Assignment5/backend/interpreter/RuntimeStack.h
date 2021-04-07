/**
 * <h1>RuntimeStack</h1>
 *
 * <p>The interpreter's runtime stack.</p>
 *
 * <p>Copyright (c) 2020 by Ronald Mak</p>
 * <p>For instructional purposes only.  No warranties.</p>
 */
#ifndef RUNTIMESTACK_H_
#define RUNTIMESTACK_H_

#include <string>
#include <vector>
#include "RuntimeDisplay.h"
#include "StackFrame.h"

namespace backend { namespace interpreter {

class RuntimeStack
{
private:
    vector<StackFrame *> stack;  // runtime stack
    RuntimeDisplay *display;     // runtime display

public:
    /**
     * Constructor.
     */
    RuntimeStack() : display(new RuntimeDisplay()) {}

    /**
     * Destructor.
     */
    ~RuntimeStack()
    {
        delete display;
    }

    /**
     * @return an array list of the activation records on the stack.
     */
    vector<StackFrame *> *records() { return &stack; }

    /**
     * Get the topmost stack frame at a given nesting level.
     * @param nestingLevel the nesting level.
     * @return the stack frame.
     */
    StackFrame *getTopmost(const int nestingLevel) const
    {
        return display->getStackFrame(nestingLevel);
    }

    /**
     * Get the current nesting level.
     * @return the current level.
     */
    int currentNestingLevel() const
    {
        int topIndex = stack.size() - 1;
        return topIndex >= 0 ? stack[topIndex]->getNestingLevel() : -1;
    }

    /**
     * Push a stack frame onto the stack for a routine being called.
     * @param frame the stack frame to push.
     */
    void push(StackFrame *frame)
    {
        int nestingLevel = frame->getNestingLevel();

        stack.push_back(frame);
        display->callUpdate(nestingLevel, frame);
    }

    /**
     * Pop a stack frame off the stack for a returning routine.
     */
    void pop()
    {
        display->returnUpdate(currentNestingLevel());
        stack.erase(stack.begin() + stack.size() - 1);
    }
};

}}  // namespace backend::interpreter

#endif /* RUNTIMESTACK_H_ */
