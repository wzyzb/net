#ifndef _CBYTESBUFFER_H_
#define _CBYTESBUFFER_H_
#include "inputoutputstream.h"

#define  MEM_INCREASE_SIZE 1024
class CBytesBuffer :public CRefShare,public IInputOutputStream
{
public:
    /*
    *构造函数
    *@Buflen 缓存长度
    */
    CBytesBuffer(int Buflen=0);
	/*
	 *拷贝构造函数
	 *@other CBytesBuffer
	 */
	CBytesBuffer(const CBytesBuffer& other);
    /*
    *析造函数
    */
    virtual ~CBytesBuffer();
    /*
    *读取一段缓存
    *@out:pBuf
    *@in: Buflen长度
    *return :读出的长度
    */
    virtual int readBytes(char *pBuf,uint32& Buflen);
	/**
	 * 读取数据到BytesBuffer中
	 */
	int readBytes(CBytesBuffer& Buffer);
    /*
    *写入一段缓存
    *@in: Buflen长度
    *return :写入的长度
    */
    virtual int writeBytes(const char* pBuf, uint32& Buflen);
    /*
    *写入一段缓存
    *@in: Buflen长度
    *return :写入的长度
    */
    virtual int writeBytes(const CBytesBuffer& Buffer);
    /*
    *移除一段缓存
    */
    virtual int popBytes(uint32 BytesToPop);
    /*
    *返回缓存有效长度
    */
    virtual uint32 getDataSize() const;
    /*
    *返回缓存
    */
    virtual const char* c_str();

	/**
	 * 清空所有数据
	 */
	virtual void clear();
	/**
	 * 重置所有数据
	 */
	void reset();
	/*
	*
	*/
	void incRef();
	/*
	*
	*/
	bool decRef();
    /**
     * 增加内存
     * @param IncrementSize 新增内存大小，默认为0，代表使用bytesbuffer自身的默认增长长度。
     */
    int increase(int32 IncrementSize = MEM_INCREASE_SIZE );
    /**
     * 重新分配内存
     * @param DataSize 重新分配的内存大小
     */
    int resize(uint32 DataSize);
	/**
	 * 压缩内存，释放空余的内存
	 * @return int 剩余的缓冲区空间
	 */
	uint32 compact();
	/**
	 * 获取当前总的缓冲区大小
	 * @return uint32 缓冲区总长度，单位：字节
	 */
	uint32 getBufferSize() const;

	/**
	 * 设置有效数据长度
	 * @param DataSize 要设置的有效数据长度
	 */
	void setDataSize(uint32 DataSize);
	/*
	 * 获取数据的原始起始指针
	 * @return void* 数据的原始起始指针
	 */
	void* getRawData();
	/**
	 * 与Other的BytesBuffer交换数据
	 * @param Other CBytesBuffer 
	 */
	void swap(CBytesBuffer& Other);

private :
	/**
	 * 根据字节对齐，重新分配大小，如果新的空间小于有效数据长度，则会丢失数据
	 * @param DataSize,要分配的大小，单位字节
	 * @return uint32 重新分配后的空间大小
	 */
	uint32 resizeByAlign(uint32 DataSize);

private:
    /*
    *缓存指针
    */
    char *m_pBuf;
    /*
    *缓存总长度
    */
    uint32 m_BufLen;
    /*
    *缓存可用长度
    */
    uint32 m_freeLen;
};

#include "util/objectpool.h"
typedef CPoolObjectSharePtr<CBytesBuffer> CBytesBufferPtr;

#endif

