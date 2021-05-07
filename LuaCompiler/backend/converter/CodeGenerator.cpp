#include "CodeGenerator.h"

namespace backend { namespace converter {

CodeGenerator::CodeGenerator()
    : length(0), position(0), indentation(""), needLF(false)
{
    blanks = "";
    for (int i = 0; i < 8; i++) blanks += "          ";
}

void CodeGenerator::open(string programName, string suffix)
{
    objectFileName = programName + "." + suffix;
    objectFile.open(objectFileName);

    if (!objectFile.is_open())
    {
        cout << "ERROR: Failed to open object file \""
             << objectFileName << "\"." << endl;
        exit(-1);
    }
}

void CodeGenerator::lfIfNeeded()
{
    if (needLF)
    {
        objectFile << endl;
        objectFile.flush();
        length = 0;
        needLF = false;
    }
}

void CodeGenerator::emit(const string code)
{
    objectFile << code;
    objectFile.flush();
    length += code.length();
    needLF = true;
}

void CodeGenerator::emitStart()
{
    lfIfNeeded();
    emit(indentation);
    position = 0;
}

void CodeGenerator::emitStart(const string code)
{
    lfIfNeeded();
    emit(indentation + code);
    position = 0;
}

void CodeGenerator::emitLine()
{
    lfIfNeeded();
    objectFile << endl;
    objectFile.flush();

    length = 0;
    position = 0;
    needLF = false;
}

void CodeGenerator::emitLine(const string code)
{
    lfIfNeeded();
    objectFile << indentation << code << endl;
    objectFile.flush();

    length = 0;
    position = 0;
    needLF = false;
}

void CodeGenerator::emitEnd(const string code)
{
    objectFile << code << endl;
    objectFile.flush();

    length = 0;
    position = 0;
    needLF = false;
}

void CodeGenerator::emitCommentLine(const string text)
{
    emitLine(indentation + "// " + text);
    needLF = false;
}

void CodeGenerator::indent()
{
    indentation += "    ";
}

void CodeGenerator::dedent()
{
    indentation = indentation.substr(0, indentation.length()-4);
}

void CodeGenerator::mark()
{
    position = length;
}

void CodeGenerator::split(const int limit)
{
    if (length > limit)
    {
        objectFile << endl;
        objectFile << blanks.substr(0, position);
        objectFile.flush();

        length = position;
        position = 0;
        needLF = false;
    }
}

}} // namespace backend::converter
