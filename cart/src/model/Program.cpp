#include <string>
#include <fstream>
#include <sstream>
#include <stdexcept>
using namespace std;

#include "Program.h"

string Program::ReadFile(const string file_path)
{
    ifstream file(file_path);

    if (!file.is_open())
        throw runtime_error("can't open file \"" + file_path + "\"");

    stringstream buffer;
    buffer << file.rdbuf();
    string src = buffer.str();
    buffer.clear();
    file.close();
}
