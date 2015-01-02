#ifndef _HANDLER_H_
#define _HANDLER_H_

class   IIOHandler
{
public:
    IIOHandler();
    virtual ~IIOHandler();

    virtual int write(const void* data ,const int len , void* act ){ return -1; };
    virtual int read(const void* data ,const int len , void* act ){ return -1; };

    virtual int open( const void *handle , void* act ) { return -1; }
	virtual int close(){ return -1; };
};

class IBusinessHandler
{
public:
    IBusinessHandler();
    virtual ~IBusinessHandler();

    virtual bool handlePacket(const void* bufin, const int inlen){return true;};
};


#endif
