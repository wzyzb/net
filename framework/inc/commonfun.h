#ifndef _COMMONFUN_H_
#define _COMMONFUN_H_

#include <public.h>

/**
 *  产生指定范围内的随机数, [nMin, nMax]
 */
inline int rand_range(int nMin, int nMax)
{
	if ( nMax <= nMin )
		return nMin;

    return ( rand() % (nMax + 1 - nMin) + nMin );
}

inline void splitString(const std::string& strSrc, const std::string& strToken, std::vector<uint16>& vecResult, int nBase = 10)
{
	vecResult.clear();
	std::string::size_type nBegin = 0;
	std::string::size_type nEnd   = 0;

	while( (nBegin = strSrc.find_first_not_of(strToken.c_str(), nEnd) ) != std::string::npos )
	{
		nEnd = strSrc.find_first_of(strToken.c_str(), nBegin);
		vecResult.push_back( (uint16)strtol( strSrc.substr(nBegin, nEnd - nBegin).c_str(), NULL, nBase ) );
	}
}

/**
 * 分割字符串
 * @param nBase 多少进制进行转换
 */
inline void splitString(const std::string& strSrc, const std::string& strToken, std::vector<uint8>& vecResult, int nBase = 10)
{
	vecResult.clear();
	std::string::size_type nBegin = 0;
	std::string::size_type nEnd   = 0;

	while( (nBegin = strSrc.find_first_not_of(strToken.c_str(), nEnd) ) != std::string::npos )
	{
		nEnd = strSrc.find_first_of(strToken.c_str(), nBegin);
		vecResult.push_back( (uint8)strtol( strSrc.substr(nBegin, nEnd - nBegin).c_str(), NULL, nBase ) );
	}
}

/**
* 分割字符串
* @param nBase 多少进制进行转换
*/
inline void splitString(const std::string& strSrc, const std::string& strToken, std::vector<uint32>& vecResult, int nBase = 10)
{
	vecResult.clear();
	std::string::size_type nBegin = 0;
	std::string::size_type nEnd   = 0;

	while( (nBegin = strSrc.find_first_not_of(strToken.c_str(), nEnd) ) != std::string::npos )
	{
		nEnd = strSrc.find_first_of(strToken.c_str(), nBegin);
		vecResult.push_back( (uint32)strtol( strSrc.substr(nBegin, nEnd - nBegin).c_str(), NULL, nBase ) );
	}
}

/**
 * 分割字符串
 * @param nBase 多少进制进行转换
 */
inline void splitString(const std::string& strSrc, const std::string& strToken, std::vector<std::string>& vecResult)
{
	vecResult.clear();
	std::string::size_type nBegin = 0;
	std::string::size_type nEnd   = 0;

	while( (nBegin = strSrc.find_first_not_of(strToken.c_str(), nEnd) ) != std::string::npos )
	{
		nEnd = strSrc.find_first_of(strToken.c_str(), nBegin);
		vecResult.push_back( strSrc.substr(nBegin, nEnd - nBegin) );
	}
}

class CSafeMathOpt
{
public:
	/**
	 *@
	 */
	static uint32 add( uint32 nNumA, uint32 nNumB )
	{
		uint32 nNumC = nNumA + nNumB;
		if (  nNumC < nNumA || nNumC < nNumB )
			return MAX_UINT32;
		else
			return nNumC;
	}
	/**
	 *@ 
	 */
	static uint32 sub( uint32 nNumA, uint32 nNumB )
	{
		if ( nNumA > nNumB )
			return nNumA - nNumB;
		else
			return 0;
	}
	/**
	 *@ 
	 */
	static uint32 diff( uint32 nNumA, uint32 nNumB )
	{
		if ( nNumA > nNumB )
			return nNumA - nNumB;
		else
			return nNumB - nNumA;
	}
	/**
	 *@
	 */
	static uint32 addByCircle( uint32 nNumA, uint32 nNumB )
	{
		uint32 nNumC = nNumA + nNumB;
		if (  nNumC < nNumA || nNumC < nNumB )
			return 0;
		else
			return nNumC;
	}
	/**
	 *@
	 */
	static uint32 div( uint32 nNumA, uint32 nNumB )
	{
		nNumB = ( nNumB == 0 ) ? ( 1 ) : ( nNumB );
		return nNumA / nNumB;
	}
	/**
	 *@ 
	 */
	static uint32 mul( uint32 nNumA, uint32 nNumB )
	{
		if ( nNumA == 0 || nNumB == 0 )
			return 0;

		if ( ( MAX_UINT32 / nNumA ) < nNumB )
			return MAX_UINT32;
		else
			return nNumA * nNumB;
	}
};



/**
 *@
 */
inline uint8 getMax( uint8 nNumA, uint8 nNumB )
{
	return ( nNumA > nNumB ) ? nNumA : nNumB;
}

/**
 *@
 */
inline uint32 getMax( uint32 nNumA, uint32 nNumB )
{
	return ( nNumA > nNumB ) ? nNumA : nNumB;
}

/**
 *@
 */
inline uint32 getMin( uint32 nNumA, uint32 nNumB )
{
	return ( nNumA < nNumB ) ? nNumA : nNumB;
}


#endif 

