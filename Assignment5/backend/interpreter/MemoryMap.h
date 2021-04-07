/**
 * <h1>MemoryMap</h1>
 *
 * <p>The interpreter's runtime memory map.</p>
 *
 * <p>Copyright (c) 2020 by Ronald Mak</p>
 * <p>For instructional purposes only.  No warranties.</p>
 */
#ifndef MEMORYMAPIMPL_H_
#define MEMORYMAPIMPL_H_

#include <string>
#include <vector>
#include <map>

#include "antlr4-runtime.h"

#include "Cell.h"
#include "intermediate/symtab/Symtab.h"
#include "intermediate/type/Typespec.h"

namespace backend { namespace interpreter {

using namespace std;
using namespace intermediate::symtab;
using namespace intermediate::type;

class MemoryMap
{
private:
    unordered_map<string, Cell *> contents;

    /**
     * Make an allocation for a value of a given data type for a memory cell.
     * @param type the data type.
     * @return the allocation.
     */
    Object allocateCellValue(Typespec *type)
    {
        Form form = type->getForm();

        switch (form)
        {
            case ARRAY:  return allocateArrayCells(type);
            case RECORD: return allocateRecordMap(type);

            default: return nullptr;  // uninitialized scalar value
        }
    }

    /**
     * Allocate the memory cells of an array.
     * @param type the array type.
     * @return the allocation.
     */
    Object allocateArrayCells(Typespec *type)
    {
        int elmtCount = type->getArrayElementCount();
        Typespec *elmtType = type->getArrayElementType();
        vector<Cell *> *array = new vector<Cell *>;

        array->reserve(elmtCount);
        for (int i = 0; i < elmtCount; ++i)
        {
            array->push_back(new Cell(allocateCellValue(elmtType)));
        }

        return array;
    }

    /**
     * Allocate the memory map for a record.
     * @param type the record type.
     * @return the allocation.
     */
    MemoryMap *allocateRecordMap(Typespec *type)
    {
        Symtab *symtab = type->getRecordSymtab();
        return new MemoryMap(symtab);
    }

public:
    /**
     * Default constructor.
     */
    MemoryMap() {}

    /**
     * Constructor.
     * Create a memory map and allocate its memory cells
     * based on the entries in a symbol table.
     * @param symtab the symbol table.
     */
    MemoryMap(Symtab *symtab)
    {
        vector<SymtabEntry *> entries = symtab->sortedEntries();

        // Loop for each entry of the symbol table.
        for (SymtabEntry *entry : entries)
        {
            Kind kind = entry->getKind();

            switch (kind)
            {
                // Not a reference parameter: Allocate cells for the datatype
                //                            in the memory map.
                case VARIABLE:
                case FUNCTION:
                case VALUE_PARAMETER:
                case RECORD_FIELD:
                {
                    string name = entry->getName();
                    Typespec *typespec = entry->getType();
                    Object value = allocateCellValue(typespec);
                    contents[name] = new Cell(value);
                    break;
                }

                // Reference parameter: Allocate a single cell for a reference.
                case REFERENCE_PARAMETER:
                {
                    string name = entry->getName();
                    contents[name] = new Cell();
                    break;
                }

                default: break;  // should never get here
            }
        }
    }

    /**
     * Destructor.
     */
    ~MemoryMap()
    {
        unordered_map<string, Cell *>::iterator it;

        for (it = contents.begin(); it != contents.end(); it++)
        {
            delete it->second;
        }
    }

    /*
     * Get the memory map contents.
     * @return the contents.
     */
    unordered_map<string, Cell *> *getContents() { return &contents; }

    /**
     * Return the memory cell with the given name.
     * @param name the name.
     * @return the cell.
     */
    Cell *getCell(const string& name) { return contents[name];  }

    /**
     * Replace the memory cell with the given name.
     * @param name the name.
     * @param cell the replacement cell.
     */
    void replaceCell(string name, Cell *cell) { contents[name] = cell; }

    /**
     * Return the list of all the name-value pairs.
     * @return the list.
     */
    vector<pair<string, Cell*>> getAllPairs()
    {
        vector<pair<string, Cell*>> pairs;
        unordered_map<string, Cell *>::iterator it;

        for (it = contents.begin(); it != contents.end(); it++)
        {
            pairs.push_back(*it);
        }

        return pairs;
    }

};

}}  // namespace ::backend::interpreter

#endif /* MEMORYMAPIMPL_H_ */
