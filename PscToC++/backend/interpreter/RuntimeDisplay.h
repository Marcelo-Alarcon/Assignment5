/**
 * <h1>RuntimeDisplay</h1>
 *
 * <p>The interpreter's runtime display.</p>
 *
 * <p>Copyright (c) 2020 by Ronald Mak</p>
 * <p>For instructional purposes only.  No warranties.</p>
 */
#ifndef RUNTIMEDISPLAY_H_
#define RUNTIMEDISPLAY_H_

#include <string>
#include <vector>
#include "StackFrame.h"

namespace backend { namespace interpreter {

class RuntimeDisplay
{
private:
    vector<StackFrame *> display;  // runtime display

public:
    /**
     * Constructor.
     */
    RuntimeDisplay()
    {
        display.push_back(nullptr);  // dummy element 0 (never used)
    }

    /**
     * Get the stack frame at a given nesting level.
     * @param nestingLevel the nesting level.
     * @return the stack frame.
     */
    StackFrame *getStackFrame(const int nestingLevel) const
    {
        return display[nestingLevel];
    }

    /**
     * Update the display for a call to a routine at a given nesting level.
     * @param nestingLevel the nesting level.
     * @param frame the stack frame for the routine.
     */
    void callUpdate(const int nestingLevel, StackFrame *frame)
    {
        // Next higher nesting level: Append a new element at the top.
        if (nestingLevel >= display.size())
        {
            display.push_back(frame);
        }

        // Existing nesting level: Set at the specified level.
        else
        {
            StackFrame *prevFrame = display[nestingLevel];
            display[nestingLevel] = frame->createBacklink(prevFrame);
        }
    }

    /**
     * Update the display for a return from a routine at a given nesting level.
     * @param nestingLevel the nesting level.
     */
    void returnUpdate(const int nestingLevel)
    {
        int topIndex = display.size() - 1;
        StackFrame *frame = display[nestingLevel];     // pop off this frame
        StackFrame *prevFrame = frame->getBacklink();  // previous frame

        // Point the element at that nesting level to the previous stack frame.
        if (prevFrame != nullptr) display[nestingLevel] = prevFrame;

        // The top element has become null, so remove it.
        else if (nestingLevel == topIndex)
        {
            display.erase(display.begin() + topIndex);
        }
    }
};

}}  // namespace backend::interpreterl

#endif /* RUNTIMEDISPLAY_H_ */
