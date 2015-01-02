#ifndef _ENDPOINTTASK_H_
#define _ENDPOINTTASK_H_

#include <framework/bussiness.h>
#include <framework/daserver.h>

class CDAServer;
typedef CRefObject<CDAServer> CDAServerPtr;

class CEndPointTask : public IBussiness
{
public:
    CEndPointTask();
    virtual ~CEndPointTask();
    
    virtual void execute();

	void setCurrent(CCurrentPtr p);
    CCurrentPtr getCurrent();

	void setServantName(uint32 nServantName);
	virtual uint32 getServantName();

    void setDAServer(CDAServerPtr p);
    CDAServerPtr getDAServer();

    void setHost( const std::string& strHost );
    std::string getHost();

    void setPort( uint16 nPort );
    uint16 getPort();

    void setUseEncrypt( bool b );
    bool getUseEncrypt();

    void setMask( uint32 nMask );
    uint32 getMask();

	void setParam( void* Param );
	void* getParam();

    void setEndPointHandler( IEndPointHandlerPtr pEndPointHandler );
    IEndPointHandlerPtr getEndPointHandler();

protected:
	friend class CDAServer;

private:
    bool isIP(const char* pHost);

    CDAServerPtr m_pDAServer;
	std::string  m_strHost;
    uint16       m_nPort;
    bool         m_bUseEncrypt;
    uint32       m_nMask;
    IEndPointHandlerPtr m_pEndPointHandler;
    CCurrentPtr  m_pCurrent;
    uint32       m_nServantName;
	void*        m_pParam;
};

typedef CRefObject<CEndPointTask> CEndPointTaskPtr;

#endif 

