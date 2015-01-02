#ifndef _SESSION_H_
#define _SESSION_H_

#include <framework/serial.h>
#include <util/refshare.h>
#include <util/objectpool.h>

struct _t_sessionvalue;

class CSession : public CRefShare
{
public:
    CSession();
    ~CSession();
	/**
	 * 保存session值，如果key已存在，则会替换
	 * @
	 */
    void setInt(int Name, int32 Value);
	void setInt64(int Name, int64 Value);
	void setString(int Name, const char* Value);
	/**
	 * 保存指针，必须是支持引用计数的指针,会在session拥有此指针时候增加对期的引用
	 */
	void setPtr(int Name,IRefShare* Value);
	/**
	 * 是否存在这个key
	 */
	bool isExist(int Name);
    /**
	 * 获取session值
	 * @param Name key名
	 * @param bThrowable 不存在是否抛异常，默认为true
	 * @param bDefault 默认值
	 * @return int32 具体的值
	 */
    int32 getInt(int Name,bool bThrowable = true,int32 nDefault = 0);
    /**
	 * 获取session值
	 * @param Name key名
	 * @param bThrowable 不存在是否抛异常，默认为true
	 * @param bDefault 默认值
	 * @return int64 具体的值
	 */
    int64 getInt64(int Name,bool bThrowable = true,int64 nDefault = 0);
    /**
	 * 获取session值
	 * @param Name key名
	 * @param bThrowable 不存在是否抛异常，默认为true
	 * @param bDefault 默认值
	 * @return int32 具体的值
	 */
    const char* getString(int Name,bool bThrowable = true,const char* pDefault = "");
    /**
	 * 获取session值
	 * @param Name key名
	 * @param bThrowable 不存在是否抛异常，默认为true
	 * @param bDefault 默认值
	 * @return IRefShare* 具体的值
	 */
    IRefShare* getPtr(int Name,bool bThrowable = true,IRefShare* pDefault = NULL);
	/**
	 * 删除session值
	 * @return bool 是否存在此值
	 */
	bool remove(int Name);
    
    void setSessionID(uint32 nID);

    uint32 getSessionID();
	void reset();

private:
    uint32 m_nSessionID;
	//内部保存
	map<int,struct _t_sessionvalue> m_Values;
};

typedef CPoolObjectSharePtr<CSession> CSessionPtr;

#endif 
