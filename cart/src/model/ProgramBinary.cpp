#include "ProgramBinary.h"

void ProgramBinary::Create(const string file_path)
{
    string src;
    ReadFile(file_path, &src);
    return;
}
