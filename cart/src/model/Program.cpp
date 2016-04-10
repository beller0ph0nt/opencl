#include "Program.h"

void Program::ReadFile(const string file_path, string &src)
{
    ifstream file(file_path);

    if (file.is_open())
    {
        stringstream buffer;
        buffer << file.rdbuf();
        src = buffer.str();
        buffer.clear();
    }
    file.close();
}

virtual void ProgramSource::Create(const string file_path)
{
    string src;
    ReadFile(file_path, &src);
}

virtual void ProgramBinary::Create(const string file_path)
{
    string src;
    ReadFile(file_path, &src);
}
