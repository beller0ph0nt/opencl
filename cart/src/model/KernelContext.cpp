#include "KernelContext.h"

KernelContext::KernelContext(const string file_path)
{

}

KernelContext::~KernelContext()
{

}

void KernelContext::ReadKernelSrc(const string file_path, string &src)
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
