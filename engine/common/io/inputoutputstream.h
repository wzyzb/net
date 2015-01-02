#ifndef _INPUTOUTPUTSTREAM_H_
#define _INPUTOUTPUTSTREAM_H_

#include "inputstream.h"
#include "outputstream.h"

class IInputOutputStream : public IInputStream,public IOutputStream
{
public :
	virtual ~IInputOutputStream() {}
};

#endif
