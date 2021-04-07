/**
 * <h1>StackFrame</h1>
 *
 * <p>The runtime stack frame.</p>
 *
 * <p>Copyright (c) 2020 by Ronald Mak</p>
 * <p>For instructional purposes only.  No warranties.</p>
 */
#ifndef STACKFRAME_H_
#define STACKFRAME_H_

#include <string>
#include <vector>

#include "intermediate/symtab/SymtabEntry.h"
#include "MemoryMap.h"

namespace backend { namespace interpreter {

using namespace std;
using namespace intermediate::symtab;
using namespace intermediate::type;

class StackFrame
{
private:
    StackFrame  *backlink;   // back link to the previous stack frame
    SymtabEntry *routineId;  // symbol table entry of the routine's name
    int nestingLevel;        // scope nesting level of this stack frame
    MemoryMap *memoryMap;    // memory map of this stack frame

public:
    /**
     * Constructor.
     * @param routineId the symbol table entry of the routine's name.
     */
    StackFrame(SymtabEntry *routineId)
        : backlink(nullptr), routineId(routineId)
    {
        Symtab *symtab = routineId->getRoutineSymtab();
        nestingLevel = symtab->getNestingLevel();
        memoryMap = new MemoryMap(symtab);
    }

    /**
     * Destructor.
     */
    virtual ~StackFrame() { delete memoryMap; }

    /**
     * Get the symbol table entry of the routine's name.
     * @return the symbol table entry.
     */
    SymtabEntry *getRoutineId() const { return routineId; }

    /**
     * Get the memory cell for the given name from the memory map.
     * @param name the name.
     * @return the cell.
     */
    Cell *getCell(string& name) { return memoryMap->getCell(name); }

    /**
     * Replace the memory cell with the given name in the memory map.
     * @param name the name.
     * @param cell the replacement cell.
     */
    void replaceCell(string name, Cell *cell)
    {
        memoryMap->replaceCell(name, cell);
    }

    /**
     * Get the list of all the name-value pairs in the memory map.
     * @return the list.
     */
    vector<pair<string, Cell*>> getAllPairs()
    {
        return memoryMap->getAllPairs();
    }

    /**
     * Get the scope nesting level.
     * @return the nesting level.
     */
    int getNestingLevel() const { return nestingLevel; }

    /**
     * Get the stack frame to which this frame is dynamically linked.
     * @return the link.
     */
    StackFrame *getBacklink() const { return backlink; }

    /**
     * Make a backlink from this stack frame to another one.
     * @param frame the stack frame to link to.
     * @return this stack frame.
     */
    StackFrame *createBacklink(StackFrame *frame)
    {
        backlink = frame;
        return this;
    }
};

}}  // namespace backend::interpreter

#endif /* STACKFRAME_H_ */
