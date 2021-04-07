#ifndef CONVERTER_CODEGENERATOR_H_
#define CONVERTER_CODEGENERATOR_H_

#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>

namespace backend { namespace converter {

using namespace std;

class CodeGenerator
{
private:
    ofstream objectFile;
    string objectFileName;

    int length;          // length of the code line
    int position;        // position in the code line
    string indentation;  // indentation of the code line
    bool needLF;         // true if a line feed is needed, else false
    string blanks;       // 80 blanks

public:
    /**
     * Default constructor.
     */
    CodeGenerator();

    /**
     * Destructor.
     */
    virtual ~CodeGenerator() {}

    /**
     * Get the name of the object (Java) file.
     * @return the name.
     */
    string getObjectFileName() const { return objectFileName; }

    /**
     * Open the object file.
     * @param programName the name of the program.
     * @param suffix the suffix of the object file name.
     */
    void open(string programName, string suffix);

    /**
     * Close the object file.
     */
    void close() { objectFile.close(); }

    /**
     * Emit a line feed if needed.
     */
    void lfIfNeeded();

    /**
     * Emit some code.
     * @param code the code to emit.
     */
    void emit(const string code);

    /**
     * Emit the start of a new line of code with indentation.
     */
    void emitStart();

    /**
     * Emit the start of a new line with indentation and some code.
     * @param code the code to emit.
     */
    void emitStart(const string code);

    /**
     * Emit a blank code line.
     */
    void emitLine();

    /**
     * Emit a complete line of code with indentation.
     * @param code the code to emit.
     */
    void emitLine(const string code);

    /**
     * Emit some code to end a line.
     * @param code the code to emit.
     */
    void emitEnd(const string code);

    /**
     * Emit a comment line with indentation.
     * @param text the comment text.
     */
    void emitCommentLine(const string text);

    /**
     * Increase the indentation.
     */
    void indent();

    /**
     * Decrease the indentation.
     */
    void dedent();

    /**
     * Mark a position in the code line for indentation of the second line
     * after a potential line split.
     */
    void mark();

    /**
     * Split the code line if its length exceeds the given limit
     * and indent the second line to the marked position.
     * @param limit the limit.
     */
    void split(const int limit);
};

}} // namespace backend::converter

#endif /* CONVERTER_CODEGENERATOR_H_ */
