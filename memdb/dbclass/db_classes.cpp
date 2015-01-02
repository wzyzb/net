#include "db_classes.h"
#include <util/memutil.h>
#include "global.h"
#include "playercachedmgr.h"

#ifndef max
#define max(a,b) ( (a) > (b) ? (a) : (b))
#endif
REGISTER(_t_account);
uint64 account::g_Key = 0;
uint64 account::generateKey()
{
	return ++g_Key;
}

 account::account()
{
	m_TableName = m_Name="account";
	reset();
	m_bHasMoreRecord = false;
	m_pRecord=NULL;
}

void account::reset()
{
	memset(&m_Record,0,sizeof(m_Record));
	m_Record.UserName=this->UserName;
	UserName[0] = 0;
	m_Record.Diamond = 0;
	m_Record.RecentRecharge = 0;
	m_Record.RechargeTimes = 0;
	m_Record.RechargeTotal = 0;
	m_pRecord=NULL;
}

CDBDaoImpl* account::clone()
{
	return new account(*this);
}

 account::account(const account& Other)
{
	m_Record = Other.m_Record;
	m_TableName = Other.m_TableName;
	memcpy(UserName,Other.UserName,sizeof(UserName));
	m_Record.UserName=this->UserName;
	m_Name=Other.m_Name;
}

bool account::check(const CMysqlDaoImpl* pOther) const
{
	const account* p = static_cast<const account*>(pOther);
	return m_Record.ID<p->m_Record.ID;
}

bool account::readMDB()
{
	return true;
}

void account::readDB(CResultSet& ResultSet)
{
	reset();
	if ( ! ResultSet.isNull(1) ) m_Record.ID = ResultSet.getInt64(1);
	if ( ! ResultSet.isNull(2) ) m_Record.AccountID = ResultSet.getInt64(2);
	if ( ! ResultSet.isNull(3) ) snprintf(UserName,sizeof(UserName),"%s",ResultSet.getString(3).c_str());
	UserName[25]=0;
	if ( ! ResultSet.isNull(4) ) m_Record.LastLoginId = ResultSet.getInt64(4);
	if ( ! ResultSet.isNull(5) ) m_Record.Diamond = ResultSet.getInt(5);
	if ( ! ResultSet.isNull(6) ) m_Record.RecentRecharge = ResultSet.getInt(6);
	if ( ! ResultSet.isNull(7) ) m_Record.RechargeTimes = ResultSet.getInt(7);
	if ( ! ResultSet.isNull(8) ) m_Record.RechargeTotal = ResultSet.getInt(8);
	return ;
}

bool account::nextMDB()
{
	if ( ! m_bHasMoreRecord )
		return false;
	memcpy(&m_Record,m_pRecord,sizeof(m_Record));
	snprintf((char*)UserName,sizeof(UserName),"%s",m_Record.UserName);
	m_pRecord = m_Cursor.next();	m_bHasMoreRecord = ( NULL != m_pRecord );
	return true;
}

int account::select(dbQuery& Query)
{
	m_bHasMoreRecord = false;
	int count = m_Cursor.select(Query,dbCursorViewOnly);
	if ( count == 0 )
		return count;
	 m_bHasMoreRecord = true;
	m_pRecord = m_Cursor.get();
	return count;
}

bool account::insertMDB()
{
	if ( m_Record.ID == 0 )
		m_Record.ID=generateKey();
	m_Record.UserName = UserName;
	dbCursor<_t_account> slcursor;
	dbQuery query;
	query,"ID=",m_Record.ID;
	if ( slcursor.select(query,dbCursorViewOnly) > 0 )
		ThrowException<CDaoException>("insert error[account primary key(%d) already exists]",m_Record.ID);
	::insert(m_Record);
	return true;
}

bool account::insertMDB(const account_record& record)
{
	m_Record.ID = record.ID;
	if ( m_Record.ID == 0 )
		m_Record.ID=generateKey();
	m_Record.AccountID = record.AccountID;
	snprintf((char*)UserName, sizeof(UserName), "%s", record.UserName.c_str());
	m_Record.UserName = UserName;
	m_Record.LastLoginId = record.LastLoginId;
	m_Record.Diamond = record.Diamond;
	m_Record.RecentRecharge = record.RecentRecharge;
	m_Record.RechargeTimes = record.RechargeTimes;
	m_Record.RechargeTotal = record.RechargeTotal;
	dbCursor<_t_account> slcursor;
	dbQuery query;
	query,"ID=",m_Record.ID;
	if ( slcursor.select(query,dbCursorViewOnly) > 0 )
		ThrowException<CDaoException>("insert error[account primary key(%d) already exists]",m_Record.ID);
	::insert(m_Record);
	return true;
}

bool account::insertDB(CConnection& Conn)
{
	CStatementPtr stmt = Conn.prepareStatement("insert into account values(?,?,?,?,?,?,?,?)");
	stmt->setUInt64(1,m_Record.ID);
	stmt->setUInt64(2,m_Record.AccountID);
	stmt->setString(3,UserName);
	stmt->setUInt64(4,m_Record.LastLoginId);
	stmt->setUInt(5,m_Record.Diamond);
	stmt->setUInt(6,m_Record.RecentRecharge);
	stmt->setUInt(7,m_Record.RechargeTimes);
	stmt->setUInt(8,m_Record.RechargeTotal);
	bool retval = stmt->executeUpdate() > 0;
	m_Record.ID=stmt->getInsertID(NULL);
	return retval;
}

string account::generateInsertSQL()
{
	char tmpsql[1025];
	string sql = "insert into account values (";
	snprintf(tmpsql,sizeof(tmpsql),"%d",m_Record.ID);
	sql+=tmpsql;
	sql += ",";
	snprintf(tmpsql,sizeof(tmpsql),"%d",m_Record.AccountID);
	sql+=tmpsql;
	sql += ",";
	snprintf(tmpsql,sizeof(tmpsql),"`%s`",UserName);
	sql+=tmpsql;
	sql += ",";
	snprintf(tmpsql,sizeof(tmpsql),"%d",m_Record.LastLoginId);
	sql+=tmpsql;
	sql += ",";
	snprintf(tmpsql,sizeof(tmpsql),"%d",m_Record.Diamond);
	sql+=tmpsql;
	sql += ",";
	snprintf(tmpsql,sizeof(tmpsql),"%d",m_Record.RecentRecharge);
	sql+=tmpsql;
	sql += ",";
	snprintf(tmpsql,sizeof(tmpsql),"%d",m_Record.RechargeTimes);
	sql+=tmpsql;
	sql += ",";
	snprintf(tmpsql,sizeof(tmpsql),"%d",m_Record.RechargeTotal);
	sql+=tmpsql;
	sql+=");";
	return sql;
}

bool account::updateMDB()
{
	dbCursor<_t_account> upcursor;
	dbQuery query;
	query,"ID=",m_Record.ID;
	if ( upcursor.select(query,dbCursorForUpdate) <= 0 )
		ThrowException<CDaoException>("update error not such key record");
	upcursor->AccountID=m_Record.AccountID;
	upcursor->UserName=UserName;
	upcursor->LastLoginId=m_Record.LastLoginId;
	upcursor->Diamond=m_Record.Diamond;
	upcursor->RecentRecharge=m_Record.RecentRecharge;
	upcursor->RechargeTimes=m_Record.RechargeTimes;
	upcursor->RechargeTotal=m_Record.RechargeTotal;
	upcursor.update();
	return true;
}

bool account::updateDB(CConnection& Conn)
{
	CStatementPtr stmt = Conn.prepareStatement("update account set AccountID=?,UserName=?,LastLoginId=?,Diamond=?,RecentRecharge=?,RechargeTimes=?,RechargeTotal=? where ID=?");
	stmt->setUInt64(1,m_Record.AccountID);
	stmt->setString(2,UserName);
	stmt->setUInt64(3,m_Record.LastLoginId);
	stmt->setUInt(4,m_Record.Diamond);
	stmt->setUInt(5,m_Record.RecentRecharge);
	stmt->setUInt(6,m_Record.RechargeTimes);
	stmt->setUInt(7,m_Record.RechargeTotal);
	stmt->setUInt64(8,m_Record.ID);
	return stmt->executeUpdate() > 0;
}

string account::generateUpdateSQL()
{
	char tmpsql[1025];
	string sql = "update account set ";
	snprintf(tmpsql,sizeof(tmpsql),"AccountID=%d",m_Record.AccountID);
	sql+=tmpsql;
	sql += ",";
	snprintf(tmpsql,sizeof(tmpsql),"UserName=`%s`",UserName);
	sql+=tmpsql;
	sql += ",";
	snprintf(tmpsql,sizeof(tmpsql),"LastLoginId=%d",m_Record.LastLoginId);
	sql+=tmpsql;
	sql += ",";
	snprintf(tmpsql,sizeof(tmpsql),"Diamond=%d",m_Record.Diamond);
	sql+=tmpsql;
	sql += ",";
	snprintf(tmpsql,sizeof(tmpsql),"RecentRecharge=%d",m_Record.RecentRecharge);
	sql+=tmpsql;
	sql += ",";
	snprintf(tmpsql,sizeof(tmpsql),"RechargeTimes=%d",m_Record.RechargeTimes);
	sql+=tmpsql;
	sql += ",";
	snprintf(tmpsql,sizeof(tmpsql),"RechargeTotal=%d",m_Record.RechargeTotal);
	sql+=tmpsql;
	sql += "where ";
	snprintf(tmpsql,sizeof(tmpsql),"ID=%d",m_Record.ID);
	sql+=tmpsql;
	sql+=";";
	return sql;
}

bool account::removeMDB()
{
	dbCursor<_t_account> upcursor;
	dbQuery query;
	query,"ID=",m_Record.ID;
	if ( upcursor.select(query,dbCursorForUpdate) <= 0 )
		ThrowException<CDaoException>("update error not such key record");
	upcursor->AccountID=m_Record.AccountID;
	upcursor->UserName=UserName;
	upcursor->LastLoginId=m_Record.LastLoginId;
	upcursor->Diamond=m_Record.Diamond;
	upcursor->RecentRecharge=m_Record.RecentRecharge;
	upcursor->RechargeTimes=m_Record.RechargeTimes;
	upcursor->RechargeTotal=m_Record.RechargeTotal;
	upcursor.remove();
	return true;
}

bool account::removeRecord(CConnection& Conn)
{
	CStatementPtr stmt = Conn.prepareStatement("delete from account  where ID=?");
	stmt->setUInt64(1,m_Record.ID);
	return stmt->executeUpdate() > 0;
}

string account::generateRemoveSQL()
{
	char tmpsql[1025];
	string sql = "delete from account ";
	sql += " where ";
	snprintf(tmpsql,sizeof(tmpsql),"ID=%d",m_Record.ID);
	sql+=tmpsql;
	sql+=";";
	return sql;
}

bool account::removeDB(CConnection& Conn)
{
	return removeRecord(Conn);
}

bool account::removeAll()
{
	account dao;
	dbCursor<_t_account> upcursor(dbCursorForUpdate);
	upcursor.removeAll();
	return true;
}

void account::loadDB(CConnection& Conn)
{
	account dao;
	CStatementPtr stmt = Conn.prepareStatement("select * from account");
	CResultSetPtr resultset = stmt->executeQuery();
	uint64 recordcount = 0;
	while ( resultset->next() )
	{
		recordcount++;
		dao.readDB(*resultset.get());
		dao.insert();
	}
	log_info("tablename[%s] recordcount[" I64_FMT "]", "account", recordcount);
	dao.reset();
	dao.setID(0);
	dao.insertDB(Conn);
	g_Key = dao.getID() - 1;
	dao.removeRecord(Conn);
	log_info("%s id[" I64_FMT "]", "account", g_Key);
}

uint64 account::getKey()
{
	return g_Key;
}

REGISTER(_t_arena);
uint64 arena::g_Key = 0;
uint64 arena::generateKey()
{
	return ++g_Key;
}

 arena::arena()
{
	m_TableName = m_Name="arena";
	reset();
	m_bHasMoreRecord = false;
	m_pRecord=NULL;
}

void arena::reset()
{
	memset(&m_Record,0,sizeof(m_Record));
	m_pRecord=NULL;
}

CDBDaoImpl* arena::clone()
{
	return new arena(*this);
}

 arena::arena(const arena& Other)
{
	m_Record = Other.m_Record;
	m_TableName = Other.m_TableName;
	m_Name=Other.m_Name;
}

bool arena::check(const CMysqlDaoImpl* pOther) const
{
	const arena* p = static_cast<const arena*>(pOther);
	return m_Record.id<p->m_Record.id;
}

bool arena::readMDB()
{
	return true;
}

void arena::readDB(CResultSet& ResultSet)
{
	reset();
	if ( ! ResultSet.isNull(1) ) m_Record.id = ResultSet.getInt64(1);
	if ( ! ResultSet.isNull(2) ) m_Record.player_id = ResultSet.getInt64(2);
	return ;
}

bool arena::nextMDB()
{
	if ( ! m_bHasMoreRecord )
		return false;
	memcpy(&m_Record,m_pRecord,sizeof(m_Record));
	m_pRecord = m_Cursor.next();	m_bHasMoreRecord = ( NULL != m_pRecord );
	return true;
}

int arena::select(dbQuery& Query)
{
	m_bHasMoreRecord = false;
	int count = m_Cursor.select(Query,dbCursorViewOnly);
	if ( count == 0 )
		return count;
	 m_bHasMoreRecord = true;
	m_pRecord = m_Cursor.get();
	return count;
}

bool arena::insertMDB()
{
	if ( m_Record.id == 0 )
		m_Record.id=generateKey();
	dbCursor<_t_arena> slcursor;
	dbQuery query;
	query,"id=",m_Record.id;
	if ( slcursor.select(query,dbCursorViewOnly) > 0 )
		ThrowException<CDaoException>("insert error[arena primary key(%d) already exists]",m_Record.id);
	::insert(m_Record);
	return true;
}

bool arena::insertMDB(const arena_record& record)
{
	m_Record.id = record.id;
	if ( m_Record.id == 0 )
		m_Record.id=generateKey();
	m_Record.player_id = record.player_id;
	dbCursor<_t_arena> slcursor;
	dbQuery query;
	query,"id=",m_Record.id;
	if ( slcursor.select(query,dbCursorViewOnly) > 0 )
		ThrowException<CDaoException>("insert error[arena primary key(%d) already exists]",m_Record.id);
	::insert(m_Record);
	return true;
}

bool arena::insertDB(CConnection& Conn)
{
	CStatementPtr stmt = Conn.prepareStatement("insert into arena values(?,?)");
	stmt->setUInt64(1,m_Record.id);
	stmt->setUInt64(2,m_Record.player_id);
	bool retval = stmt->executeUpdate() > 0;
	m_Record.id=stmt->getInsertID(NULL);
	return retval;
}

string arena::generateInsertSQL()
{
	char tmpsql[1025];
	string sql = "insert into arena values (";
	snprintf(tmpsql,sizeof(tmpsql),"%d",m_Record.id);
	sql+=tmpsql;
	sql += ",";
	snprintf(tmpsql,sizeof(tmpsql),"%d",m_Record.player_id);
	sql+=tmpsql;
	sql+=");";
	return sql;
}

bool arena::updateMDB()
{
	dbCursor<_t_arena> upcursor;
	dbQuery query;
	query,"id=",m_Record.id;
	if ( upcursor.select(query,dbCursorForUpdate) <= 0 )
		ThrowException<CDaoException>("update error not such key record");
	upcursor->player_id=m_Record.player_id;
	upcursor.update();
	return true;
}

bool arena::updateDB(CConnection& Conn)
{
	CStatementPtr stmt = Conn.prepareStatement("update arena set player_id=? where id=?");
	stmt->setUInt64(1,m_Record.player_id);
	stmt->setUInt64(2,m_Record.id);
	return stmt->executeUpdate() > 0;
}

string arena::generateUpdateSQL()
{
	char tmpsql[1025];
	string sql = "update arena set ";
	snprintf(tmpsql,sizeof(tmpsql),"player_id=%d",m_Record.player_id);
	sql+=tmpsql;
	sql += "where ";
	snprintf(tmpsql,sizeof(tmpsql),"id=%d",m_Record.id);
	sql+=tmpsql;
	sql+=";";
	return sql;
}

bool arena::removeMDB()
{
	dbCursor<_t_arena> upcursor;
	dbQuery query;
	query,"id=",m_Record.id;
	if ( upcursor.select(query,dbCursorForUpdate) <= 0 )
		ThrowException<CDaoException>("update error not such key record");
	upcursor->player_id=m_Record.player_id;
	upcursor.remove();
	return true;
}

bool arena::removeRecord(CConnection& Conn)
{
	CStatementPtr stmt = Conn.prepareStatement("delete from arena  where id=?");
	stmt->setUInt64(1,m_Record.id);
	return stmt->executeUpdate() > 0;
}

string arena::generateRemoveSQL()
{
	char tmpsql[1025];
	string sql = "delete from arena ";
	sql += " where ";
	snprintf(tmpsql,sizeof(tmpsql),"id=%d",m_Record.id);
	sql+=tmpsql;
	sql+=";";
	return sql;
}

bool arena::removeDB(CConnection& Conn)
{
	return removeRecord(Conn);
}

bool arena::removeAll()
{
	arena dao;
	dbCursor<_t_arena> upcursor(dbCursorForUpdate);
	upcursor.removeAll();
	return true;
}

void arena::loadDB(CConnection& Conn)
{
	arena dao;
	CStatementPtr stmt = Conn.prepareStatement("select * from arena");
	CResultSetPtr resultset = stmt->executeQuery();
	uint64 recordcount = 0;
	while ( resultset->next() )
	{
		recordcount++;
		dao.readDB(*resultset.get());
		dao.insert();
	}
	log_info("tablename[%s] recordcount[" I64_FMT "]", "arena", recordcount);
	dao.reset();
	dao.setid(0);
	dao.insertDB(Conn);
	g_Key = dao.getid() - 1;
	dao.removeRecord(Conn);
	log_info("%s id[" I64_FMT "]", "arena", g_Key);
}

uint64 arena::getKey()
{
	return g_Key;
}

REGISTER(_t_avatar);
uint64 avatar::g_Key = 0;
uint64 avatar::generateKey()
{
	return ++g_Key;
}

 avatar::avatar()
{
	m_TableName = m_Name="avatar";
	reset();
	m_bHasMoreRecord = false;
	m_pRecord=NULL;
}

void avatar::reset()
{
	memset(&m_Record,0,sizeof(m_Record));
	m_Record.CreateTime = CDateTime().getTotalMills()/1000;
	m_Record.LastLogoutTime = CDateTime().getTotalMills()/1000;
	m_Record.NormalRecruitTime = CDateTime().getTotalMills()/1000;
	m_Record.NormalBoxOpenTime = CDateTime().getTotalMills()/1000;
	m_Record.LastAddVigorTime = CDateTime().getTotalMills()/1000;
	m_pRecord=NULL;
}

CDBDaoImpl* avatar::clone()
{
	return new avatar(*this);
}

 avatar::avatar(const avatar& Other)
{
	m_Record = Other.m_Record;
	m_TableName = Other.m_TableName;
	m_Name=Other.m_Name;
}

bool avatar::check(const CMysqlDaoImpl* pOther) const
{
	const avatar* p = static_cast<const avatar*>(pOther);
	return m_Record.ID<p->m_Record.ID;
}

bool avatar::readMDB()
{
	return true;
}

void avatar::readDB(CResultSet& ResultSet)
{
	reset();
	if ( ! ResultSet.isNull(1) ) m_Record.ID = ResultSet.getInt64(1);
	if ( ! ResultSet.isNull(2) ) m_Record.AccountID = ResultSet.getInt64(2);
	if ( ! ResultSet.isNull(3) ) m_Record.PlayerID = ResultSet.getInt64(3);
	if ( ! ResultSet.isNull(4) ) m_Record.PlayerLevel = ResultSet.getInt(4);
	if ( ! ResultSet.isNull(5) ) m_Record.CreateTime = ResultSet.getTimestamp(5).getTotalMills()/1000;
	if ( ! ResultSet.isNull(6) ) m_Record.LastLogoutTime = ResultSet.getTimestamp(6).getTotalMills()/1000;
	if ( ! ResultSet.isNull(7) ) m_Record.MaxVigor = ResultSet.getInt(7);
	if ( ! ResultSet.isNull(8) ) m_Record.CurVigor = ResultSet.getInt(8);
	if ( ! ResultSet.isNull(9) ) m_Record.Money = ResultSet.getInt(9);
	if ( ! ResultSet.isNull(10) ) m_Record.Experience = ResultSet.getInt(10);
	if ( ! ResultSet.isNull(11) ) m_Record.PlayerJob = ResultSet.getInt(11);
	if ( ! ResultSet.isNull(12) ) m_Record.CurChapter = ResultSet.getInt(12);
	if ( ! ResultSet.isNull(13) ) m_Record.PartnerExp = ResultSet.getInt(13);
	if ( ! ResultSet.isNull(14) ) m_Record.NormalRecruitTime = ResultSet.getTimestamp(14).getTotalMills()/1000;
	if ( ! ResultSet.isNull(15) ) m_Record.NormalBoxOpenTime = ResultSet.getTimestamp(15).getTotalMills()/1000;
	if ( ! ResultSet.isNull(16) ) m_Record.LastAddVigorTime = ResultSet.getTimestamp(16).getTotalMills()/1000;
	if ( ! ResultSet.isNull(17) ) m_Record.SmeltExp = ResultSet.getInt(17);
	if ( ! ResultSet.isNull(18) ) m_Record.ServedPartnerId = ResultSet.getInt(18);
	return ;
}

bool avatar::nextMDB()
{
	if ( ! m_bHasMoreRecord )
		return false;
	memcpy(&m_Record,m_pRecord,sizeof(m_Record));
	m_pRecord = m_Cursor.next();	m_bHasMoreRecord = ( NULL != m_pRecord );
	return true;
}

int avatar::select(dbQuery& Query)
{
	m_bHasMoreRecord = false;
	int count = m_Cursor.select(Query,dbCursorViewOnly);
	if ( count == 0 )
		return count;
	 m_bHasMoreRecord = true;
	m_pRecord = m_Cursor.get();
	return count;
}

bool avatar::insertMDB()
{
	if ( m_Record.ID == 0 )
		m_Record.ID=generateKey();
	dbCursor<_t_avatar> slcursor;
	dbQuery query;
	query,"ID=",m_Record.ID;
	if ( slcursor.select(query,dbCursorViewOnly) > 0 )
		ThrowException<CDaoException>("insert error[avatar primary key(%d) already exists]",m_Record.ID);
	::insert(m_Record);
	return true;
}

bool avatar::insertMDB(const avatar_record& record)
{
	m_Record.ID = record.ID;
	if ( m_Record.ID == 0 )
		m_Record.ID=generateKey();
	m_Record.AccountID = record.AccountID;
	m_Record.PlayerID = record.PlayerID;
	m_Record.PlayerLevel = record.PlayerLevel;
	m_Record.CreateTime = record.CreateTime/1000;
	m_Record.LastLogoutTime = record.LastLogoutTime/1000;
	m_Record.MaxVigor = record.MaxVigor;
	m_Record.CurVigor = record.CurVigor;
	m_Record.Money = record.Money;
	m_Record.Experience = record.Experience;
	m_Record.PlayerJob = record.PlayerJob;
	m_Record.CurChapter = record.CurChapter;
	m_Record.PartnerExp = record.PartnerExp;
	m_Record.NormalRecruitTime = record.NormalRecruitTime/1000;
	m_Record.NormalBoxOpenTime = record.NormalBoxOpenTime/1000;
	m_Record.LastAddVigorTime = record.LastAddVigorTime/1000;
	m_Record.SmeltExp = record.SmeltExp;
	m_Record.ServedPartnerId = record.ServedPartnerId;
	dbCursor<_t_avatar> slcursor;
	dbQuery query;
	query,"ID=",m_Record.ID;
	if ( slcursor.select(query,dbCursorViewOnly) > 0 )
		ThrowException<CDaoException>("insert error[avatar primary key(%d) already exists]",m_Record.ID);
	::insert(m_Record);
	return true;
}

bool avatar::insertDB(CConnection& Conn)
{
	CStatementPtr stmt = Conn.prepareStatement("insert into avatar values(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)");
	stmt->setUInt64(1,m_Record.ID);
	stmt->setUInt64(2,m_Record.AccountID);
	stmt->setUInt64(3,m_Record.PlayerID);
	stmt->setUInt(4,m_Record.PlayerLevel);
	stmt->setTimestamp(5,m_Record.CreateTime*1000);
	stmt->setTimestamp(6,m_Record.LastLogoutTime*1000);
	stmt->setUInt(7,m_Record.MaxVigor);
	stmt->setUInt(8,m_Record.CurVigor);
	stmt->setUInt(9,m_Record.Money);
	stmt->setUInt(10,m_Record.Experience);
	stmt->setUInt(11,m_Record.PlayerJob);
	stmt->setUInt(12,m_Record.CurChapter);
	stmt->setUInt(13,m_Record.PartnerExp);
	stmt->setTimestamp(14,m_Record.NormalRecruitTime*1000);
	stmt->setTimestamp(15,m_Record.NormalBoxOpenTime*1000);
	stmt->setTimestamp(16,m_Record.LastAddVigorTime*1000);
	stmt->setUInt(17,m_Record.SmeltExp);
	stmt->setUInt(18,m_Record.ServedPartnerId);
	bool retval = stmt->executeUpdate() > 0;
	m_Record.ID=stmt->getInsertID(NULL);
	return retval;
}

string avatar::generateInsertSQL()
{
	char tmpsql[1025];
	string sql = "insert into avatar values (";
	snprintf(tmpsql,sizeof(tmpsql),"%d",m_Record.ID);
	sql+=tmpsql;
	sql += ",";
	snprintf(tmpsql,sizeof(tmpsql),"%d",m_Record.AccountID);
	sql+=tmpsql;
	sql += ",";
	snprintf(tmpsql,sizeof(tmpsql),"%d",m_Record.PlayerID);
	sql+=tmpsql;
	sql += ",";
	snprintf(tmpsql,sizeof(tmpsql),"%d",m_Record.PlayerLevel);
	sql+=tmpsql;
	sql += ",";
	snprintf(tmpsql,sizeof(tmpsql),"`%s`",CDateTime(m_Record.CreateTime*1000).asString("YYYY-MM-DD hh:nn:ss").c_str());
	sql+=tmpsql;
	sql += ",";
	snprintf(tmpsql,sizeof(tmpsql),"`%s`",CDateTime(m_Record.LastLogoutTime*1000).asString("YYYY-MM-DD hh:nn:ss").c_str());
	sql+=tmpsql;
	sql += ",";
	snprintf(tmpsql,sizeof(tmpsql),"%d",m_Record.MaxVigor);
	sql+=tmpsql;
	sql += ",";
	snprintf(tmpsql,sizeof(tmpsql),"%d",m_Record.CurVigor);
	sql+=tmpsql;
	sql += ",";
	snprintf(tmpsql,sizeof(tmpsql),"%d",m_Record.Money);
	sql+=tmpsql;
	sql += ",";
	snprintf(tmpsql,sizeof(tmpsql),"%d",m_Record.Experience);
	sql+=tmpsql;
	sql += ",";
	snprintf(tmpsql,sizeof(tmpsql),"%d",m_Record.PlayerJob);
	sql+=tmpsql;
	sql += ",";
	snprintf(tmpsql,sizeof(tmpsql),"%d",m_Record.CurChapter);
	sql+=tmpsql;
	sql += ",";
	snprintf(tmpsql,sizeof(tmpsql),"%d",m_Record.PartnerExp);
	sql+=tmpsql;
	sql += ",";
	snprintf(tmpsql,sizeof(tmpsql),"`%s`",CDateTime(m_Record.NormalRecruitTime*1000).asString("YYYY-MM-DD hh:nn:ss").c_str());
	sql+=tmpsql;
	sql += ",";
	snprintf(tmpsql,sizeof(tmpsql),"`%s`",CDateTime(m_Record.NormalBoxOpenTime*1000).asString("YYYY-MM-DD hh:nn:ss").c_str());
	sql+=tmpsql;
	sql += ",";
	snprintf(tmpsql,sizeof(tmpsql),"`%s`",CDateTime(m_Record.LastAddVigorTime*1000).asString("YYYY-MM-DD hh:nn:ss").c_str());
	sql+=tmpsql;
	sql += ",";
	snprintf(tmpsql,sizeof(tmpsql),"%d",m_Record.SmeltExp);
	sql+=tmpsql;
	sql += ",";
	snprintf(tmpsql,sizeof(tmpsql),"%d",m_Record.ServedPartnerId);
	sql+=tmpsql;
	sql+=");";
	return sql;
}

bool avatar::updateMDB()
{
	dbCursor<_t_avatar> upcursor;
	dbQuery query;
	query,"ID=",m_Record.ID;
	if ( upcursor.select(query,dbCursorForUpdate) <= 0 )
		ThrowException<CDaoException>("update error not such key record");
	upcursor->AccountID=m_Record.AccountID;
	upcursor->PlayerID=m_Record.PlayerID;
	upcursor->PlayerLevel=m_Record.PlayerLevel;
	upcursor->CreateTime=m_Record.CreateTime;
	upcursor->LastLogoutTime=m_Record.LastLogoutTime;
	upcursor->MaxVigor=m_Record.MaxVigor;
	upcursor->CurVigor=m_Record.CurVigor;
	upcursor->Money=m_Record.Money;
	upcursor->Experience=m_Record.Experience;
	upcursor->PlayerJob=m_Record.PlayerJob;
	upcursor->CurChapter=m_Record.CurChapter;
	upcursor->PartnerExp=m_Record.PartnerExp;
	upcursor->NormalRecruitTime=m_Record.NormalRecruitTime;
	upcursor->NormalBoxOpenTime=m_Record.NormalBoxOpenTime;
	upcursor->LastAddVigorTime=m_Record.LastAddVigorTime;
	upcursor->SmeltExp=m_Record.SmeltExp;
	upcursor->ServedPartnerId=m_Record.ServedPartnerId;
	upcursor.update();
	return true;
}

bool avatar::updateDB(CConnection& Conn)
{
	CStatementPtr stmt = Conn.prepareStatement("update avatar set AccountID=?,PlayerID=?,PlayerLevel=?,CreateTime=?,LastLogoutTime=?,MaxVigor=?,CurVigor=?,Money=?,Experience=?,PlayerJob=?,CurChapter=?,PartnerExp=?,NormalRecruitTime=?,NormalBoxOpenTime=?,LastAddVigorTime=?,SmeltExp=?,ServedPartnerId=? where ID=?");
	stmt->setUInt64(1,m_Record.AccountID);
	stmt->setUInt64(2,m_Record.PlayerID);
	stmt->setUInt(3,m_Record.PlayerLevel);
	stmt->setTimestamp(4,m_Record.CreateTime*1000);
	stmt->setTimestamp(5,m_Record.LastLogoutTime*1000);
	stmt->setUInt(6,m_Record.MaxVigor);
	stmt->setUInt(7,m_Record.CurVigor);
	stmt->setUInt(8,m_Record.Money);
	stmt->setUInt(9,m_Record.Experience);
	stmt->setUInt(10,m_Record.PlayerJob);
	stmt->setUInt(11,m_Record.CurChapter);
	stmt->setUInt(12,m_Record.PartnerExp);
	stmt->setTimestamp(13,m_Record.NormalRecruitTime*1000);
	stmt->setTimestamp(14,m_Record.NormalBoxOpenTime*1000);
	stmt->setTimestamp(15,m_Record.LastAddVigorTime*1000);
	stmt->setUInt(16,m_Record.SmeltExp);
	stmt->setUInt(17,m_Record.ServedPartnerId);
	stmt->setUInt64(18,m_Record.ID);
	return stmt->executeUpdate() > 0;
}

string avatar::generateUpdateSQL()
{
	char tmpsql[1025];
	string sql = "update avatar set ";
	snprintf(tmpsql,sizeof(tmpsql),"AccountID=%d",m_Record.AccountID);
	sql+=tmpsql;
	sql += ",";
	snprintf(tmpsql,sizeof(tmpsql),"PlayerID=%d",m_Record.PlayerID);
	sql+=tmpsql;
	sql += ",";
	snprintf(tmpsql,sizeof(tmpsql),"PlayerLevel=%d",m_Record.PlayerLevel);
	sql+=tmpsql;
	sql += ",";
	snprintf(tmpsql,sizeof(tmpsql),"CreateTime=`%s`",CDateTime(m_Record.CreateTime*1000).asString("YYYY-MM-DD hh:nn:ss").c_str());
	sql+=tmpsql;
	sql += ",";
	snprintf(tmpsql,sizeof(tmpsql),"LastLogoutTime=`%s`",CDateTime(m_Record.LastLogoutTime*1000).asString("YYYY-MM-DD hh:nn:ss").c_str());
	sql+=tmpsql;
	sql += ",";
	snprintf(tmpsql,sizeof(tmpsql),"MaxVigor=%d",m_Record.MaxVigor);
	sql+=tmpsql;
	sql += ",";
	snprintf(tmpsql,sizeof(tmpsql),"CurVigor=%d",m_Record.CurVigor);
	sql+=tmpsql;
	sql += ",";
	snprintf(tmpsql,sizeof(tmpsql),"Money=%d",m_Record.Money);
	sql+=tmpsql;
	sql += ",";
	snprintf(tmpsql,sizeof(tmpsql),"Experience=%d",m_Record.Experience);
	sql+=tmpsql;
	sql += ",";
	snprintf(tmpsql,sizeof(tmpsql),"PlayerJob=%d",m_Record.PlayerJob);
	sql+=tmpsql;
	sql += ",";
	snprintf(tmpsql,sizeof(tmpsql),"CurChapter=%d",m_Record.CurChapter);
	sql+=tmpsql;
	sql += ",";
	snprintf(tmpsql,sizeof(tmpsql),"PartnerExp=%d",m_Record.PartnerExp);
	sql+=tmpsql;
	sql += ",";
	snprintf(tmpsql,sizeof(tmpsql),"NormalRecruitTime=`%s`",CDateTime(m_Record.NormalRecruitTime*1000).asString("YYYY-MM-DD hh:nn:ss").c_str());
	sql+=tmpsql;
	sql += ",";
	snprintf(tmpsql,sizeof(tmpsql),"NormalBoxOpenTime=`%s`",CDateTime(m_Record.NormalBoxOpenTime*1000).asString("YYYY-MM-DD hh:nn:ss").c_str());
	sql+=tmpsql;
	sql += ",";
	snprintf(tmpsql,sizeof(tmpsql),"LastAddVigorTime=`%s`",CDateTime(m_Record.LastAddVigorTime*1000).asString("YYYY-MM-DD hh:nn:ss").c_str());
	sql+=tmpsql;
	sql += ",";
	snprintf(tmpsql,sizeof(tmpsql),"SmeltExp=%d",m_Record.SmeltExp);
	sql+=tmpsql;
	sql += ",";
	snprintf(tmpsql,sizeof(tmpsql),"ServedPartnerId=%d",m_Record.ServedPartnerId);
	sql+=tmpsql;
	sql += "where ";
	snprintf(tmpsql,sizeof(tmpsql),"ID=%d",m_Record.ID);
	sql+=tmpsql;
	sql+=";";
	return sql;
}

bool avatar::removeMDB()
{
	dbCursor<_t_avatar> upcursor;
	dbQuery query;
	query,"ID=",m_Record.ID;
	if ( upcursor.select(query,dbCursorForUpdate) <= 0 )
		ThrowException<CDaoException>("update error not such key record");
	upcursor->AccountID=m_Record.AccountID;
	upcursor->PlayerID=m_Record.PlayerID;
	upcursor->PlayerLevel=m_Record.PlayerLevel;
			upcursor->MaxVigor=m_Record.MaxVigor;
	upcursor->CurVigor=m_Record.CurVigor;
	upcursor->Money=m_Record.Money;
	upcursor->Experience=m_Record.Experience;
	upcursor->PlayerJob=m_Record.PlayerJob;
	upcursor->CurChapter=m_Record.CurChapter;
	upcursor->PartnerExp=m_Record.PartnerExp;
				upcursor->SmeltExp=m_Record.SmeltExp;
	upcursor->ServedPartnerId=m_Record.ServedPartnerId;
	upcursor.remove();
	return true;
}

bool avatar::removeRecord(CConnection& Conn)
{
	CStatementPtr stmt = Conn.prepareStatement("delete from avatar  where ID=?");
	stmt->setUInt64(1,m_Record.ID);
	return stmt->executeUpdate() > 0;
}

string avatar::generateRemoveSQL()
{
	char tmpsql[1025];
	string sql = "delete from avatar ";
	sql += " where ";
	snprintf(tmpsql,sizeof(tmpsql),"ID=%d",m_Record.ID);
	sql+=tmpsql;
	sql+=";";
	return sql;
}

bool avatar::removeDB(CConnection& Conn)
{
	return removeRecord(Conn);
}

bool avatar::removeAll()
{
	avatar dao;
	dbCursor<_t_avatar> upcursor(dbCursorForUpdate);
	upcursor.removeAll();
	return true;
}

void avatar::loadDB(CConnection& Conn)
{
	avatar dao;
	CStatementPtr stmt = Conn.prepareStatement("select * from avatar");
	CResultSetPtr resultset = stmt->executeQuery();
	uint64 recordcount = 0;
	while ( resultset->next() )
	{
		recordcount++;
		dao.readDB(*resultset.get());
		dao.insert();
	}
	log_info("tablename[%s] recordcount[" I64_FMT "]", "avatar", recordcount);
	dao.reset();
	dao.setID(0);
	dao.insertDB(Conn);
	g_Key = dao.getID() - 1;
	dao.removeRecord(Conn);
	log_info("%s id[" I64_FMT "]", "avatar", g_Key);
}

uint64 avatar::getKey()
{
	return g_Key;
}

REGISTER(_t_battlechecklog);
uint64 battlechecklog::g_Key = 0;
uint64 battlechecklog::generateKey()
{
	return ++g_Key;
}

 battlechecklog::battlechecklog()
{
	m_TableName = m_Name="battlechecklog";
	reset();
	m_bHasMoreRecord = false;
	m_pRecord=NULL;
}

void battlechecklog::reset()
{
	memset(&m_Record,0,sizeof(m_Record));
	m_Record.Create_time = CDateTime().getTotalMills()/1000;
	m_pRecord=NULL;
}

CDBDaoImpl* battlechecklog::clone()
{
	return new battlechecklog(*this);
}

 battlechecklog::battlechecklog(const battlechecklog& Other)
{
	m_Record = Other.m_Record;
	m_TableName = Other.m_TableName;
	m_Name=Other.m_Name;
}

bool battlechecklog::check(const CMysqlDaoImpl* pOther) const
{
	const battlechecklog* p = static_cast<const battlechecklog*>(pOther);
	return m_Record.id<p->m_Record.id;
}

bool battlechecklog::readMDB()
{
	return true;
}

void battlechecklog::readDB(CResultSet& ResultSet)
{
	reset();
	if ( ! ResultSet.isNull(1) ) m_Record.id = ResultSet.getInt64(1);
	if ( ! ResultSet.isNull(2) ) m_Record.BattlemapId = ResultSet.getInt(2);
	if ( ! ResultSet.isNull(3) ) m_Record.PlayerId = ResultSet.getInt64(3);
	if ( ! ResultSet.isNull(4) ) m_Record.CheckResult = ResultSet.getInt(4);
	if ( ! ResultSet.isNull(5) ) m_Record.Create_time = ResultSet.getTimestamp(5).getTotalMills()/1000;
	return ;
}

bool battlechecklog::nextMDB()
{
	if ( ! m_bHasMoreRecord )
		return false;
	memcpy(&m_Record,m_pRecord,sizeof(m_Record));
	m_pRecord = m_Cursor.next();	m_bHasMoreRecord = ( NULL != m_pRecord );
	return true;
}

int battlechecklog::select(dbQuery& Query)
{
	m_bHasMoreRecord = false;
	int count = m_Cursor.select(Query,dbCursorViewOnly);
	if ( count == 0 )
		return count;
	 m_bHasMoreRecord = true;
	m_pRecord = m_Cursor.get();
	return count;
}

bool battlechecklog::insertMDB()
{
	if ( m_Record.id == 0 )
		m_Record.id=generateKey();
	dbCursor<_t_battlechecklog> slcursor;
	dbQuery query;
	query,"id=",m_Record.id;
	if ( slcursor.select(query,dbCursorViewOnly) > 0 )
		ThrowException<CDaoException>("insert error[battlechecklog primary key(%d) already exists]",m_Record.id);
	::insert(m_Record);
	return true;
}

bool battlechecklog::insertMDB(const battlechecklog_record& record)
{
	m_Record.id = record.id;
	if ( m_Record.id == 0 )
		m_Record.id=generateKey();
	m_Record.BattlemapId = record.BattlemapId;
	m_Record.PlayerId = record.PlayerId;
	m_Record.CheckResult = record.CheckResult;
	m_Record.Create_time = record.Create_time/1000;
	dbCursor<_t_battlechecklog> slcursor;
	dbQuery query;
	query,"id=",m_Record.id;
	if ( slcursor.select(query,dbCursorViewOnly) > 0 )
		ThrowException<CDaoException>("insert error[battlechecklog primary key(%d) already exists]",m_Record.id);
	::insert(m_Record);
	return true;
}

bool battlechecklog::insertDB(CConnection& Conn)
{
	CStatementPtr stmt = Conn.prepareStatement("insert into battlechecklog values(?,?,?,?,?)");
	stmt->setUInt64(1,m_Record.id);
	stmt->setInt(2,m_Record.BattlemapId);
	stmt->setInt64(3,m_Record.PlayerId);
	stmt->setInt(4,m_Record.CheckResult);
	stmt->setTimestamp(5,m_Record.Create_time*1000);
	bool retval = stmt->executeUpdate() > 0;
	m_Record.id=stmt->getInsertID(NULL);
	return retval;
}

string battlechecklog::generateInsertSQL()
{
	char tmpsql[1025];
	string sql = "insert into battlechecklog values (";
	snprintf(tmpsql,sizeof(tmpsql),"%d",m_Record.id);
	sql+=tmpsql;
	sql += ",";
	snprintf(tmpsql,sizeof(tmpsql),"%d",m_Record.BattlemapId);
	sql+=tmpsql;
	sql += ",";
	snprintf(tmpsql,sizeof(tmpsql),"%d",m_Record.PlayerId);
	sql+=tmpsql;
	sql += ",";
	snprintf(tmpsql,sizeof(tmpsql),"%d",m_Record.CheckResult);
	sql+=tmpsql;
	sql += ",";
	snprintf(tmpsql,sizeof(tmpsql),"`%s`",CDateTime(m_Record.Create_time*1000).asString("YYYY-MM-DD hh:nn:ss").c_str());
	sql+=tmpsql;
	sql+=");";
	return sql;
}

bool battlechecklog::updateMDB()
{
	dbCursor<_t_battlechecklog> upcursor;
	dbQuery query;
	query,"id=",m_Record.id;
	if ( upcursor.select(query,dbCursorForUpdate) <= 0 )
		ThrowException<CDaoException>("update error not such key record");
	upcursor->BattlemapId=m_Record.BattlemapId;
	upcursor->PlayerId=m_Record.PlayerId;
	upcursor->CheckResult=m_Record.CheckResult;
	upcursor->Create_time=m_Record.Create_time;
	upcursor.update();
	return true;
}

bool battlechecklog::updateDB(CConnection& Conn)
{
	CStatementPtr stmt = Conn.prepareStatement("update battlechecklog set BattlemapId=?,PlayerId=?,CheckResult=?,Create_time=? where id=?");
	stmt->setInt(1,m_Record.BattlemapId);
	stmt->setInt64(2,m_Record.PlayerId);
	stmt->setInt(3,m_Record.CheckResult);
	stmt->setTimestamp(4,m_Record.Create_time*1000);
	stmt->setUInt64(5,m_Record.id);
	return stmt->executeUpdate() > 0;
}

string battlechecklog::generateUpdateSQL()
{
	char tmpsql[1025];
	string sql = "update battlechecklog set ";
	snprintf(tmpsql,sizeof(tmpsql),"BattlemapId=%d",m_Record.BattlemapId);
	sql+=tmpsql;
	sql += ",";
	snprintf(tmpsql,sizeof(tmpsql),"PlayerId=%d",m_Record.PlayerId);
	sql+=tmpsql;
	sql += ",";
	snprintf(tmpsql,sizeof(tmpsql),"CheckResult=%d",m_Record.CheckResult);
	sql+=tmpsql;
	sql += ",";
	snprintf(tmpsql,sizeof(tmpsql),"Create_time=`%s`",CDateTime(m_Record.Create_time*1000).asString("YYYY-MM-DD hh:nn:ss").c_str());
	sql+=tmpsql;
	sql += "where ";
	snprintf(tmpsql,sizeof(tmpsql),"id=%d",m_Record.id);
	sql+=tmpsql;
	sql+=";";
	return sql;
}

bool battlechecklog::removeMDB()
{
	dbCursor<_t_battlechecklog> upcursor;
	dbQuery query;
	query,"id=",m_Record.id;
	if ( upcursor.select(query,dbCursorForUpdate) <= 0 )
		ThrowException<CDaoException>("update error not such key record");
	upcursor->BattlemapId=m_Record.BattlemapId;
	upcursor->PlayerId=m_Record.PlayerId;
	upcursor->CheckResult=m_Record.CheckResult;
		upcursor.remove();
	return true;
}

bool battlechecklog::removeRecord(CConnection& Conn)
{
	CStatementPtr stmt = Conn.prepareStatement("delete from battlechecklog  where id=?");
	stmt->setUInt64(1,m_Record.id);
	return stmt->executeUpdate() > 0;
}

string battlechecklog::generateRemoveSQL()
{
	char tmpsql[1025];
	string sql = "delete from battlechecklog ";
	sql += " where ";
	snprintf(tmpsql,sizeof(tmpsql),"id=%d",m_Record.id);
	sql+=tmpsql;
	sql+=";";
	return sql;
}

bool battlechecklog::removeDB(CConnection& Conn)
{
	return removeRecord(Conn);
}

bool battlechecklog::removeAll()
{
	battlechecklog dao;
	dbCursor<_t_battlechecklog> upcursor(dbCursorForUpdate);
	upcursor.removeAll();
	return true;
}

void battlechecklog::loadDB(CConnection& Conn)
{
	battlechecklog dao;
	CStatementPtr stmt = Conn.prepareStatement("select * from battlechecklog");
	CResultSetPtr resultset = stmt->executeQuery();
	uint64 recordcount = 0;
	while ( resultset->next() )
	{
		recordcount++;
		dao.readDB(*resultset.get());
		dao.insert();
	}
	log_info("tablename[%s] recordcount[" I64_FMT "]", "battlechecklog", recordcount);
	dao.reset();
	dao.setid(0);
	dao.insertDB(Conn);
	g_Key = dao.getid() - 1;
	dao.removeRecord(Conn);
	log_info("%s id[" I64_FMT "]", "battlechecklog", g_Key);
}

uint64 battlechecklog::getKey()
{
	return g_Key;
}

REGISTER(_t_battleinfo);
uint64 battleinfo::g_Key = 0;
uint64 battleinfo::generateKey()
{
	return ++g_Key;
}

 battleinfo::battleinfo()
{
	m_TableName = m_Name="battleinfo";
	reset();
	m_bHasMoreRecord = false;
	m_pRecord=NULL;
}

void battleinfo::reset()
{
	memset(&m_Record,0,sizeof(m_Record));
	m_Record.last_modify_date = CDateTime().getTotalMills()/1000;
	m_Record.create_date = CDateTime().getTotalMills()/1000;
	m_pRecord=NULL;
}

CDBDaoImpl* battleinfo::clone()
{
	return new battleinfo(*this);
}

 battleinfo::battleinfo(const battleinfo& Other)
{
	m_Record = Other.m_Record;
	m_TableName = Other.m_TableName;
	m_Name=Other.m_Name;
}

bool battleinfo::check(const CMysqlDaoImpl* pOther) const
{
	const battleinfo* p = static_cast<const battleinfo*>(pOther);
	return m_Record.id<p->m_Record.id;
}

bool battleinfo::readMDB()
{
	return true;
}

void battleinfo::readDB(CResultSet& ResultSet)
{
	reset();
	if ( ! ResultSet.isNull(1) ) m_Record.id = ResultSet.getInt64(1);
	if ( ! ResultSet.isNull(2) ) m_Record.playerid = ResultSet.getInt64(2);
	if ( ! ResultSet.isNull(3) ) m_Record.battlemapid = ResultSet.getInt(3);
	if ( ! ResultSet.isNull(4) ) m_Record.star = ResultSet.getInt(4);
	if ( ! ResultSet.isNull(5) ) m_Record.valid = ResultSet.getInt(5);
	if ( ! ResultSet.isNull(6) ) m_Record.last_modify_date = ResultSet.getTimestamp(6).getTotalMills()/1000;
	if ( ! ResultSet.isNull(7) ) m_Record.create_date = ResultSet.getTimestamp(7).getTotalMills()/1000;
	return ;
}

bool battleinfo::nextMDB()
{
	if ( ! m_bHasMoreRecord )
		return false;
	memcpy(&m_Record,m_pRecord,sizeof(m_Record));
	m_pRecord = m_Cursor.next();	m_bHasMoreRecord = ( NULL != m_pRecord );
	return true;
}

int battleinfo::select(dbQuery& Query)
{
	m_bHasMoreRecord = false;
	int count = m_Cursor.select(Query,dbCursorViewOnly);
	if ( count == 0 )
		return count;
	 m_bHasMoreRecord = true;
	m_pRecord = m_Cursor.get();
	return count;
}

bool battleinfo::insertMDB()
{
	if ( m_Record.id == 0 )
		m_Record.id=generateKey();
	dbCursor<_t_battleinfo> slcursor;
	dbQuery query;
	query,"id=",m_Record.id;
	if ( slcursor.select(query,dbCursorViewOnly) > 0 )
		ThrowException<CDaoException>("insert error[battleinfo primary key(%d) already exists]",m_Record.id);
	::insert(m_Record);
	return true;
}

bool battleinfo::insertMDB(const battleinfo_record& record)
{
	m_Record.id = record.id;
	if ( m_Record.id == 0 )
		m_Record.id=generateKey();
	m_Record.playerid = record.playerid;
	m_Record.battlemapid = record.battlemapid;
	m_Record.star = record.star;
	m_Record.valid = record.valid;
	m_Record.last_modify_date = record.last_modify_date/1000;
	m_Record.create_date = record.create_date/1000;
	dbCursor<_t_battleinfo> slcursor;
	dbQuery query;
	query,"id=",m_Record.id;
	if ( slcursor.select(query,dbCursorViewOnly) > 0 )
		ThrowException<CDaoException>("insert error[battleinfo primary key(%d) already exists]",m_Record.id);
	::insert(m_Record);
	return true;
}

bool battleinfo::insertDB(CConnection& Conn)
{
	CStatementPtr stmt = Conn.prepareStatement("insert into battleinfo values(?,?,?,?,?,?,?)");
	stmt->setInt64(1,m_Record.id);
	stmt->setUInt64(2,m_Record.playerid);
	stmt->setInt(3,m_Record.battlemapid);
	stmt->setInt(4,m_Record.star);
	stmt->setUInt(5,m_Record.valid);
	stmt->setTimestamp(6,m_Record.last_modify_date*1000);
	stmt->setTimestamp(7,m_Record.create_date*1000);
	bool retval = stmt->executeUpdate() > 0;
	m_Record.id=stmt->getInsertID(NULL);
	return retval;
}

string battleinfo::generateInsertSQL()
{
	char tmpsql[1025];
	string sql = "insert into battleinfo values (";
	snprintf(tmpsql,sizeof(tmpsql),"%d",m_Record.id);
	sql+=tmpsql;
	sql += ",";
	snprintf(tmpsql,sizeof(tmpsql),"%d",m_Record.playerid);
	sql+=tmpsql;
	sql += ",";
	snprintf(tmpsql,sizeof(tmpsql),"%d",m_Record.battlemapid);
	sql+=tmpsql;
	sql += ",";
	snprintf(tmpsql,sizeof(tmpsql),"%d",m_Record.star);
	sql+=tmpsql;
	sql += ",";
	snprintf(tmpsql,sizeof(tmpsql),"%d",m_Record.valid);
	sql+=tmpsql;
	sql += ",";
	snprintf(tmpsql,sizeof(tmpsql),"`%s`",CDateTime(m_Record.last_modify_date*1000).asString("YYYY-MM-DD hh:nn:ss").c_str());
	sql+=tmpsql;
	sql += ",";
	snprintf(tmpsql,sizeof(tmpsql),"`%s`",CDateTime(m_Record.create_date*1000).asString("YYYY-MM-DD hh:nn:ss").c_str());
	sql+=tmpsql;
	sql+=");";
	return sql;
}

bool battleinfo::updateMDB()
{
	dbCursor<_t_battleinfo> upcursor;
	dbQuery query;
	query,"id=",m_Record.id;
	if ( upcursor.select(query,dbCursorForUpdate) <= 0 )
		ThrowException<CDaoException>("update error not such key record");
	upcursor->playerid=m_Record.playerid;
	upcursor->battlemapid=m_Record.battlemapid;
	upcursor->star=m_Record.star;
	upcursor->valid=m_Record.valid;
	upcursor->last_modify_date=m_Record.last_modify_date;
	upcursor->create_date=m_Record.create_date;
	upcursor.update();
	return true;
}

bool battleinfo::updateDB(CConnection& Conn)
{
	CStatementPtr stmt = Conn.prepareStatement("update battleinfo set playerid=?,battlemapid=?,star=?,valid=?,last_modify_date=?,create_date=? where id=?");
	stmt->setUInt64(1,m_Record.playerid);
	stmt->setInt(2,m_Record.battlemapid);
	stmt->setInt(3,m_Record.star);
	stmt->setUInt(4,m_Record.valid);
	stmt->setTimestamp(5,m_Record.last_modify_date*1000);
	stmt->setTimestamp(6,m_Record.create_date*1000);
	stmt->setInt64(7,m_Record.id);
	return stmt->executeUpdate() > 0;
}

string battleinfo::generateUpdateSQL()
{
	char tmpsql[1025];
	string sql = "update battleinfo set ";
	snprintf(tmpsql,sizeof(tmpsql),"playerid=%d",m_Record.playerid);
	sql+=tmpsql;
	sql += ",";
	snprintf(tmpsql,sizeof(tmpsql),"battlemapid=%d",m_Record.battlemapid);
	sql+=tmpsql;
	sql += ",";
	snprintf(tmpsql,sizeof(tmpsql),"star=%d",m_Record.star);
	sql+=tmpsql;
	sql += ",";
	snprintf(tmpsql,sizeof(tmpsql),"valid=%d",m_Record.valid);
	sql+=tmpsql;
	sql += ",";
	snprintf(tmpsql,sizeof(tmpsql),"last_modify_date=`%s`",CDateTime(m_Record.last_modify_date*1000).asString("YYYY-MM-DD hh:nn:ss").c_str());
	sql+=tmpsql;
	sql += ",";
	snprintf(tmpsql,sizeof(tmpsql),"create_date=`%s`",CDateTime(m_Record.create_date*1000).asString("YYYY-MM-DD hh:nn:ss").c_str());
	sql+=tmpsql;
	sql += "where ";
	snprintf(tmpsql,sizeof(tmpsql),"id=%d",m_Record.id);
	sql+=tmpsql;
	sql+=";";
	return sql;
}

bool battleinfo::removeMDB()
{
	dbCursor<_t_battleinfo> upcursor;
	dbQuery query;
	query,"id=",m_Record.id;
	if ( upcursor.select(query,dbCursorForUpdate) <= 0 )
		ThrowException<CDaoException>("update error not such key record");
	upcursor->playerid=m_Record.playerid;
	upcursor->battlemapid=m_Record.battlemapid;
	upcursor->star=m_Record.star;
	upcursor->valid=m_Record.valid;
			upcursor.remove();
	return true;
}

bool battleinfo::removeRecord(CConnection& Conn)
{
	CStatementPtr stmt = Conn.prepareStatement("delete from battleinfo  where id=?");
	stmt->setInt64(1,m_Record.id);
	return stmt->executeUpdate() > 0;
}

string battleinfo::generateRemoveSQL()
{
	char tmpsql[1025];
	string sql = "delete from battleinfo ";
	sql += " where ";
	snprintf(tmpsql,sizeof(tmpsql),"id=%d",m_Record.id);
	sql+=tmpsql;
	sql+=";";
	return sql;
}

bool battleinfo::removeDB(CConnection& Conn)
{
	return removeRecord(Conn);
}

bool battleinfo::removeAll()
{
	battleinfo dao;
	dbCursor<_t_battleinfo> upcursor(dbCursorForUpdate);
	upcursor.removeAll();
	return true;
}

void battleinfo::loadDB(CConnection& Conn)
{
	battleinfo dao;
	CStatementPtr stmt = Conn.prepareStatement("select * from battleinfo");
	CResultSetPtr resultset = stmt->executeQuery();
	uint64 recordcount = 0;
	while ( resultset->next() )
	{
		recordcount++;
		dao.readDB(*resultset.get());
		dao.insert();
	}
	log_info("tablename[%s] recordcount[" I64_FMT "]", "battleinfo", recordcount);
	dao.reset();
	dao.setid(0);
	dao.insertDB(Conn);
	g_Key = dao.getid() - 1;
	dao.removeRecord(Conn);
	log_info("%s id[" I64_FMT "]", "battleinfo", g_Key);
}

uint64 battleinfo::getKey()
{
	return g_Key;
}

REGISTER(_t_friend);
uint64 friend::g_Key = 0;
uint64 friend::generateKey()
{
	return ++g_Key;
}

 friend::friend()
{
	m_TableName = m_Name="friend";
	reset();
	m_bHasMoreRecord = false;
	m_pRecord=NULL;
}

void friend::reset()
{
	memset(&m_Record,0,sizeof(m_Record));
	m_Record.LastGiveVigorTime = CDateTime().getTotalMills()/1000;
	m_Record.LastAssistTime = CDateTime().getTotalMills()/1000;
	m_pRecord=NULL;
}

CDBDaoImpl* friend::clone()
{
	return new friend(*this);
}

 friend::friend(const friend& Other)
{
	m_Record = Other.m_Record;
	m_TableName = Other.m_TableName;
	m_Name=Other.m_Name;
}

bool friend::check(const CMysqlDaoImpl* pOther) const
{
	const friend* p = static_cast<const friend*>(pOther);
	return m_Record.ID<p->m_Record.ID;
}

bool friend::readMDB()
{
	return true;
}

void friend::readDB(CResultSet& ResultSet)
{
	reset();
	if ( ! ResultSet.isNull(1) ) m_Record.ID = ResultSet.getInt64(1);
	if ( ! ResultSet.isNull(2) ) m_Record.AccountID = ResultSet.getInt64(2);
	if ( ! ResultSet.isNull(3) ) m_Record.FriendAccountID = ResultSet.getInt64(3);
	if ( ! ResultSet.isNull(4) ) m_Record.LastGiveVigorTime = ResultSet.getTimestamp(4).getTotalMills()/1000;
	if ( ! ResultSet.isNull(5) ) m_Record.LastAssistTime = ResultSet.getTimestamp(5).getTotalMills()/1000;
	return ;
}

bool friend::nextMDB()
{
	if ( ! m_bHasMoreRecord )
		return false;
	memcpy(&m_Record,m_pRecord,sizeof(m_Record));
	m_pRecord = m_Cursor.next();	m_bHasMoreRecord = ( NULL != m_pRecord );
	return true;
}

int friend::select(dbQuery& Query)
{
	m_bHasMoreRecord = false;
	int count = m_Cursor.select(Query,dbCursorViewOnly);
	if ( count == 0 )
		return count;
	 m_bHasMoreRecord = true;
	m_pRecord = m_Cursor.get();
	return count;
}

bool friend::insertMDB()
{
	if ( m_Record.ID == 0 )
		m_Record.ID=generateKey();
	dbCursor<_t_friend> slcursor;
	dbQuery query;
	query,"ID=",m_Record.ID;
	if ( slcursor.select(query,dbCursorViewOnly) > 0 )
		ThrowException<CDaoException>("insert error[friend primary key(%d) already exists]",m_Record.ID);
	::insert(m_Record);
	return true;
}

bool friend::insertMDB(const friend_record& record)
{
	m_Record.ID = record.ID;
	if ( m_Record.ID == 0 )
		m_Record.ID=generateKey();
	m_Record.AccountID = record.AccountID;
	m_Record.FriendAccountID = record.FriendAccountID;
	m_Record.LastGiveVigorTime = record.LastGiveVigorTime/1000;
	m_Record.LastAssistTime = record.LastAssistTime/1000;
	dbCursor<_t_friend> slcursor;
	dbQuery query;
	query,"ID=",m_Record.ID;
	if ( slcursor.select(query,dbCursorViewOnly) > 0 )
		ThrowException<CDaoException>("insert error[friend primary key(%d) already exists]",m_Record.ID);
	::insert(m_Record);
	return true;
}

bool friend::insertDB(CConnection& Conn)
{
	CStatementPtr stmt = Conn.prepareStatement("insert into friend values(?,?,?,?,?)");
	stmt->setUInt64(1,m_Record.ID);
	stmt->setUInt64(2,m_Record.AccountID);
	stmt->setUInt64(3,m_Record.FriendAccountID);
	stmt->setTimestamp(4,m_Record.LastGiveVigorTime*1000);
	stmt->setTimestamp(5,m_Record.LastAssistTime*1000);
	bool retval = stmt->executeUpdate() > 0;
	m_Record.ID=stmt->getInsertID(NULL);
	return retval;
}

string friend::generateInsertSQL()
{
	char tmpsql[1025];
	string sql = "insert into friend values (";
	snprintf(tmpsql,sizeof(tmpsql),"%d",m_Record.ID);
	sql+=tmpsql;
	sql += ",";
	snprintf(tmpsql,sizeof(tmpsql),"%d",m_Record.AccountID);
	sql+=tmpsql;
	sql += ",";
	snprintf(tmpsql,sizeof(tmpsql),"%d",m_Record.FriendAccountID);
	sql+=tmpsql;
	sql += ",";
	snprintf(tmpsql,sizeof(tmpsql),"`%s`",CDateTime(m_Record.LastGiveVigorTime*1000).asString("YYYY-MM-DD hh:nn:ss").c_str());
	sql+=tmpsql;
	sql += ",";
	snprintf(tmpsql,sizeof(tmpsql),"`%s`",CDateTime(m_Record.LastAssistTime*1000).asString("YYYY-MM-DD hh:nn:ss").c_str());
	sql+=tmpsql;
	sql+=");";
	return sql;
}

bool friend::updateMDB()
{
	dbCursor<_t_friend> upcursor;
	dbQuery query;
	query,"ID=",m_Record.ID;
	if ( upcursor.select(query,dbCursorForUpdate) <= 0 )
		ThrowException<CDaoException>("update error not such key record");
	upcursor->AccountID=m_Record.AccountID;
	upcursor->FriendAccountID=m_Record.FriendAccountID;
	upcursor->LastGiveVigorTime=m_Record.LastGiveVigorTime;
	upcursor->LastAssistTime=m_Record.LastAssistTime;
	upcursor.update();
	return true;
}

bool friend::updateDB(CConnection& Conn)
{
	CStatementPtr stmt = Conn.prepareStatement("update friend set AccountID=?,FriendAccountID=?,LastGiveVigorTime=?,LastAssistTime=? where ID=?");
	stmt->setUInt64(1,m_Record.AccountID);
	stmt->setUInt64(2,m_Record.FriendAccountID);
	stmt->setTimestamp(3,m_Record.LastGiveVigorTime*1000);
	stmt->setTimestamp(4,m_Record.LastAssistTime*1000);
	stmt->setUInt64(5,m_Record.ID);
	return stmt->executeUpdate() > 0;
}

string friend::generateUpdateSQL()
{
	char tmpsql[1025];
	string sql = "update friend set ";
	snprintf(tmpsql,sizeof(tmpsql),"AccountID=%d",m_Record.AccountID);
	sql+=tmpsql;
	sql += ",";
	snprintf(tmpsql,sizeof(tmpsql),"FriendAccountID=%d",m_Record.FriendAccountID);
	sql+=tmpsql;
	sql += ",";
	snprintf(tmpsql,sizeof(tmpsql),"LastGiveVigorTime=`%s`",CDateTime(m_Record.LastGiveVigorTime*1000).asString("YYYY-MM-DD hh:nn:ss").c_str());
	sql+=tmpsql;
	sql += ",";
	snprintf(tmpsql,sizeof(tmpsql),"LastAssistTime=`%s`",CDateTime(m_Record.LastAssistTime*1000).asString("YYYY-MM-DD hh:nn:ss").c_str());
	sql+=tmpsql;
	sql += "where ";
	snprintf(tmpsql,sizeof(tmpsql),"ID=%d",m_Record.ID);
	sql+=tmpsql;
	sql+=";";
	return sql;
}

bool friend::removeMDB()
{
	dbCursor<_t_friend> upcursor;
	dbQuery query;
	query,"ID=",m_Record.ID;
	if ( upcursor.select(query,dbCursorForUpdate) <= 0 )
		ThrowException<CDaoException>("update error not such key record");
	upcursor->AccountID=m_Record.AccountID;
	upcursor->FriendAccountID=m_Record.FriendAccountID;
			upcursor.remove();
	return true;
}

bool friend::removeRecord(CConnection& Conn)
{
	CStatementPtr stmt = Conn.prepareStatement("delete from friend  where ID=?");
	stmt->setUInt64(1,m_Record.ID);
	return stmt->executeUpdate() > 0;
}

string friend::generateRemoveSQL()
{
	char tmpsql[1025];
	string sql = "delete from friend ";
	sql += " where ";
	snprintf(tmpsql,sizeof(tmpsql),"ID=%d",m_Record.ID);
	sql+=tmpsql;
	sql+=";";
	return sql;
}

bool friend::removeDB(CConnection& Conn)
{
	return removeRecord(Conn);
}

bool friend::removeAll()
{
	friend dao;
	dbCursor<_t_friend> upcursor(dbCursorForUpdate);
	upcursor.removeAll();
	return true;
}

void friend::loadDB(CConnection& Conn)
{
	friend dao;
	CStatementPtr stmt = Conn.prepareStatement("select * from friend");
	CResultSetPtr resultset = stmt->executeQuery();
	uint64 recordcount = 0;
	while ( resultset->next() )
	{
		recordcount++;
		dao.readDB(*resultset.get());
		dao.insert();
	}
	log_info("tablename[%s] recordcount[" I64_FMT "]", "friend", recordcount);
	dao.reset();
	dao.setID(0);
	dao.insertDB(Conn);
	g_Key = dao.getID() - 1;
	dao.removeRecord(Conn);
	log_info("%s id[" I64_FMT "]", "friend", g_Key);
}

uint64 friend::getKey()
{
	return g_Key;
}

REGISTER(_t_friend_battle_award);
uint64 friend_battle_award::g_Key = 0;
uint64 friend_battle_award::generateKey()
{
	return ++g_Key;
}

 friend_battle_award::friend_battle_award()
{
	m_TableName = m_Name="friend_battle_award";
	reset();
	m_bHasMoreRecord = false;
	m_pRecord=NULL;
}

void friend_battle_award::reset()
{
	memset(&m_Record,0,sizeof(m_Record));
	m_Record.award_exp = 0;
	m_Record.award_silver = 0;
	m_Record.create_time = CDateTime().getTotalMills()/1000;
	m_pRecord=NULL;
}

CDBDaoImpl* friend_battle_award::clone()
{
	return new friend_battle_award(*this);
}

 friend_battle_award::friend_battle_award(const friend_battle_award& Other)
{
	m_Record = Other.m_Record;
	m_TableName = Other.m_TableName;
	m_Name=Other.m_Name;
}

bool friend_battle_award::check(const CMysqlDaoImpl* pOther) const
{
	const friend_battle_award* p = static_cast<const friend_battle_award*>(pOther);
	return m_Record.id<p->m_Record.id;
}

bool friend_battle_award::readMDB()
{
	return true;
}

void friend_battle_award::readDB(CResultSet& ResultSet)
{
	reset();
	if ( ! ResultSet.isNull(1) ) m_Record.id = ResultSet.getInt64(1);
	if ( ! ResultSet.isNull(2) ) m_Record.player_id = ResultSet.getInt64(2);
	if ( ! ResultSet.isNull(3) ) m_Record.award_exp = ResultSet.getInt(3);
	if ( ! ResultSet.isNull(4) ) m_Record.award_silver = ResultSet.getInt(4);
	if ( ! ResultSet.isNull(5) ) m_Record.award_vigor = ResultSet.getInt(5);
	if ( ! ResultSet.isNull(6) ) m_Record.create_time = ResultSet.getTimestamp(6).getTotalMills()/1000;
	return ;
}

bool friend_battle_award::nextMDB()
{
	if ( ! m_bHasMoreRecord )
		return false;
	memcpy(&m_Record,m_pRecord,sizeof(m_Record));
	m_pRecord = m_Cursor.next();	m_bHasMoreRecord = ( NULL != m_pRecord );
	return true;
}

int friend_battle_award::select(dbQuery& Query)
{
	m_bHasMoreRecord = false;
	int count = m_Cursor.select(Query,dbCursorViewOnly);
	if ( count == 0 )
		return count;
	 m_bHasMoreRecord = true;
	m_pRecord = m_Cursor.get();
	return count;
}

bool friend_battle_award::insertMDB()
{
	if ( m_Record.id == 0 )
		m_Record.id=generateKey();
	dbCursor<_t_friend_battle_award> slcursor;
	dbQuery query;
	query,"id=",m_Record.id;
	if ( slcursor.select(query,dbCursorViewOnly) > 0 )
		ThrowException<CDaoException>("insert error[friend_battle_award primary key(%d) already exists]",m_Record.id);
	::insert(m_Record);
	return true;
}

bool friend_battle_award::insertMDB(const friend_battle_award_record& record)
{
	m_Record.id = record.id;
	if ( m_Record.id == 0 )
		m_Record.id=generateKey();
	m_Record.player_id = record.player_id;
	m_Record.award_exp = record.award_exp;
	m_Record.award_silver = record.award_silver;
	m_Record.award_vigor = record.award_vigor;
	m_Record.create_time = record.create_time/1000;
	dbCursor<_t_friend_battle_award> slcursor;
	dbQuery query;
	query,"id=",m_Record.id;
	if ( slcursor.select(query,dbCursorViewOnly) > 0 )
		ThrowException<CDaoException>("insert error[friend_battle_award primary key(%d) already exists]",m_Record.id);
	::insert(m_Record);
	return true;
}

bool friend_battle_award::insertDB(CConnection& Conn)
{
	CStatementPtr stmt = Conn.prepareStatement("insert into friend_battle_award values(?,?,?,?,?,?)");
	stmt->setInt64(1,m_Record.id);
	stmt->setUInt64(2,m_Record.player_id);
	stmt->setUInt(3,m_Record.award_exp);
	stmt->setInt(4,m_Record.award_silver);
	stmt->setInt(5,m_Record.award_vigor);
	stmt->setTimestamp(6,m_Record.create_time*1000);
	bool retval = stmt->executeUpdate() > 0;
	m_Record.id=stmt->getInsertID(NULL);
	return retval;
}

string friend_battle_award::generateInsertSQL()
{
	char tmpsql[1025];
	string sql = "insert into friend_battle_award values (";
	snprintf(tmpsql,sizeof(tmpsql),"%d",m_Record.id);
	sql+=tmpsql;
	sql += ",";
	snprintf(tmpsql,sizeof(tmpsql),"%d",m_Record.player_id);
	sql+=tmpsql;
	sql += ",";
	snprintf(tmpsql,sizeof(tmpsql),"%d",m_Record.award_exp);
	sql+=tmpsql;
	sql += ",";
	snprintf(tmpsql,sizeof(tmpsql),"%d",m_Record.award_silver);
	sql+=tmpsql;
	sql += ",";
	snprintf(tmpsql,sizeof(tmpsql),"%d",m_Record.award_vigor);
	sql+=tmpsql;
	sql += ",";
	snprintf(tmpsql,sizeof(tmpsql),"`%s`",CDateTime(m_Record.create_time*1000).asString("YYYY-MM-DD hh:nn:ss").c_str());
	sql+=tmpsql;
	sql+=");";
	return sql;
}

bool friend_battle_award::updateMDB()
{
	dbCursor<_t_friend_battle_award> upcursor;
	dbQuery query;
	query,"id=",m_Record.id;
	if ( upcursor.select(query,dbCursorForUpdate) <= 0 )
		ThrowException<CDaoException>("update error not such key record");
	upcursor->player_id=m_Record.player_id;
	upcursor->award_exp=m_Record.award_exp;
	upcursor->award_silver=m_Record.award_silver;
	upcursor->award_vigor=m_Record.award_vigor;
	upcursor->create_time=m_Record.create_time;
	upcursor.update();
	return true;
}

bool friend_battle_award::updateDB(CConnection& Conn)
{
	CStatementPtr stmt = Conn.prepareStatement("update friend_battle_award set player_id=?,award_exp=?,award_silver=?,award_vigor=?,create_time=? where id=?");
	stmt->setUInt64(1,m_Record.player_id);
	stmt->setUInt(2,m_Record.award_exp);
	stmt->setInt(3,m_Record.award_silver);
	stmt->setInt(4,m_Record.award_vigor);
	stmt->setTimestamp(5,m_Record.create_time*1000);
	stmt->setInt64(6,m_Record.id);
	return stmt->executeUpdate() > 0;
}

string friend_battle_award::generateUpdateSQL()
{
	char tmpsql[1025];
	string sql = "update friend_battle_award set ";
	snprintf(tmpsql,sizeof(tmpsql),"player_id=%d",m_Record.player_id);
	sql+=tmpsql;
	sql += ",";
	snprintf(tmpsql,sizeof(tmpsql),"award_exp=%d",m_Record.award_exp);
	sql+=tmpsql;
	sql += ",";
	snprintf(tmpsql,sizeof(tmpsql),"award_silver=%d",m_Record.award_silver);
	sql+=tmpsql;
	sql += ",";
	snprintf(tmpsql,sizeof(tmpsql),"award_vigor=%d",m_Record.award_vigor);
	sql+=tmpsql;
	sql += ",";
	snprintf(tmpsql,sizeof(tmpsql),"create_time=`%s`",CDateTime(m_Record.create_time*1000).asString("YYYY-MM-DD hh:nn:ss").c_str());
	sql+=tmpsql;
	sql += "where ";
	snprintf(tmpsql,sizeof(tmpsql),"id=%d",m_Record.id);
	sql+=tmpsql;
	sql+=";";
	return sql;
}

bool friend_battle_award::removeMDB()
{
	dbCursor<_t_friend_battle_award> upcursor;
	dbQuery query;
	query,"id=",m_Record.id;
	if ( upcursor.select(query,dbCursorForUpdate) <= 0 )
		ThrowException<CDaoException>("update error not such key record");
	upcursor->player_id=m_Record.player_id;
	upcursor->award_exp=m_Record.award_exp;
	upcursor->award_silver=m_Record.award_silver;
	upcursor->award_vigor=m_Record.award_vigor;
		upcursor.remove();
	return true;
}

bool friend_battle_award::removeRecord(CConnection& Conn)
{
	CStatementPtr stmt = Conn.prepareStatement("delete from friend_battle_award  where id=?");
	stmt->setInt64(1,m_Record.id);
	return stmt->executeUpdate() > 0;
}

string friend_battle_award::generateRemoveSQL()
{
	char tmpsql[1025];
	string sql = "delete from friend_battle_award ";
	sql += " where ";
	snprintf(tmpsql,sizeof(tmpsql),"id=%d",m_Record.id);
	sql+=tmpsql;
	sql+=";";
	return sql;
}

bool friend_battle_award::removeDB(CConnection& Conn)
{
	return removeRecord(Conn);
}

bool friend_battle_award::removeAll()
{
	friend_battle_award dao;
	dbCursor<_t_friend_battle_award> upcursor(dbCursorForUpdate);
	upcursor.removeAll();
	return true;
}

void friend_battle_award::loadDB(CConnection& Conn)
{
	friend_battle_award dao;
	CStatementPtr stmt = Conn.prepareStatement("select * from friend_battle_award");
	CResultSetPtr resultset = stmt->executeQuery();
	uint64 recordcount = 0;
	while ( resultset->next() )
	{
		recordcount++;
		dao.readDB(*resultset.get());
		dao.insert();
	}
	log_info("tablename[%s] recordcount[" I64_FMT "]", "friend_battle_award", recordcount);
	dao.reset();
	dao.setid(0);
	dao.insertDB(Conn);
	g_Key = dao.getid() - 1;
	dao.removeRecord(Conn);
	log_info("%s id[" I64_FMT "]", "friend_battle_award", g_Key);
}

uint64 friend_battle_award::getKey()
{
	return g_Key;
}

REGISTER(_t_item);
uint64 item::g_Key = 0;
uint64 item::generateKey()
{
	return ++g_Key;
}

 item::item()
{
	m_TableName = m_Name="item";
	reset();
	m_bHasMoreRecord = false;
	m_pRecord=NULL;
}

void item::reset()
{
	memset(&m_Record,0,sizeof(m_Record));
	m_pRecord=NULL;
}

CDBDaoImpl* item::clone()
{
	return new item(*this);
}

 item::item(const item& Other)
{
	m_Record = Other.m_Record;
	m_TableName = Other.m_TableName;
	m_Name=Other.m_Name;
}

bool item::check(const CMysqlDaoImpl* pOther) const
{
	const item* p = static_cast<const item*>(pOther);
	return m_Record.ID<p->m_Record.ID;
}

bool item::readMDB()
{
	return true;
}

void item::readDB(CResultSet& ResultSet)
{
	reset();
	if ( ! ResultSet.isNull(1) ) m_Record.ID = ResultSet.getInt64(1);
	if ( ! ResultSet.isNull(2) ) m_Record.PlayerID = ResultSet.getInt64(2);
	if ( ! ResultSet.isNull(3) ) m_Record.ItemID = ResultSet.getInt(3);
	if ( ! ResultSet.isNull(4) ) m_Record.ItemPos = ResultSet.getInt(4);
	if ( ! ResultSet.isNull(5) ) m_Record.ItemNum = ResultSet.getInt(5);
	return ;
}

bool item::nextMDB()
{
	if ( ! m_bHasMoreRecord )
		return false;
	memcpy(&m_Record,m_pRecord,sizeof(m_Record));
	m_pRecord = m_Cursor.next();	m_bHasMoreRecord = ( NULL != m_pRecord );
	return true;
}

int item::select(dbQuery& Query)
{
	m_bHasMoreRecord = false;
	int count = m_Cursor.select(Query,dbCursorViewOnly);
	if ( count == 0 )
		return count;
	 m_bHasMoreRecord = true;
	m_pRecord = m_Cursor.get();
	return count;
}

bool item::insertMDB()
{
	if ( m_Record.ID == 0 )
		m_Record.ID=generateKey();
	dbCursor<_t_item> slcursor;
	dbQuery query;
	query,"ID=",m_Record.ID;
	if ( slcursor.select(query,dbCursorViewOnly) > 0 )
		ThrowException<CDaoException>("insert error[item primary key(%d) already exists]",m_Record.ID);
	::insert(m_Record);
	return true;
}

bool item::insertMDB(const item_record& record)
{
	m_Record.ID = record.ID;
	if ( m_Record.ID == 0 )
		m_Record.ID=generateKey();
	m_Record.PlayerID = record.PlayerID;
	m_Record.ItemID = record.ItemID;
	m_Record.ItemPos = record.ItemPos;
	m_Record.ItemNum = record.ItemNum;
	dbCursor<_t_item> slcursor;
	dbQuery query;
	query,"ID=",m_Record.ID;
	if ( slcursor.select(query,dbCursorViewOnly) > 0 )
		ThrowException<CDaoException>("insert error[item primary key(%d) already exists]",m_Record.ID);
	::insert(m_Record);
	return true;
}

bool item::insertDB(CConnection& Conn)
{
	CStatementPtr stmt = Conn.prepareStatement("insert into item values(?,?,?,?,?)");
	stmt->setUInt64(1,m_Record.ID);
	stmt->setUInt64(2,m_Record.PlayerID);
	stmt->setUInt(3,m_Record.ItemID);
	stmt->setUInt(4,m_Record.ItemPos);
	stmt->setUInt(5,m_Record.ItemNum);
	bool retval = stmt->executeUpdate() > 0;
	m_Record.ID=stmt->getInsertID(NULL);
	return retval;
}

string item::generateInsertSQL()
{
	char tmpsql[1025];
	string sql = "insert into item values (";
	snprintf(tmpsql,sizeof(tmpsql),"%d",m_Record.ID);
	sql+=tmpsql;
	sql += ",";
	snprintf(tmpsql,sizeof(tmpsql),"%d",m_Record.PlayerID);
	sql+=tmpsql;
	sql += ",";
	snprintf(tmpsql,sizeof(tmpsql),"%d",m_Record.ItemID);
	sql+=tmpsql;
	sql += ",";
	snprintf(tmpsql,sizeof(tmpsql),"%d",m_Record.ItemPos);
	sql+=tmpsql;
	sql += ",";
	snprintf(tmpsql,sizeof(tmpsql),"%d",m_Record.ItemNum);
	sql+=tmpsql;
	sql+=");";
	return sql;
}

bool item::updateMDB()
{
	dbCursor<_t_item> upcursor;
	dbQuery query;
	query,"ID=",m_Record.ID;
	if ( upcursor.select(query,dbCursorForUpdate) <= 0 )
		ThrowException<CDaoException>("update error not such key record");
	upcursor->PlayerID=m_Record.PlayerID;
	upcursor->ItemID=m_Record.ItemID;
	upcursor->ItemPos=m_Record.ItemPos;
	upcursor->ItemNum=m_Record.ItemNum;
	upcursor.update();
	return true;
}

bool item::updateDB(CConnection& Conn)
{
	CStatementPtr stmt = Conn.prepareStatement("update item set PlayerID=?,ItemID=?,ItemPos=?,ItemNum=? where ID=?");
	stmt->setUInt64(1,m_Record.PlayerID);
	stmt->setUInt(2,m_Record.ItemID);
	stmt->setUInt(3,m_Record.ItemPos);
	stmt->setUInt(4,m_Record.ItemNum);
	stmt->setUInt64(5,m_Record.ID);
	return stmt->executeUpdate() > 0;
}

string item::generateUpdateSQL()
{
	char tmpsql[1025];
	string sql = "update item set ";
	snprintf(tmpsql,sizeof(tmpsql),"PlayerID=%d",m_Record.PlayerID);
	sql+=tmpsql;
	sql += ",";
	snprintf(tmpsql,sizeof(tmpsql),"ItemID=%d",m_Record.ItemID);
	sql+=tmpsql;
	sql += ",";
	snprintf(tmpsql,sizeof(tmpsql),"ItemPos=%d",m_Record.ItemPos);
	sql+=tmpsql;
	sql += ",";
	snprintf(tmpsql,sizeof(tmpsql),"ItemNum=%d",m_Record.ItemNum);
	sql+=tmpsql;
	sql += "where ";
	snprintf(tmpsql,sizeof(tmpsql),"ID=%d",m_Record.ID);
	sql+=tmpsql;
	sql+=";";
	return sql;
}

bool item::removeMDB()
{
	dbCursor<_t_item> upcursor;
	dbQuery query;
	query,"ID=",m_Record.ID;
	if ( upcursor.select(query,dbCursorForUpdate) <= 0 )
		ThrowException<CDaoException>("update error not such key record");
	upcursor->PlayerID=m_Record.PlayerID;
	upcursor->ItemID=m_Record.ItemID;
	upcursor->ItemPos=m_Record.ItemPos;
	upcursor->ItemNum=m_Record.ItemNum;
	upcursor.remove();
	return true;
}

bool item::removeRecord(CConnection& Conn)
{
	CStatementPtr stmt = Conn.prepareStatement("delete from item  where ID=?");
	stmt->setUInt64(1,m_Record.ID);
	return stmt->executeUpdate() > 0;
}

string item::generateRemoveSQL()
{
	char tmpsql[1025];
	string sql = "delete from item ";
	sql += " where ";
	snprintf(tmpsql,sizeof(tmpsql),"ID=%d",m_Record.ID);
	sql+=tmpsql;
	sql+=";";
	return sql;
}

bool item::removeDB(CConnection& Conn)
{
	return removeRecord(Conn);
}

bool item::removeAll()
{
	item dao;
	dbCursor<_t_item> upcursor(dbCursorForUpdate);
	upcursor.removeAll();
	return true;
}

void item::loadDB(CConnection& Conn)
{
	item dao;
	CStatementPtr stmt = Conn.prepareStatement("select * from item");
	CResultSetPtr resultset = stmt->executeQuery();
	uint64 recordcount = 0;
	while ( resultset->next() )
	{
		recordcount++;
		dao.readDB(*resultset.get());
		dao.insert();
	}
	log_info("tablename[%s] recordcount[" I64_FMT "]", "item", recordcount);
	dao.reset();
	dao.setID(0);
	dao.insertDB(Conn);
	g_Key = dao.getID() - 1;
	dao.removeRecord(Conn);
	log_info("%s id[" I64_FMT "]", "item", g_Key);
}

uint64 item::getKey()
{
	return g_Key;
}

REGISTER(_t_mounts);
uint64 mounts::g_Key = 0;
uint64 mounts::generateKey()
{
	return ++g_Key;
}

 mounts::mounts()
{
	m_TableName = m_Name="mounts";
	reset();
	m_bHasMoreRecord = false;
	m_pRecord=NULL;
}

void mounts::reset()
{
	memset(&m_Record,0,sizeof(m_Record));
	m_pRecord=NULL;
}

CDBDaoImpl* mounts::clone()
{
	return new mounts(*this);
}

 mounts::mounts(const mounts& Other)
{
	m_Record = Other.m_Record;
	m_TableName = Other.m_TableName;
	m_Name=Other.m_Name;
}

bool mounts::check(const CMysqlDaoImpl* pOther) const
{
	const mounts* p = static_cast<const mounts*>(pOther);
	return m_Record.ID<p->m_Record.ID;
}

bool mounts::readMDB()
{
	return true;
}

void mounts::readDB(CResultSet& ResultSet)
{
	reset();
	if ( ! ResultSet.isNull(1) ) m_Record.ID = ResultSet.getInt64(1);
	if ( ! ResultSet.isNull(2) ) m_Record.PlayerID = ResultSet.getInt64(2);
	if ( ! ResultSet.isNull(3) ) m_Record.MountsID = ResultSet.getInt(3);
	if ( ! ResultSet.isNull(4) ) m_Record.MountsExp = ResultSet.getInt(4);
	return ;
}

bool mounts::nextMDB()
{
	if ( ! m_bHasMoreRecord )
		return false;
	memcpy(&m_Record,m_pRecord,sizeof(m_Record));
	m_pRecord = m_Cursor.next();	m_bHasMoreRecord = ( NULL != m_pRecord );
	return true;
}

int mounts::select(dbQuery& Query)
{
	m_bHasMoreRecord = false;
	int count = m_Cursor.select(Query,dbCursorViewOnly);
	if ( count == 0 )
		return count;
	 m_bHasMoreRecord = true;
	m_pRecord = m_Cursor.get();
	return count;
}

bool mounts::insertMDB()
{
	if ( m_Record.ID == 0 )
		m_Record.ID=generateKey();
	dbCursor<_t_mounts> slcursor;
	dbQuery query;
	query,"ID=",m_Record.ID;
	if ( slcursor.select(query,dbCursorViewOnly) > 0 )
		ThrowException<CDaoException>("insert error[mounts primary key(%d) already exists]",m_Record.ID);
	::insert(m_Record);
	return true;
}

bool mounts::insertMDB(const mounts_record& record)
{
	m_Record.ID = record.ID;
	if ( m_Record.ID == 0 )
		m_Record.ID=generateKey();
	m_Record.PlayerID = record.PlayerID;
	m_Record.MountsID = record.MountsID;
	m_Record.MountsExp = record.MountsExp;
	dbCursor<_t_mounts> slcursor;
	dbQuery query;
	query,"ID=",m_Record.ID;
	if ( slcursor.select(query,dbCursorViewOnly) > 0 )
		ThrowException<CDaoException>("insert error[mounts primary key(%d) already exists]",m_Record.ID);
	::insert(m_Record);
	return true;
}

bool mounts::insertDB(CConnection& Conn)
{
	CStatementPtr stmt = Conn.prepareStatement("insert into mounts values(?,?,?,?)");
	stmt->setInt64(1,m_Record.ID);
	stmt->setUInt64(2,m_Record.PlayerID);
	stmt->setUInt(3,m_Record.MountsID);
	stmt->setUInt(4,m_Record.MountsExp);
	bool retval = stmt->executeUpdate() > 0;
	m_Record.ID=stmt->getInsertID(NULL);
	return retval;
}

string mounts::generateInsertSQL()
{
	char tmpsql[1025];
	string sql = "insert into mounts values (";
	snprintf(tmpsql,sizeof(tmpsql),"%d",m_Record.ID);
	sql+=tmpsql;
	sql += ",";
	snprintf(tmpsql,sizeof(tmpsql),"%d",m_Record.PlayerID);
	sql+=tmpsql;
	sql += ",";
	snprintf(tmpsql,sizeof(tmpsql),"%d",m_Record.MountsID);
	sql+=tmpsql;
	sql += ",";
	snprintf(tmpsql,sizeof(tmpsql),"%d",m_Record.MountsExp);
	sql+=tmpsql;
	sql+=");";
	return sql;
}

bool mounts::updateMDB()
{
	dbCursor<_t_mounts> upcursor;
	dbQuery query;
	query,"ID=",m_Record.ID;
	if ( upcursor.select(query,dbCursorForUpdate) <= 0 )
		ThrowException<CDaoException>("update error not such key record");
	upcursor->PlayerID=m_Record.PlayerID;
	upcursor->MountsID=m_Record.MountsID;
	upcursor->MountsExp=m_Record.MountsExp;
	upcursor.update();
	return true;
}

bool mounts::updateDB(CConnection& Conn)
{
	CStatementPtr stmt = Conn.prepareStatement("update mounts set PlayerID=?,MountsID=?,MountsExp=? where ID=?");
	stmt->setUInt64(1,m_Record.PlayerID);
	stmt->setUInt(2,m_Record.MountsID);
	stmt->setUInt(3,m_Record.MountsExp);
	stmt->setInt64(4,m_Record.ID);
	return stmt->executeUpdate() > 0;
}

string mounts::generateUpdateSQL()
{
	char tmpsql[1025];
	string sql = "update mounts set ";
	snprintf(tmpsql,sizeof(tmpsql),"PlayerID=%d",m_Record.PlayerID);
	sql+=tmpsql;
	sql += ",";
	snprintf(tmpsql,sizeof(tmpsql),"MountsID=%d",m_Record.MountsID);
	sql+=tmpsql;
	sql += ",";
	snprintf(tmpsql,sizeof(tmpsql),"MountsExp=%d",m_Record.MountsExp);
	sql+=tmpsql;
	sql += "where ";
	snprintf(tmpsql,sizeof(tmpsql),"ID=%d",m_Record.ID);
	sql+=tmpsql;
	sql+=";";
	return sql;
}

bool mounts::removeMDB()
{
	dbCursor<_t_mounts> upcursor;
	dbQuery query;
	query,"ID=",m_Record.ID;
	if ( upcursor.select(query,dbCursorForUpdate) <= 0 )
		ThrowException<CDaoException>("update error not such key record");
	upcursor->PlayerID=m_Record.PlayerID;
	upcursor->MountsID=m_Record.MountsID;
	upcursor->MountsExp=m_Record.MountsExp;
	upcursor.remove();
	return true;
}

bool mounts::removeRecord(CConnection& Conn)
{
	CStatementPtr stmt = Conn.prepareStatement("delete from mounts  where ID=?");
	stmt->setInt64(1,m_Record.ID);
	return stmt->executeUpdate() > 0;
}

string mounts::generateRemoveSQL()
{
	char tmpsql[1025];
	string sql = "delete from mounts ";
	sql += " where ";
	snprintf(tmpsql,sizeof(tmpsql),"ID=%d",m_Record.ID);
	sql+=tmpsql;
	sql+=";";
	return sql;
}

bool mounts::removeDB(CConnection& Conn)
{
	return removeRecord(Conn);
}

bool mounts::removeAll()
{
	mounts dao;
	dbCursor<_t_mounts> upcursor(dbCursorForUpdate);
	upcursor.removeAll();
	return true;
}

void mounts::loadDB(CConnection& Conn)
{
	mounts dao;
	CStatementPtr stmt = Conn.prepareStatement("select * from mounts");
	CResultSetPtr resultset = stmt->executeQuery();
	uint64 recordcount = 0;
	while ( resultset->next() )
	{
		recordcount++;
		dao.readDB(*resultset.get());
		dao.insert();
	}
	log_info("tablename[%s] recordcount[" I64_FMT "]", "mounts", recordcount);
	dao.reset();
	dao.setID(0);
	dao.insertDB(Conn);
	g_Key = dao.getID() - 1;
	dao.removeRecord(Conn);
	log_info("%s id[" I64_FMT "]", "mounts", g_Key);
}

uint64 mounts::getKey()
{
	return g_Key;
}

REGISTER(_t_pet);
uint64 pet::g_Key = 0;
uint64 pet::generateKey()
{
	return ++g_Key;
}

 pet::pet()
{
	m_TableName = m_Name="pet";
	reset();
	m_bHasMoreRecord = false;
	m_pRecord=NULL;
}

void pet::reset()
{
	memset(&m_Record,0,sizeof(m_Record));
	m_pRecord=NULL;
}

CDBDaoImpl* pet::clone()
{
	return new pet(*this);
}

 pet::pet(const pet& Other)
{
	m_Record = Other.m_Record;
	m_TableName = Other.m_TableName;
	m_Name=Other.m_Name;
}

bool pet::check(const CMysqlDaoImpl* pOther) const
{
	const pet* p = static_cast<const pet*>(pOther);
	return m_Record.ID<p->m_Record.ID;
}

bool pet::readMDB()
{
	return true;
}

void pet::readDB(CResultSet& ResultSet)
{
	reset();
	if ( ! ResultSet.isNull(1) ) m_Record.ID = ResultSet.getInt64(1);
	if ( ! ResultSet.isNull(2) ) m_Record.PlayerID = ResultSet.getInt64(2);
	if ( ! ResultSet.isNull(3) ) m_Record.PetID = ResultSet.getInt(3);
	if ( ! ResultSet.isNull(4) ) m_Record.PetExp = ResultSet.getInt(4);
	return ;
}

bool pet::nextMDB()
{
	if ( ! m_bHasMoreRecord )
		return false;
	memcpy(&m_Record,m_pRecord,sizeof(m_Record));
	m_pRecord = m_Cursor.next();	m_bHasMoreRecord = ( NULL != m_pRecord );
	return true;
}

int pet::select(dbQuery& Query)
{
	m_bHasMoreRecord = false;
	int count = m_Cursor.select(Query,dbCursorViewOnly);
	if ( count == 0 )
		return count;
	 m_bHasMoreRecord = true;
	m_pRecord = m_Cursor.get();
	return count;
}

bool pet::insertMDB()
{
	if ( m_Record.ID == 0 )
		m_Record.ID=generateKey();
	dbCursor<_t_pet> slcursor;
	dbQuery query;
	query,"ID=",m_Record.ID;
	if ( slcursor.select(query,dbCursorViewOnly) > 0 )
		ThrowException<CDaoException>("insert error[pet primary key(%d) already exists]",m_Record.ID);
	::insert(m_Record);
	return true;
}

bool pet::insertMDB(const pet_record& record)
{
	m_Record.ID = record.ID;
	if ( m_Record.ID == 0 )
		m_Record.ID=generateKey();
	m_Record.PlayerID = record.PlayerID;
	m_Record.PetID = record.PetID;
	m_Record.PetExp = record.PetExp;
	dbCursor<_t_pet> slcursor;
	dbQuery query;
	query,"ID=",m_Record.ID;
	if ( slcursor.select(query,dbCursorViewOnly) > 0 )
		ThrowException<CDaoException>("insert error[pet primary key(%d) already exists]",m_Record.ID);
	::insert(m_Record);
	return true;
}

bool pet::insertDB(CConnection& Conn)
{
	CStatementPtr stmt = Conn.prepareStatement("insert into pet values(?,?,?,?)");
	stmt->setUInt64(1,m_Record.ID);
	stmt->setUInt64(2,m_Record.PlayerID);
	stmt->setUInt(3,m_Record.PetID);
	stmt->setUInt(4,m_Record.PetExp);
	bool retval = stmt->executeUpdate() > 0;
	m_Record.ID=stmt->getInsertID(NULL);
	return retval;
}

string pet::generateInsertSQL()
{
	char tmpsql[1025];
	string sql = "insert into pet values (";
	snprintf(tmpsql,sizeof(tmpsql),"%d",m_Record.ID);
	sql+=tmpsql;
	sql += ",";
	snprintf(tmpsql,sizeof(tmpsql),"%d",m_Record.PlayerID);
	sql+=tmpsql;
	sql += ",";
	snprintf(tmpsql,sizeof(tmpsql),"%d",m_Record.PetID);
	sql+=tmpsql;
	sql += ",";
	snprintf(tmpsql,sizeof(tmpsql),"%d",m_Record.PetExp);
	sql+=tmpsql;
	sql+=");";
	return sql;
}

bool pet::updateMDB()
{
	dbCursor<_t_pet> upcursor;
	dbQuery query;
	query,"ID=",m_Record.ID;
	if ( upcursor.select(query,dbCursorForUpdate) <= 0 )
		ThrowException<CDaoException>("update error not such key record");
	upcursor->PlayerID=m_Record.PlayerID;
	upcursor->PetID=m_Record.PetID;
	upcursor->PetExp=m_Record.PetExp;
	upcursor.update();
	return true;
}

bool pet::updateDB(CConnection& Conn)
{
	CStatementPtr stmt = Conn.prepareStatement("update pet set PlayerID=?,PetID=?,PetExp=? where ID=?");
	stmt->setUInt64(1,m_Record.PlayerID);
	stmt->setUInt(2,m_Record.PetID);
	stmt->setUInt(3,m_Record.PetExp);
	stmt->setUInt64(4,m_Record.ID);
	return stmt->executeUpdate() > 0;
}

string pet::generateUpdateSQL()
{
	char tmpsql[1025];
	string sql = "update pet set ";
	snprintf(tmpsql,sizeof(tmpsql),"PlayerID=%d",m_Record.PlayerID);
	sql+=tmpsql;
	sql += ",";
	snprintf(tmpsql,sizeof(tmpsql),"PetID=%d",m_Record.PetID);
	sql+=tmpsql;
	sql += ",";
	snprintf(tmpsql,sizeof(tmpsql),"PetExp=%d",m_Record.PetExp);
	sql+=tmpsql;
	sql += "where ";
	snprintf(tmpsql,sizeof(tmpsql),"ID=%d",m_Record.ID);
	sql+=tmpsql;
	sql+=";";
	return sql;
}

bool pet::removeMDB()
{
	dbCursor<_t_pet> upcursor;
	dbQuery query;
	query,"ID=",m_Record.ID;
	if ( upcursor.select(query,dbCursorForUpdate) <= 0 )
		ThrowException<CDaoException>("update error not such key record");
	upcursor->PlayerID=m_Record.PlayerID;
	upcursor->PetID=m_Record.PetID;
	upcursor->PetExp=m_Record.PetExp;
	upcursor.remove();
	return true;
}

bool pet::removeRecord(CConnection& Conn)
{
	CStatementPtr stmt = Conn.prepareStatement("delete from pet  where ID=?");
	stmt->setUInt64(1,m_Record.ID);
	return stmt->executeUpdate() > 0;
}

string pet::generateRemoveSQL()
{
	char tmpsql[1025];
	string sql = "delete from pet ";
	sql += " where ";
	snprintf(tmpsql,sizeof(tmpsql),"ID=%d",m_Record.ID);
	sql+=tmpsql;
	sql+=";";
	return sql;
}

bool pet::removeDB(CConnection& Conn)
{
	return removeRecord(Conn);
}

bool pet::removeAll()
{
	pet dao;
	dbCursor<_t_pet> upcursor(dbCursorForUpdate);
	upcursor.removeAll();
	return true;
}

void pet::loadDB(CConnection& Conn)
{
	pet dao;
	CStatementPtr stmt = Conn.prepareStatement("select * from pet");
	CResultSetPtr resultset = stmt->executeQuery();
	uint64 recordcount = 0;
	while ( resultset->next() )
	{
		recordcount++;
		dao.readDB(*resultset.get());
		dao.insert();
	}
	log_info("tablename[%s] recordcount[" I64_FMT "]", "pet", recordcount);
	dao.reset();
	dao.setID(0);
	dao.insertDB(Conn);
	g_Key = dao.getID() - 1;
	dao.removeRecord(Conn);
	log_info("%s id[" I64_FMT "]", "pet", g_Key);
}

uint64 pet::getKey()
{
	return g_Key;
}

REGISTER(_t_player_arena_info);
uint64 player_arena_info::g_Key = 0;
uint64 player_arena_info::generateKey()
{
	return ++g_Key;
}

 player_arena_info::player_arena_info()
{
	m_TableName = m_Name="player_arena_info";
	reset();
	m_bHasMoreRecord = false;
	m_pRecord=NULL;
}

void player_arena_info::reset()
{
	memset(&m_Record,0,sizeof(m_Record));
	m_Record.date = CDateTime().getTotalMills()/1000;
	m_Record.cd_start = CDateTime().getTotalMills()/1000;
	m_pRecord=NULL;
}

CDBDaoImpl* player_arena_info::clone()
{
	return new player_arena_info(*this);
}

 player_arena_info::player_arena_info(const player_arena_info& Other)
{
	m_Record = Other.m_Record;
	m_TableName = Other.m_TableName;
	m_Name=Other.m_Name;
}

bool player_arena_info::check(const CMysqlDaoImpl* pOther) const
{
	const player_arena_info* p = static_cast<const player_arena_info*>(pOther);
	return m_Record.id<p->m_Record.id;
}

bool player_arena_info::readMDB()
{
	return true;
}

void player_arena_info::readDB(CResultSet& ResultSet)
{
	reset();
	if ( ! ResultSet.isNull(1) ) m_Record.id = ResultSet.getInt64(1);
	if ( ! ResultSet.isNull(2) ) m_Record.player_id = ResultSet.getInt64(2);
	if ( ! ResultSet.isNull(3) ) m_Record.num = ResultSet.getInt(3);
	if ( ! ResultSet.isNull(4) ) m_Record.date = ResultSet.getTimestamp(4).getTotalMills()/1000;
	if ( ! ResultSet.isNull(5) ) m_Record.cd_time = ResultSet.getInt(5);
	if ( ! ResultSet.isNull(6) ) m_Record.cd_start = ResultSet.getTimestamp(6).getTotalMills()/1000;
	return ;
}

bool player_arena_info::nextMDB()
{
	if ( ! m_bHasMoreRecord )
		return false;
	memcpy(&m_Record,m_pRecord,sizeof(m_Record));
	m_pRecord = m_Cursor.next();	m_bHasMoreRecord = ( NULL != m_pRecord );
	return true;
}

int player_arena_info::select(dbQuery& Query)
{
	m_bHasMoreRecord = false;
	int count = m_Cursor.select(Query,dbCursorViewOnly);
	if ( count == 0 )
		return count;
	 m_bHasMoreRecord = true;
	m_pRecord = m_Cursor.get();
	return count;
}

bool player_arena_info::insertMDB()
{
	if ( m_Record.id == 0 )
		m_Record.id=generateKey();
	dbCursor<_t_player_arena_info> slcursor;
	dbQuery query;
	query,"id=",m_Record.id;
	if ( slcursor.select(query,dbCursorViewOnly) > 0 )
		ThrowException<CDaoException>("insert error[player_arena_info primary key(%d) already exists]",m_Record.id);
	::insert(m_Record);
	return true;
}

bool player_arena_info::insertMDB(const player_arena_info_record& record)
{
	m_Record.id = record.id;
	if ( m_Record.id == 0 )
		m_Record.id=generateKey();
	m_Record.player_id = record.player_id;
	m_Record.num = record.num;
	m_Record.date = record.date/1000;
	m_Record.cd_time = record.cd_time;
	m_Record.cd_start = record.cd_start/1000;
	dbCursor<_t_player_arena_info> slcursor;
	dbQuery query;
	query,"id=",m_Record.id;
	if ( slcursor.select(query,dbCursorViewOnly) > 0 )
		ThrowException<CDaoException>("insert error[player_arena_info primary key(%d) already exists]",m_Record.id);
	::insert(m_Record);
	return true;
}

bool player_arena_info::insertDB(CConnection& Conn)
{
	CStatementPtr stmt = Conn.prepareStatement("insert into player_arena_info values(?,?,?,?,?,?)");
	stmt->setUInt64(1,m_Record.id);
	stmt->setUInt64(2,m_Record.player_id);
	stmt->setUInt(3,m_Record.num);
	stmt->setTimestamp(4,m_Record.date*1000);
	stmt->setUInt(5,m_Record.cd_time);
	stmt->setTimestamp(6,m_Record.cd_start*1000);
	bool retval = stmt->executeUpdate() > 0;
	m_Record.id=stmt->getInsertID(NULL);
	return retval;
}

string player_arena_info::generateInsertSQL()
{
	char tmpsql[1025];
	string sql = "insert into player_arena_info values (";
	snprintf(tmpsql,sizeof(tmpsql),"%d",m_Record.id);
	sql+=tmpsql;
	sql += ",";
	snprintf(tmpsql,sizeof(tmpsql),"%d",m_Record.player_id);
	sql+=tmpsql;
	sql += ",";
	snprintf(tmpsql,sizeof(tmpsql),"%d",m_Record.num);
	sql+=tmpsql;
	sql += ",";
	snprintf(tmpsql,sizeof(tmpsql),"`%s`",CDateTime(m_Record.date*1000).asString("YYYY-MM-DD hh:nn:ss").c_str());
	sql+=tmpsql;
	sql += ",";
	snprintf(tmpsql,sizeof(tmpsql),"%d",m_Record.cd_time);
	sql+=tmpsql;
	sql += ",";
	snprintf(tmpsql,sizeof(tmpsql),"`%s`",CDateTime(m_Record.cd_start*1000).asString("YYYY-MM-DD hh:nn:ss").c_str());
	sql+=tmpsql;
	sql+=");";
	return sql;
}

bool player_arena_info::updateMDB()
{
	dbCursor<_t_player_arena_info> upcursor;
	dbQuery query;
	query,"id=",m_Record.id;
	if ( upcursor.select(query,dbCursorForUpdate) <= 0 )
		ThrowException<CDaoException>("update error not such key record");
	upcursor->player_id=m_Record.player_id;
	upcursor->num=m_Record.num;
	upcursor->date=m_Record.date;
	upcursor->cd_time=m_Record.cd_time;
	upcursor->cd_start=m_Record.cd_start;
	upcursor.update();
	return true;
}

bool player_arena_info::updateDB(CConnection& Conn)
{
	CStatementPtr stmt = Conn.prepareStatement("update player_arena_info set player_id=?,num=?,date=?,cd_time=?,cd_start=? where id=?");
	stmt->setUInt64(1,m_Record.player_id);
	stmt->setUInt(2,m_Record.num);
	stmt->setTimestamp(3,m_Record.date*1000);
	stmt->setUInt(4,m_Record.cd_time);
	stmt->setTimestamp(5,m_Record.cd_start*1000);
	stmt->setUInt64(6,m_Record.id);
	return stmt->executeUpdate() > 0;
}

string player_arena_info::generateUpdateSQL()
{
	char tmpsql[1025];
	string sql = "update player_arena_info set ";
	snprintf(tmpsql,sizeof(tmpsql),"player_id=%d",m_Record.player_id);
	sql+=tmpsql;
	sql += ",";
	snprintf(tmpsql,sizeof(tmpsql),"num=%d",m_Record.num);
	sql+=tmpsql;
	sql += ",";
	snprintf(tmpsql,sizeof(tmpsql),"date=`%s`",CDateTime(m_Record.date*1000).asString("YYYY-MM-DD hh:nn:ss").c_str());
	sql+=tmpsql;
	sql += ",";
	snprintf(tmpsql,sizeof(tmpsql),"cd_time=%d",m_Record.cd_time);
	sql+=tmpsql;
	sql += ",";
	snprintf(tmpsql,sizeof(tmpsql),"cd_start=`%s`",CDateTime(m_Record.cd_start*1000).asString("YYYY-MM-DD hh:nn:ss").c_str());
	sql+=tmpsql;
	sql += "where ";
	snprintf(tmpsql,sizeof(tmpsql),"id=%d",m_Record.id);
	sql+=tmpsql;
	sql+=";";
	return sql;
}

bool player_arena_info::removeMDB()
{
	dbCursor<_t_player_arena_info> upcursor;
	dbQuery query;
	query,"id=",m_Record.id;
	if ( upcursor.select(query,dbCursorForUpdate) <= 0 )
		ThrowException<CDaoException>("update error not such key record");
	upcursor->player_id=m_Record.player_id;
	upcursor->num=m_Record.num;
		upcursor->cd_time=m_Record.cd_time;
		upcursor.remove();
	return true;
}

bool player_arena_info::removeRecord(CConnection& Conn)
{
	CStatementPtr stmt = Conn.prepareStatement("delete from player_arena_info  where id=?");
	stmt->setUInt64(1,m_Record.id);
	return stmt->executeUpdate() > 0;
}

string player_arena_info::generateRemoveSQL()
{
	char tmpsql[1025];
	string sql = "delete from player_arena_info ";
	sql += " where ";
	snprintf(tmpsql,sizeof(tmpsql),"id=%d",m_Record.id);
	sql+=tmpsql;
	sql+=";";
	return sql;
}

bool player_arena_info::removeDB(CConnection& Conn)
{
	return removeRecord(Conn);
}

bool player_arena_info::removeAll()
{
	player_arena_info dao;
	dbCursor<_t_player_arena_info> upcursor(dbCursorForUpdate);
	upcursor.removeAll();
	return true;
}

void player_arena_info::loadDB(CConnection& Conn)
{
	player_arena_info dao;
	CStatementPtr stmt = Conn.prepareStatement("select * from player_arena_info");
	CResultSetPtr resultset = stmt->executeQuery();
	uint64 recordcount = 0;
	while ( resultset->next() )
	{
		recordcount++;
		dao.readDB(*resultset.get());
		dao.insert();
	}
	log_info("tablename[%s] recordcount[" I64_FMT "]", "player_arena_info", recordcount);
	dao.reset();
	dao.setid(0);
	dao.insertDB(Conn);
	g_Key = dao.getid() - 1;
	dao.removeRecord(Conn);
	log_info("%s id[" I64_FMT "]", "player_arena_info", g_Key);
}

uint64 player_arena_info::getKey()
{
	return g_Key;
}

REGISTER(_t_relice);
uint64 relice::g_Key = 0;
uint64 relice::generateKey()
{
	return ++g_Key;
}

 relice::relice()
{
	m_TableName = m_Name="relice";
	reset();
	m_bHasMoreRecord = false;
	m_pRecord=NULL;
}

void relice::reset()
{
	memset(&m_Record,0,sizeof(m_Record));
	m_Record.GemHole1Level = 0;
	m_Record.GemHole1CurExp = 0;
	m_Record.GemHole2Level = 0;
	m_Record.GemHole2CurExp = 0;
	m_Record.GemHole3Level = 0;
	m_Record.GemHole3CurExp = 0;
	m_pRecord=NULL;
}

CDBDaoImpl* relice::clone()
{
	return new relice(*this);
}

 relice::relice(const relice& Other)
{
	m_Record = Other.m_Record;
	m_TableName = Other.m_TableName;
	m_Name=Other.m_Name;
}

bool relice::check(const CMysqlDaoImpl* pOther) const
{
	const relice* p = static_cast<const relice*>(pOther);
	return m_Record.ID<p->m_Record.ID;
}

bool relice::readMDB()
{
	return true;
}

void relice::readDB(CResultSet& ResultSet)
{
	reset();
	if ( ! ResultSet.isNull(1) ) m_Record.ID = ResultSet.getInt64(1);
	if ( ! ResultSet.isNull(2) ) m_Record.PlayerID = ResultSet.getInt64(2);
	if ( ! ResultSet.isNull(3) ) m_Record.ReliceID = ResultSet.getInt(3);
	if ( ! ResultSet.isNull(4) ) m_Record.ActiveAttr = ResultSet.getInt(4);
	if ( ! ResultSet.isNull(5) ) m_Record.GemHole1Level = ResultSet.getInt(5);
	if ( ! ResultSet.isNull(6) ) m_Record.GemHole1CurExp = ResultSet.getInt(6);
	if ( ! ResultSet.isNull(7) ) m_Record.GemHole2Level = ResultSet.getInt(7);
	if ( ! ResultSet.isNull(8) ) m_Record.GemHole2CurExp = ResultSet.getInt(8);
	if ( ! ResultSet.isNull(9) ) m_Record.GemHole3Level = ResultSet.getInt(9);
	if ( ! ResultSet.isNull(10) ) m_Record.GemHole3CurExp = ResultSet.getInt(10);
	return ;
}

bool relice::nextMDB()
{
	if ( ! m_bHasMoreRecord )
		return false;
	memcpy(&m_Record,m_pRecord,sizeof(m_Record));
	m_pRecord = m_Cursor.next();	m_bHasMoreRecord = ( NULL != m_pRecord );
	return true;
}

int relice::select(dbQuery& Query)
{
	m_bHasMoreRecord = false;
	int count = m_Cursor.select(Query,dbCursorViewOnly);
	if ( count == 0 )
		return count;
	 m_bHasMoreRecord = true;
	m_pRecord = m_Cursor.get();
	return count;
}

bool relice::insertMDB()
{
	if ( m_Record.ID == 0 )
		m_Record.ID=generateKey();
	dbCursor<_t_relice> slcursor;
	dbQuery query;
	query,"ID=",m_Record.ID;
	if ( slcursor.select(query,dbCursorViewOnly) > 0 )
		ThrowException<CDaoException>("insert error[relice primary key(%d) already exists]",m_Record.ID);
	::insert(m_Record);
	return true;
}

bool relice::insertMDB(const relice_record& record)
{
	m_Record.ID = record.ID;
	if ( m_Record.ID == 0 )
		m_Record.ID=generateKey();
	m_Record.PlayerID = record.PlayerID;
	m_Record.ReliceID = record.ReliceID;
	m_Record.ActiveAttr = record.ActiveAttr;
	m_Record.GemHole1Level = record.GemHole1Level;
	m_Record.GemHole1CurExp = record.GemHole1CurExp;
	m_Record.GemHole2Level = record.GemHole2Level;
	m_Record.GemHole2CurExp = record.GemHole2CurExp;
	m_Record.GemHole3Level = record.GemHole3Level;
	m_Record.GemHole3CurExp = record.GemHole3CurExp;
	dbCursor<_t_relice> slcursor;
	dbQuery query;
	query,"ID=",m_Record.ID;
	if ( slcursor.select(query,dbCursorViewOnly) > 0 )
		ThrowException<CDaoException>("insert error[relice primary key(%d) already exists]",m_Record.ID);
	::insert(m_Record);
	return true;
}

bool relice::insertDB(CConnection& Conn)
{
	CStatementPtr stmt = Conn.prepareStatement("insert into relice values(?,?,?,?,?,?,?,?,?,?)");
	stmt->setUInt64(1,m_Record.ID);
	stmt->setUInt64(2,m_Record.PlayerID);
	stmt->setUInt(3,m_Record.ReliceID);
	stmt->setInt(4,m_Record.ActiveAttr);
	stmt->setUInt(5,m_Record.GemHole1Level);
	stmt->setUInt(6,m_Record.GemHole1CurExp);
	stmt->setUInt(7,m_Record.GemHole2Level);
	stmt->setUInt(8,m_Record.GemHole2CurExp);
	stmt->setUInt(9,m_Record.GemHole3Level);
	stmt->setUInt(10,m_Record.GemHole3CurExp);
	bool retval = stmt->executeUpdate() > 0;
	m_Record.ID=stmt->getInsertID(NULL);
	return retval;
}

string relice::generateInsertSQL()
{
	char tmpsql[1025];
	string sql = "insert into relice values (";
	snprintf(tmpsql,sizeof(tmpsql),"%d",m_Record.ID);
	sql+=tmpsql;
	sql += ",";
	snprintf(tmpsql,sizeof(tmpsql),"%d",m_Record.PlayerID);
	sql+=tmpsql;
	sql += ",";
	snprintf(tmpsql,sizeof(tmpsql),"%d",m_Record.ReliceID);
	sql+=tmpsql;
	sql += ",";
	snprintf(tmpsql,sizeof(tmpsql),"%d",m_Record.ActiveAttr);
	sql+=tmpsql;
	sql += ",";
	snprintf(tmpsql,sizeof(tmpsql),"%d",m_Record.GemHole1Level);
	sql+=tmpsql;
	sql += ",";
	snprintf(tmpsql,sizeof(tmpsql),"%d",m_Record.GemHole1CurExp);
	sql+=tmpsql;
	sql += ",";
	snprintf(tmpsql,sizeof(tmpsql),"%d",m_Record.GemHole2Level);
	sql+=tmpsql;
	sql += ",";
	snprintf(tmpsql,sizeof(tmpsql),"%d",m_Record.GemHole2CurExp);
	sql+=tmpsql;
	sql += ",";
	snprintf(tmpsql,sizeof(tmpsql),"%d",m_Record.GemHole3Level);
	sql+=tmpsql;
	sql += ",";
	snprintf(tmpsql,sizeof(tmpsql),"%d",m_Record.GemHole3CurExp);
	sql+=tmpsql;
	sql+=");";
	return sql;
}

bool relice::updateMDB()
{
	dbCursor<_t_relice> upcursor;
	dbQuery query;
	query,"ID=",m_Record.ID;
	if ( upcursor.select(query,dbCursorForUpdate) <= 0 )
		ThrowException<CDaoException>("update error not such key record");
	upcursor->PlayerID=m_Record.PlayerID;
	upcursor->ReliceID=m_Record.ReliceID;
	upcursor->ActiveAttr=m_Record.ActiveAttr;
	upcursor->GemHole1Level=m_Record.GemHole1Level;
	upcursor->GemHole1CurExp=m_Record.GemHole1CurExp;
	upcursor->GemHole2Level=m_Record.GemHole2Level;
	upcursor->GemHole2CurExp=m_Record.GemHole2CurExp;
	upcursor->GemHole3Level=m_Record.GemHole3Level;
	upcursor->GemHole3CurExp=m_Record.GemHole3CurExp;
	upcursor.update();
	return true;
}

bool relice::updateDB(CConnection& Conn)
{
	CStatementPtr stmt = Conn.prepareStatement("update relice set PlayerID=?,ReliceID=?,ActiveAttr=?,GemHole1Level=?,GemHole1CurExp=?,GemHole2Level=?,GemHole2CurExp=?,GemHole3Level=?,GemHole3CurExp=? where ID=?");
	stmt->setUInt64(1,m_Record.PlayerID);
	stmt->setUInt(2,m_Record.ReliceID);
	stmt->setInt(3,m_Record.ActiveAttr);
	stmt->setUInt(4,m_Record.GemHole1Level);
	stmt->setUInt(5,m_Record.GemHole1CurExp);
	stmt->setUInt(6,m_Record.GemHole2Level);
	stmt->setUInt(7,m_Record.GemHole2CurExp);
	stmt->setUInt(8,m_Record.GemHole3Level);
	stmt->setUInt(9,m_Record.GemHole3CurExp);
	stmt->setUInt64(10,m_Record.ID);
	return stmt->executeUpdate() > 0;
}

string relice::generateUpdateSQL()
{
	char tmpsql[1025];
	string sql = "update relice set ";
	snprintf(tmpsql,sizeof(tmpsql),"PlayerID=%d",m_Record.PlayerID);
	sql+=tmpsql;
	sql += ",";
	snprintf(tmpsql,sizeof(tmpsql),"ReliceID=%d",m_Record.ReliceID);
	sql+=tmpsql;
	sql += ",";
	snprintf(tmpsql,sizeof(tmpsql),"ActiveAttr=%d",m_Record.ActiveAttr);
	sql+=tmpsql;
	sql += ",";
	snprintf(tmpsql,sizeof(tmpsql),"GemHole1Level=%d",m_Record.GemHole1Level);
	sql+=tmpsql;
	sql += ",";
	snprintf(tmpsql,sizeof(tmpsql),"GemHole1CurExp=%d",m_Record.GemHole1CurExp);
	sql+=tmpsql;
	sql += ",";
	snprintf(tmpsql,sizeof(tmpsql),"GemHole2Level=%d",m_Record.GemHole2Level);
	sql+=tmpsql;
	sql += ",";
	snprintf(tmpsql,sizeof(tmpsql),"GemHole2CurExp=%d",m_Record.GemHole2CurExp);
	sql+=tmpsql;
	sql += ",";
	snprintf(tmpsql,sizeof(tmpsql),"GemHole3Level=%d",m_Record.GemHole3Level);
	sql+=tmpsql;
	sql += ",";
	snprintf(tmpsql,sizeof(tmpsql),"GemHole3CurExp=%d",m_Record.GemHole3CurExp);
	sql+=tmpsql;
	sql += "where ";
	snprintf(tmpsql,sizeof(tmpsql),"ID=%d",m_Record.ID);
	sql+=tmpsql;
	sql+=";";
	return sql;
}

bool relice::removeMDB()
{
	dbCursor<_t_relice> upcursor;
	dbQuery query;
	query,"ID=",m_Record.ID;
	if ( upcursor.select(query,dbCursorForUpdate) <= 0 )
		ThrowException<CDaoException>("update error not such key record");
	upcursor->PlayerID=m_Record.PlayerID;
	upcursor->ReliceID=m_Record.ReliceID;
	upcursor->ActiveAttr=m_Record.ActiveAttr;
	upcursor->GemHole1Level=m_Record.GemHole1Level;
	upcursor->GemHole1CurExp=m_Record.GemHole1CurExp;
	upcursor->GemHole2Level=m_Record.GemHole2Level;
	upcursor->GemHole2CurExp=m_Record.GemHole2CurExp;
	upcursor->GemHole3Level=m_Record.GemHole3Level;
	upcursor->GemHole3CurExp=m_Record.GemHole3CurExp;
	upcursor.remove();
	return true;
}

bool relice::removeRecord(CConnection& Conn)
{
	CStatementPtr stmt = Conn.prepareStatement("delete from relice  where ID=?");
	stmt->setUInt64(1,m_Record.ID);
	return stmt->executeUpdate() > 0;
}

string relice::generateRemoveSQL()
{
	char tmpsql[1025];
	string sql = "delete from relice ";
	sql += " where ";
	snprintf(tmpsql,sizeof(tmpsql),"ID=%d",m_Record.ID);
	sql+=tmpsql;
	sql+=";";
	return sql;
}

bool relice::removeDB(CConnection& Conn)
{
	return removeRecord(Conn);
}

bool relice::removeAll()
{
	relice dao;
	dbCursor<_t_relice> upcursor(dbCursorForUpdate);
	upcursor.removeAll();
	return true;
}

void relice::loadDB(CConnection& Conn)
{
	relice dao;
	CStatementPtr stmt = Conn.prepareStatement("select * from relice");
	CResultSetPtr resultset = stmt->executeQuery();
	uint64 recordcount = 0;
	while ( resultset->next() )
	{
		recordcount++;
		dao.readDB(*resultset.get());
		dao.insert();
	}
	log_info("tablename[%s] recordcount[" I64_FMT "]", "relice", recordcount);
	dao.reset();
	dao.setID(0);
	dao.insertDB(Conn);
	g_Key = dao.getID() - 1;
	dao.removeRecord(Conn);
	log_info("%s id[" I64_FMT "]", "relice", g_Key);
}

uint64 relice::getKey()
{
	return g_Key;
}

REGISTER(_t_skill);
uint64 skill::g_Key = 0;
uint64 skill::generateKey()
{
	return ++g_Key;
}

 skill::skill()
{
	m_TableName = m_Name="skill";
	reset();
	m_bHasMoreRecord = false;
	m_pRecord=NULL;
}

void skill::reset()
{
	memset(&m_Record,0,sizeof(m_Record));
	m_Record.create_date = CDateTime().getTotalMills()/1000;
	m_Record.last_modify_date = CDateTime().getTotalMills()/1000;
	m_pRecord=NULL;
}

CDBDaoImpl* skill::clone()
{
	return new skill(*this);
}

 skill::skill(const skill& Other)
{
	m_Record = Other.m_Record;
	m_TableName = Other.m_TableName;
	m_Name=Other.m_Name;
}

bool skill::check(const CMysqlDaoImpl* pOther) const
{
	const skill* p = static_cast<const skill*>(pOther);
	return m_Record.ID<p->m_Record.ID;
}

bool skill::readMDB()
{
	return true;
}

void skill::readDB(CResultSet& ResultSet)
{
	reset();
	if ( ! ResultSet.isNull(1) ) m_Record.ID = ResultSet.getInt64(1);
	if ( ! ResultSet.isNull(2) ) m_Record.PlayerID = ResultSet.getInt64(2);
	if ( ! ResultSet.isNull(3) ) m_Record.SkillID = ResultSet.getInt(3);
	if ( ! ResultSet.isNull(4) ) m_Record.SkillType = ResultSet.getInt(4);
	if ( ! ResultSet.isNull(5) ) m_Record.SkillLevel = ResultSet.getInt(5);
	if ( ! ResultSet.isNull(6) ) m_Record.SkillPos = ResultSet.getInt(6);
	if ( ! ResultSet.isNull(7) ) m_Record.valid = ResultSet.getInt(7);
	if ( ! ResultSet.isNull(8) ) m_Record.create_date = ResultSet.getTimestamp(8).getTotalMills()/1000;
	if ( ! ResultSet.isNull(9) ) m_Record.last_modify_date = ResultSet.getTimestamp(9).getTotalMills()/1000;
	return ;
}

bool skill::nextMDB()
{
	if ( ! m_bHasMoreRecord )
		return false;
	memcpy(&m_Record,m_pRecord,sizeof(m_Record));
	m_pRecord = m_Cursor.next();	m_bHasMoreRecord = ( NULL != m_pRecord );
	return true;
}

int skill::select(dbQuery& Query)
{
	m_bHasMoreRecord = false;
	int count = m_Cursor.select(Query,dbCursorViewOnly);
	if ( count == 0 )
		return count;
	 m_bHasMoreRecord = true;
	m_pRecord = m_Cursor.get();
	return count;
}

bool skill::insertMDB()
{
	if ( m_Record.ID == 0 )
		m_Record.ID=generateKey();
	dbCursor<_t_skill> slcursor;
	dbQuery query;
	query,"ID=",m_Record.ID;
	if ( slcursor.select(query,dbCursorViewOnly) > 0 )
		ThrowException<CDaoException>("insert error[skill primary key(%d) already exists]",m_Record.ID);
	::insert(m_Record);
	return true;
}

bool skill::insertMDB(const skill_record& record)
{
	m_Record.ID = record.ID;
	if ( m_Record.ID == 0 )
		m_Record.ID=generateKey();
	m_Record.PlayerID = record.PlayerID;
	m_Record.SkillID = record.SkillID;
	m_Record.SkillType = record.SkillType;
	m_Record.SkillLevel = record.SkillLevel;
	m_Record.SkillPos = record.SkillPos;
	m_Record.valid = record.valid;
	m_Record.create_date = record.create_date/1000;
	m_Record.last_modify_date = record.last_modify_date/1000;
	dbCursor<_t_skill> slcursor;
	dbQuery query;
	query,"ID=",m_Record.ID;
	if ( slcursor.select(query,dbCursorViewOnly) > 0 )
		ThrowException<CDaoException>("insert error[skill primary key(%d) already exists]",m_Record.ID);
	::insert(m_Record);
	return true;
}

bool skill::insertDB(CConnection& Conn)
{
	CStatementPtr stmt = Conn.prepareStatement("insert into skill values(?,?,?,?,?,?,?,?,?)");
	stmt->setUInt64(1,m_Record.ID);
	stmt->setUInt64(2,m_Record.PlayerID);
	stmt->setUInt(3,m_Record.SkillID);
	stmt->setUInt(4,m_Record.SkillType);
	stmt->setUInt(5,m_Record.SkillLevel);
	stmt->setInt(6,m_Record.SkillPos);
	stmt->setUInt(7,m_Record.valid);
	stmt->setTimestamp(8,m_Record.create_date*1000);
	stmt->setTimestamp(9,m_Record.last_modify_date*1000);
	bool retval = stmt->executeUpdate() > 0;
	m_Record.ID=stmt->getInsertID(NULL);
	return retval;
}

string skill::generateInsertSQL()
{
	char tmpsql[1025];
	string sql = "insert into skill values (";
	snprintf(tmpsql,sizeof(tmpsql),"%d",m_Record.ID);
	sql+=tmpsql;
	sql += ",";
	snprintf(tmpsql,sizeof(tmpsql),"%d",m_Record.PlayerID);
	sql+=tmpsql;
	sql += ",";
	snprintf(tmpsql,sizeof(tmpsql),"%d",m_Record.SkillID);
	sql+=tmpsql;
	sql += ",";
	snprintf(tmpsql,sizeof(tmpsql),"%d",m_Record.SkillType);
	sql+=tmpsql;
	sql += ",";
	snprintf(tmpsql,sizeof(tmpsql),"%d",m_Record.SkillLevel);
	sql+=tmpsql;
	sql += ",";
	snprintf(tmpsql,sizeof(tmpsql),"%d",m_Record.SkillPos);
	sql+=tmpsql;
	sql += ",";
	snprintf(tmpsql,sizeof(tmpsql),"%d",m_Record.valid);
	sql+=tmpsql;
	sql += ",";
	snprintf(tmpsql,sizeof(tmpsql),"`%s`",CDateTime(m_Record.create_date*1000).asString("YYYY-MM-DD hh:nn:ss").c_str());
	sql+=tmpsql;
	sql += ",";
	snprintf(tmpsql,sizeof(tmpsql),"`%s`",CDateTime(m_Record.last_modify_date*1000).asString("YYYY-MM-DD hh:nn:ss").c_str());
	sql+=tmpsql;
	sql+=");";
	return sql;
}

bool skill::updateMDB()
{
	dbCursor<_t_skill> upcursor;
	dbQuery query;
	query,"ID=",m_Record.ID;
	if ( upcursor.select(query,dbCursorForUpdate) <= 0 )
		ThrowException<CDaoException>("update error not such key record");
	upcursor->PlayerID=m_Record.PlayerID;
	upcursor->SkillID=m_Record.SkillID;
	upcursor->SkillType=m_Record.SkillType;
	upcursor->SkillLevel=m_Record.SkillLevel;
	upcursor->SkillPos=m_Record.SkillPos;
	upcursor->valid=m_Record.valid;
	upcursor->create_date=m_Record.create_date;
	upcursor->last_modify_date=m_Record.last_modify_date;
	upcursor.update();
	return true;
}

bool skill::updateDB(CConnection& Conn)
{
	CStatementPtr stmt = Conn.prepareStatement("update skill set PlayerID=?,SkillID=?,SkillType=?,SkillLevel=?,SkillPos=?,valid=?,create_date=?,last_modify_date=? where ID=?");
	stmt->setUInt64(1,m_Record.PlayerID);
	stmt->setUInt(2,m_Record.SkillID);
	stmt->setUInt(3,m_Record.SkillType);
	stmt->setUInt(4,m_Record.SkillLevel);
	stmt->setInt(5,m_Record.SkillPos);
	stmt->setUInt(6,m_Record.valid);
	stmt->setTimestamp(7,m_Record.create_date*1000);
	stmt->setTimestamp(8,m_Record.last_modify_date*1000);
	stmt->setUInt64(9,m_Record.ID);
	return stmt->executeUpdate() > 0;
}

string skill::generateUpdateSQL()
{
	char tmpsql[1025];
	string sql = "update skill set ";
	snprintf(tmpsql,sizeof(tmpsql),"PlayerID=%d",m_Record.PlayerID);
	sql+=tmpsql;
	sql += ",";
	snprintf(tmpsql,sizeof(tmpsql),"SkillID=%d",m_Record.SkillID);
	sql+=tmpsql;
	sql += ",";
	snprintf(tmpsql,sizeof(tmpsql),"SkillType=%d",m_Record.SkillType);
	sql+=tmpsql;
	sql += ",";
	snprintf(tmpsql,sizeof(tmpsql),"SkillLevel=%d",m_Record.SkillLevel);
	sql+=tmpsql;
	sql += ",";
	snprintf(tmpsql,sizeof(tmpsql),"SkillPos=%d",m_Record.SkillPos);
	sql+=tmpsql;
	sql += ",";
	snprintf(tmpsql,sizeof(tmpsql),"valid=%d",m_Record.valid);
	sql+=tmpsql;
	sql += ",";
	snprintf(tmpsql,sizeof(tmpsql),"create_date=`%s`",CDateTime(m_Record.create_date*1000).asString("YYYY-MM-DD hh:nn:ss").c_str());
	sql+=tmpsql;
	sql += ",";
	snprintf(tmpsql,sizeof(tmpsql),"last_modify_date=`%s`",CDateTime(m_Record.last_modify_date*1000).asString("YYYY-MM-DD hh:nn:ss").c_str());
	sql+=tmpsql;
	sql += "where ";
	snprintf(tmpsql,sizeof(tmpsql),"ID=%d",m_Record.ID);
	sql+=tmpsql;
	sql+=";";
	return sql;
}

bool skill::removeMDB()
{
	dbCursor<_t_skill> upcursor;
	dbQuery query;
	query,"ID=",m_Record.ID;
	if ( upcursor.select(query,dbCursorForUpdate) <= 0 )
		ThrowException<CDaoException>("update error not such key record");
	upcursor->PlayerID=m_Record.PlayerID;
	upcursor->SkillID=m_Record.SkillID;
	upcursor->SkillType=m_Record.SkillType;
	upcursor->SkillLevel=m_Record.SkillLevel;
	upcursor->SkillPos=m_Record.SkillPos;
	upcursor->valid=m_Record.valid;
			upcursor.remove();
	return true;
}

bool skill::removeRecord(CConnection& Conn)
{
	CStatementPtr stmt = Conn.prepareStatement("delete from skill  where ID=?");
	stmt->setUInt64(1,m_Record.ID);
	return stmt->executeUpdate() > 0;
}

string skill::generateRemoveSQL()
{
	char tmpsql[1025];
	string sql = "delete from skill ";
	sql += " where ";
	snprintf(tmpsql,sizeof(tmpsql),"ID=%d",m_Record.ID);
	sql+=tmpsql;
	sql+=";";
	return sql;
}

bool skill::removeDB(CConnection& Conn)
{
	return removeRecord(Conn);
}

bool skill::removeAll()
{
	skill dao;
	dbCursor<_t_skill> upcursor(dbCursorForUpdate);
	upcursor.removeAll();
	return true;
}

void skill::loadDB(CConnection& Conn)
{
	skill dao;
	CStatementPtr stmt = Conn.prepareStatement("select * from skill");
	CResultSetPtr resultset = stmt->executeQuery();
	uint64 recordcount = 0;
	while ( resultset->next() )
	{
		recordcount++;
		dao.readDB(*resultset.get());
		dao.insert();
	}
	log_info("tablename[%s] recordcount[" I64_FMT "]", "skill", recordcount);
	dao.reset();
	dao.setID(0);
	dao.insertDB(Conn);
	g_Key = dao.getID() - 1;
	dao.removeRecord(Conn);
	log_info("%s id[" I64_FMT "]", "skill", g_Key);
}

uint64 skill::getKey()
{
	return g_Key;
}

void updateMemDBByResponse( const CDBLoadDBPlayerDataResponsePtr& pResponse )
{
}

uint32 loadPlayerData( uint32 nPlayerID, CDBLoadDBPlayerDataResponsePtr& pResponse )
{
	return SUCCESS;
}

void unloadPlayrData( uint32 nPlayerID )
{
}

uint32 loadPlayerDataFromDB( uint32 nPlayerID, CConnection& Conn )
{
	return SUCCESS;
}

MEM_USAGE g_MemUsage;
inline void checkMem()
{
	CMemUtil::getMemInfo(g_MemUsage);
	log_info("%s",CMemUtil::formatMemInfo(g_MemUsage));
}

void loadAll(CConnection& Conn)
{
	loadall_tb_player_items(Conn);
	log_info("loading table : account");
	account::loadDB(Conn);
	checkMem();
	log_info("loading table : arena");
	arena::loadDB(Conn);
	checkMem();
	log_info("loading table : avatar");
	avatar::loadDB(Conn);
	checkMem();
	log_info("loading table : battlechecklog");
	battlechecklog::loadDB(Conn);
	checkMem();
	log_info("loading table : battleinfo");
	battleinfo::loadDB(Conn);
	checkMem();
	log_info("loading table : friend");
	friend::loadDB(Conn);
	checkMem();
	log_info("loading table : friend_battle_award");
	friend_battle_award::loadDB(Conn);
	checkMem();
	log_info("loading table : item");
	item::loadDB(Conn);
	checkMem();
	log_info("loading table : mounts");
	mounts::loadDB(Conn);
	checkMem();
	log_info("loading table : pet");
	pet::loadDB(Conn);
	checkMem();
	log_info("loading table : player_arena_info");
	player_arena_info::loadDB(Conn);
	checkMem();
	log_info("loading table : relice");
	relice::loadDB(Conn);
	checkMem();
	log_info("loading table : skill");
	skill::loadDB(Conn);
	checkMem();
}

void reLoadStaticTables(CConnection& Conn,const char* tableName)
{
}

void statisticsTables(std::vector<TABLE_PROPERTY> &vecStatisticsTables)
{
		TABLE_PROPERTY tbProp;
		CDaoTransaction trans;
		try
		{
		trans.begin();

		account tmp_account;
		tmp_account.prepare("");
		tmp_account.find();
		tbProp.nTableName = "account";
		tbProp.nTableCount = tmp_account.getRowCount();
		tbProp.nMaxRecordID = tmp_account.getKey();
		vecStatisticsTables.push_back(tbProp);

		arena tmp_arena;
		tmp_arena.prepare("");
		tmp_arena.find();
		tbProp.nTableName = "arena";
		tbProp.nTableCount = tmp_arena.getRowCount();
		tbProp.nMaxRecordID = tmp_arena.getKey();
		vecStatisticsTables.push_back(tbProp);

		avatar tmp_avatar;
		tmp_avatar.prepare("");
		tmp_avatar.find();
		tbProp.nTableName = "avatar";
		tbProp.nTableCount = tmp_avatar.getRowCount();
		tbProp.nMaxRecordID = tmp_avatar.getKey();
		vecStatisticsTables.push_back(tbProp);

		battlechecklog tmp_battlechecklog;
		tmp_battlechecklog.prepare("");
		tmp_battlechecklog.find();
		tbProp.nTableName = "battlechecklog";
		tbProp.nTableCount = tmp_battlechecklog.getRowCount();
		tbProp.nMaxRecordID = tmp_battlechecklog.getKey();
		vecStatisticsTables.push_back(tbProp);

		battleinfo tmp_battleinfo;
		tmp_battleinfo.prepare("");
		tmp_battleinfo.find();
		tbProp.nTableName = "battleinfo";
		tbProp.nTableCount = tmp_battleinfo.getRowCount();
		tbProp.nMaxRecordID = tmp_battleinfo.getKey();
		vecStatisticsTables.push_back(tbProp);

		friend tmp_friend;
		tmp_friend.prepare("");
		tmp_friend.find();
		tbProp.nTableName = "friend";
		tbProp.nTableCount = tmp_friend.getRowCount();
		tbProp.nMaxRecordID = tmp_friend.getKey();
		vecStatisticsTables.push_back(tbProp);

		friend_battle_award tmp_friend_battle_award;
		tmp_friend_battle_award.prepare("");
		tmp_friend_battle_award.find();
		tbProp.nTableName = "friend_battle_award";
		tbProp.nTableCount = tmp_friend_battle_award.getRowCount();
		tbProp.nMaxRecordID = tmp_friend_battle_award.getKey();
		vecStatisticsTables.push_back(tbProp);

		item tmp_item;
		tmp_item.prepare("");
		tmp_item.find();
		tbProp.nTableName = "item";
		tbProp.nTableCount = tmp_item.getRowCount();
		tbProp.nMaxRecordID = tmp_item.getKey();
		vecStatisticsTables.push_back(tbProp);

		mounts tmp_mounts;
		tmp_mounts.prepare("");
		tmp_mounts.find();
		tbProp.nTableName = "mounts";
		tbProp.nTableCount = tmp_mounts.getRowCount();
		tbProp.nMaxRecordID = tmp_mounts.getKey();
		vecStatisticsTables.push_back(tbProp);

		pet tmp_pet;
		tmp_pet.prepare("");
		tmp_pet.find();
		tbProp.nTableName = "pet";
		tbProp.nTableCount = tmp_pet.getRowCount();
		tbProp.nMaxRecordID = tmp_pet.getKey();
		vecStatisticsTables.push_back(tbProp);

		player_arena_info tmp_player_arena_info;
		tmp_player_arena_info.prepare("");
		tmp_player_arena_info.find();
		tbProp.nTableName = "player_arena_info";
		tbProp.nTableCount = tmp_player_arena_info.getRowCount();
		tbProp.nMaxRecordID = tmp_player_arena_info.getKey();
		vecStatisticsTables.push_back(tbProp);

		relice tmp_relice;
		tmp_relice.prepare("");
		tmp_relice.find();
		tbProp.nTableName = "relice";
		tbProp.nTableCount = tmp_relice.getRowCount();
		tbProp.nMaxRecordID = tmp_relice.getKey();
		vecStatisticsTables.push_back(tbProp);

		skill tmp_skill;
		tmp_skill.prepare("");
		tmp_skill.find();
		tbProp.nTableName = "skill";
		tbProp.nTableCount = tmp_skill.getRowCount();
		tbProp.nMaxRecordID = tmp_skill.getKey();
		vecStatisticsTables.push_back(tbProp);

		trans.commit();
		}
		catch(CSQLException& sqle)
		{
		trans.rollback();
		}
		catch(CCException& e)
		{
		trans.rollback();
		}
		catch(...)
		{
		trans.rollback();
		}
}

