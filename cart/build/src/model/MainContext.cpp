#include "MainContext.h"

MainContext::MainContext()
{
    data = new MainContextData;
}

MainContext::~MainContext()
{
    delete data;
}
