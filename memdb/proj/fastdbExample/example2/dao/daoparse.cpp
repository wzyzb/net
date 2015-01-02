#include "daoparse.h"
#include "dao.h"
#include "daostruct.h"

/*

		class	CDaoOrder

*/
CDaoOrderBy::CDaoOrderBy()
{

}

void CDaoOrderBy::addOrderField(const string& FieldName,ORDER_MODE Mode)
{
	dao_order_field field;
	field.fieldname = FieldName;
	field.ordermode = Mode;
	this->m_Order.order_fields.push_back(field);
}

/*

		class	CDaoExpression

*/
CDaoExpression& CDaoExpression::operator =(const string& FieldName)
{
	this->m_Expression.name = FieldName;
	this->m_Expression.params.clear();
	m_Expression.compmode = OPER_UNKNOW;
	return *this;
}

CDaoExpression::CDaoExpression(const string& FieldName)
{
	m_Expression.name = FieldName;
}

CDaoExpression& CDaoExpression::compare(PARAM_OPERMODE CompareMode)
{
	m_Expression.compmode = CompareMode;
	return *this;
}

CDaoExpression& CDaoExpression::addBindParam()
{
	if ( m_Expression.compmode == OPER_UNKNOW )
		ThrowException<CDaoException>("add param error , compare not set");
	dao_exp_param param;
	param.isBindParam = true;
	m_Expression.params.push_back(param);
	return *this;
}

CDaoExpression& CDaoExpression::addInt(int32 Value)
{
	if ( m_Expression.compmode == OPER_UNKNOW )
		ThrowException<CDaoException>("add param error , compare not set");
	dao_exp_param param;
	param.isBindParam = false;
	param.value.number = Value;
	param.datatype = TYPE_INT;
	m_Expression.params.push_back(param);
	return *this;
}

CDaoExpression& CDaoExpression::addInt64(int64 Value)
{
	if ( m_Expression.compmode == OPER_UNKNOW )
		ThrowException<CDaoException>("add param error , compare not set");
	dao_exp_param param;
	param.isBindParam = false;
	param.value.number = Value;
	param.datatype = TYPE_LONG64;
	m_Expression.params.push_back(param);
	return *this;
}

CDaoExpression& CDaoExpression::addInt16(int16 Value)
{
	if ( m_Expression.compmode == OPER_UNKNOW )
		ThrowException<CDaoException>("add param error , compare not set");
	dao_exp_param param;
	param.isBindParam = false;
	param.value.number = Value;
	param.datatype = TYPE_SHORT;
	m_Expression.params.push_back(param);
	return *this;
}

CDaoExpression& CDaoExpression::addDateTime(const CDateTime& Value)
{
	if ( m_Expression.compmode == OPER_UNKNOW )
		ThrowException<CDaoException>("add param error , compare not set");
	dao_exp_param param;
	param.isBindParam = false;
	param.value.number = Value.getTotalMills()/1000;
	param.datatype = TYPE_TIMESTAMP;
	m_Expression.params.push_back(param);
	return *this;
}

/*

		Class	CDaoSQLParser

*/
CDaoSQLParser& CDaoSQLParser::operator=(const CDaoSQLParser& Other)
{
	clear();
	this->m_SQL = Other.m_SQL;
	this->m_Statement = Other.m_Statement;
	m_nBindParamCount = Other.m_nBindParamCount;
	int nIndex = 0;
	//todo --
// 	for ( list<dao_expression>::iterator j = m_Statement.expressions.begin() ; j != m_Statement.expressions.end() ; j ++ )
// 	{
// 		initBindParam((*j));
// 	}
// 	//copy m_Params的值？
// 	for ( int i = 0 ; i < Other.m_Params.size() ; i ++ )
// 		*m_Params[i] = *Other.m_Params[i];
	return *this;
}

CDaoSQLParser::CDaoSQLParser()
{
	m_nBindParamCount = 0;
}

CDaoSQLParser::~CDaoSQLParser()
{
	clear();
}

// void CDaoQueryBasic::clearParams()
// {
// 	for ( int i = 0 ; i < m_Params.size() ; i ++ )
// 		delete m_Params[i];
// 	m_Params.clear();
// }

int cacluateBindParamCount(list<dao_exp_param>& Params)
{
	int count = 0;
	for ( list<dao_exp_param>::iterator i = Params.begin() ; i!= Params.end() ; i ++ )
	{
		if ( (*i).isBindParam )
			count ++;
	}
	return count;
}

CDaoSQLParser& CDaoSQLParser::operator = (CDaoExpression& Expression)
{
	clear();
	m_nBindParamCount = cacluateBindParamCount(Expression.m_Expression.params);
	m_Statement.expressions.push_back(Expression.m_Expression);
//	initBindParam(this->m_Statement.expressions.back());
	return *this;
}

CDaoSQLParser& CDaoSQLParser::And(CDaoExpression& Expression)
{
	if ( this->m_Statement.expressions.size() == 0 )
		ThrowException<CDaoException>("and expression error ,no first expression");
	this->m_Statement.expressions.back().joinmode = JOIN_AND;
	this->m_Statement.expressions.push_back(Expression.m_Expression);
	m_nBindParamCount += cacluateBindParamCount(Expression.m_Expression.params);
//	initBindParam(this->m_Statement.expressions.back());
	return *this;
}

CDaoSQLParser& CDaoSQLParser::Or(CDaoExpression& Expression)
{
	if ( this->m_Statement.expressions.size() == 0 )
		ThrowException<CDaoException>("and expression error ,no first expression");
	this->m_Statement.expressions.back().joinmode = JOIN_OR;
	this->m_Statement.expressions.push_back(Expression.m_Expression);
	m_nBindParamCount += cacluateBindParamCount(Expression.m_Expression.params);
//	initBindParam(this->m_Statement.expressions.back());
	return *this;
}

CDaoSQLParser& CDaoSQLParser::Orderby(CDaoOrderBy& Order)
{
	this->m_Statement.order = Order.m_Order;
	return *this;
}


int32 CDaoSQLParser::getBindParamCount()
{
	return m_nBindParamCount;
}


extern "C" void set_scan_string(const char* sql);
extern "C" void free_scan_string();
extern "C" int yyparse();
extern "C" statement* g_pStmt;

void CDaoSQLParser::copyExpression(list<dao_expression>& Expressions,const expression* pExpression)
{
	if ( pExpression == NULL )
		return ;
	const expression* pexp = pExpression;
	while ( pexp != NULL )
	{
		dao_expression exp;
		exp.compmode = (PARAM_OPERMODE)pexp->left->paramopermode;
		exp.joinmode = (EXPRESS_JOINMODE)pexp->joinmode;
		exp.name = pexp->left->name;
		valuelist* pvalue = pexp->left->values;
		while ( pvalue != NULL )
		{
			dao_exp_param param;
			switch ( pvalue->valuetype )
			{
				case VALUE_PARAM :
					param.isBindParam = true;
					m_nBindParamCount ++ ;
					break;
				case VALUE_NUMBER :
					param.datatype = TYPE_INT;
					param.value.number = pvalue->value.num;
					break;
				case VALUE_STRING :
					param.datatype = TYPE_STRING;
					param.value.str = pvalue->value.str;
					param.datasize = strlen(pvalue->value.str)+1;
					pvalue->value.str = NULL;
					break;
				default :
					ThrowException<CDaoException>("unknow valuetype[%d]",pvalue->valuetype);
			}
			pvalue = pvalue->pnext;
			exp.params.push_back(param);
		}
		Expressions.push_back(exp);
//		initBindParam(Expressions.back());
		pexp = pexp->right;
	}
}

void CDaoSQLParser::copyOrderList(dao_order& Order,const orderlist* pOrderList)
{
	if ( pOrderList == NULL )
		return ;
	orderfield* pvalue = pOrderList->fields;
	while ( pvalue != NULL )
	{
		dao_order_field field;
		field.fieldname = pvalue->str;
		field.ordermode = (ORDER_MODE)pvalue->ordermode;
		Order.order_fields.push_back(field);
		pvalue = pvalue->next;
	}
}

void CDaoSQLParser::copyStatement(dao_statement& Stmt,const statement* pStmt)
{
	clear();
	if ( pStmt == NULL )
		return ;
	copyOrderList(Stmt.order,pStmt->porder);
	copyExpression(Stmt.expressions,pStmt->expressions);
}

void CDaoSQLParser::prepare(const char* pSQL)
{
	if ( pSQL == NULL || pSQL[0] == 0 )
		return ;
	//转换成小写
	set_scan_string(pSQL);
	yyparse();
	copyStatement(m_Statement,g_pStmt);
	//
	free_scan_string();
	dao_free_statement(g_pStmt);
	this->m_SQL = pSQL;
}

void CDaoSQLParser::clear()
{
//	clearParams();
	m_nBindParamCount = 0;
	this->m_Statement.order.order_fields.clear();
	this->m_Statement.expressions.clear();
}
