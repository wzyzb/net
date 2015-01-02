#include <db/oracle/oraclepublic.h>
#include <util/stringfun.h>

using namespace avocado_oracle;

const int oracletypes[][2] = {
 {oracle::occi::OCCI_SQLT_CHR,TYPE_STRING},
 {oracle::occi::OCCI_SQLT_STR,TYPE_STRING},
 {oracle::occi::OCCI_SQLT_AFC,TYPE_STRING},
 {oracle::occi::OCCI_SQLT_NUM,TYPE_INT},
 {oracle::occi::OCCIUNSIGNED_INT,TYPE_LONG64},
 {oracle::occi::OCCIDOUBLE,TYPE_DOUBLE},
 {oracle::occi::OCCI_SQLT_DAT,TYPE_DATE},
 {oracle::occi::OCCI_SQLT_DATE,TYPE_DATE},
 {oracle::occi::OCCI_SQLT_TIME,TYPE_TIME},
 {oracle::occi::OCCI_SQLT_TIMESTAMP,TYPE_TIMESTAMP}, 
 };

const int occitypes[][2] = {
 {oracle::occi::OCCICHAR,TYPE_STRING},
 {oracle::occi::OCCISTRING,TYPE_STRING},
 {oracle::occi::OCCIINT,TYPE_INT},
 {oracle::occi::OCCINUMBER,TYPE_LONG64},
 {oracle::occi::OCCIDOUBLE,TYPE_DOUBLE},
 {oracle::occi::OCCIDATE,TYPE_DATE},
 {oracle::occi::OCCIDATE,TYPE_TIME},
 {oracle::occi::OCCITIMESTAMP,TYPE_TIMESTAMP}, 
 };

oracle::occi::Type avocado_oracle::toOracleType(EDATATYPE nDataType)
{
 for ( size_t i = 0 ; i < sizeof(oracletypes) / sizeof(int[2]) ; i ++ )
 {
  if ( oracletypes[i][1] == nDataType )
   return (oracle::occi::Type)oracletypes[i][0];
 }
 string errmsg = "DataType Un Supported" + IntToStr(nDataType);
 throw CSQLException(0x03,errmsg);
}

EDATATYPE avocado_oracle::toOracleType(oracle::occi::Type nSqlType)
{
 for ( size_t i = 0 ; i < sizeof(oracletypes) / sizeof(int[2]) ; i ++ )
 {
  if ( oracletypes[i][0] == nSqlType )
   return (EDATATYPE)oracletypes[i][1];
 }
 string errmsg = "DataType Un Supported" + IntToStr(nSqlType);
 throw CSQLException(0x03,errmsg);
}

oracle::occi::Type avocado_oracle::toOcciType(EDATATYPE nDataType)
{
 for ( size_t i = 0 ; i < sizeof(occitypes) / sizeof(int[2]) ; i ++ )
 {
  if ( occitypes[i][1] == nDataType )
   return (oracle::occi::Type)occitypes[i][0];
 }
 string errmsg = "DataType Un Supported" + IntToStr(nDataType);
 throw CSQLException(0x03,errmsg);
}

EDATATYPE avocado_oracle::toOcciType(oracle::occi::Type nSqlType)
{
 for ( size_t i = 0 ; i < sizeof(occitypes) / sizeof(int[2]) ; i ++ )
 {
  if ( occitypes[i][0] == nSqlType )
   return (EDATATYPE)occitypes[i][1];
 }
 string errmsg = "DataType Un Supported" + IntToStr(nSqlType);
 throw CSQLException(0x03,errmsg);
}

//Bytes 与 Long64 的类型转换
Long64 avocado_oracle::bytesToLong64(Bytes& bt)
{
	if( bt.isNull() )
		return (Long64)-1; //return -1 if NULL

	const unsigned int nlength = 8;
	unsigned char pOutBuffer[nlength];	
	memset(pOutBuffer, 0x00, nlength);
	bt.getBytes( (unsigned char*)pOutBuffer, (nlength > bt.length() ? bt.length() : nlength) );

	Long64 Value = 0x00;
	for(int i=nlength-1; i>=0; i--)
	{
		Value <<= 8; 
		Value += pOutBuffer[i];			
	}
	return 	Value;	
}


