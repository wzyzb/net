/*
保存所有元数据
*/
#if !defined(_METADATA_H_)
#define _METADATA_H_
namespace mysql
{
//	class CMetaData
//	{
//	private :
//		MYSQL_BIND m_Param;
//	public :
//		/**
//		 *
//		 */
//		CMetaData();
//		/**
//		 *
//		 */
//		~CMetaData();
//		/**
//		 * 
//		 */
//		MYSQL_BIND* MYSQL_BIND1();
//		/**
//		 * 清空缓冲
//		 */
//		void clear();
//	};

	class CResultData
	{
	public :
		//本地数据类型
		EDATATYPE m_nDataType;
		//字段名
		string m_ColumnName;
	};
}
#endif
