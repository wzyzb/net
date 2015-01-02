#ifndef _IINPUTSTREAM_H_
#define _IINPUTSTREAM_H_

#include "public.h"
#include "util/refshare.h"

class IInputStream : public IRefShare
{
public:
	/**
	 * 析构函数
	 */
    virtual ~IInputStream() {}

	/**
	 * 读取数据
	 * @param pBuf char* 保存数据的指针
	 * @param Buflen uint32 输入输出参数，输入-缓冲区长度，输出-字符长度
	 * @return int 成功与否，0--成功；其他，错误码。
	 */
    virtual int readBytes(char *pBuf, uint32& Buflen) = 0;

	/**
	 * 抛弃指定字节数的数据
	 * @param BytesToPop uint32 要抛弃的字节数
	 * @return int 成功与否，0--成功；其他，错误码。
	 */
    virtual int popBytes(uint32 BytesToPop) = 0;
	/**
	 * 获取有多少可读数据大小，单位：字节
	 */
    virtual uint32 getDataSize() const = 0;
	/**
	 * 获取字节指针
	 */
    virtual const char* c_str() = 0;
};

typedef CRefObject<IInputStream> IInputStreamPtr;

#endif

