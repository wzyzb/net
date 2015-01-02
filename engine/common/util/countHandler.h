#ifndef _COUNTHANDLER_H_
#define _COUNTHANDLER_H_

#include "public.h"
#include "handler.h"
#include "automicOper.h"

class CCountHandler
{
public:
    CCountHandler(const std::string& name);
    virtual ~CCountHandler();
    virtual void print(IIOHandler* handle);

private:
    CCountHandler()
    {

    }

    CCountHandler(const CCountHandler&)
    {

    }
    std::string  _name;
};

#endif