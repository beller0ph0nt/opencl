#ifndef __PROGRAM_SOURCE_H
#define __PROGRAM_SOURCE_H

#include "Program.h"

class ProgramSource : public Program
{
public:
    ProgramSource(Context *context, const string file_path):Program(context)
    {
        try
        {
            Create(file_path);
        }
        catch (int e)
        {
            throw e;
        }
    }
protected:
    void Create(const string file_path);
};

#endif // __PROGRAM_SOURCE_H
