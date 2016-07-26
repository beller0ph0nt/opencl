#ifndef __PROGRAM_SOURCE_H
#define __PROGRAM_SOURCE_H

#include "Program.h"

class ProgramSource : public Program
{
private:
    string _source;
    string _source_path;

public:
    ProgramSource(Context *context, const string source_path);
};

#endif // __PROGRAM_SOURCE_H
