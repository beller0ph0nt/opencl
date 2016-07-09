#ifndef __PROGRAM_BINARY_H
#define __PROGRAM_BINARY_H

#include "Program.h"

class ProgramBinary : public Program
{
public:
    ProgramBinary(Context *context, const string file_path):Program(context)
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

#endif // __PROGRAM_BINARY_H

