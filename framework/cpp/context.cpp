#include <framework/context.h>

void CContext::set(const char* Name, IRefShare* Value)
{
}

IRefShare* CContext::get(const char* Name)
{
	return NULL;
}

bool CContext::serial(char* pOut, int &Length)
{
    return false;
}

bool CContext::unserial(const char* pIn, int Length)
{
    return false;
}

bool CContext::serial(IOutputStreamPtr Output)
{
    return false;
}

bool CContext::unserial(IInputStreamPtr Input)
{
    return false;
}
