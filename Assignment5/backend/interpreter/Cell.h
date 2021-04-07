/**
 * <h1>Cell</h1>
 *
 * <p>The interpreter's runtime memory cell.</p>
 *
 * <p>Copyright (c) 2020 by Ronald Mak</p>
 * <p>For instructional purposes only.  No warranties.</p>
 */
#ifndef BACKEND_INTERPRETER_CELL_H_
#define BACKEND_INTERPRETER_CELL_H_

#include "antlr4-runtime.h"

#include "../../Object.h"

namespace backend { namespace interpreter {

class Cell
{
private:
    Object value = nullptr;  // value contained in the memory cell

public:
    /**
     * Default constructor.
     */
    Cell() : value(nullptr) {}

    /**
     * Constructor.
     * @param value the value for the cell.
     */
    Cell(const Object value) : value(value) {}

    /**
     * Get the value in the cell.
     * @return the value.
     */
    Object getValue() { return value; }

    /**
     * Set a new value into the cell.
     * @param value the new value.
     */
    void setValue(const Object value) { this->value = value; }
};

}}  // namespace backend::interpreter

#endif /* BACKEND_INTERPRETER_CELL_H_ */
