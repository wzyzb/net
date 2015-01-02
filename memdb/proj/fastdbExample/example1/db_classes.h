#ifndef _DB_CLASSES_
#define _DB_CLASSES_
#include <public.h>
#include <util/userexception.h>
#include <util/datetime.h>
#include <db/dbinterface.h>
#include <fastdb.h>
#include <dao/fastdb/fastdbdaoimpl.h>
#include <dao/mysql/mysqldaoimpl.h>
#include <dbmgr/dbtable.h>
#include <dbmgr/dbloaddbplayerdata.h>
#include <business/bussinesscommon.h>
#include "dbmgr/statisticstables.h"
#include "dbmgr/playercachedmgr.h"
struct _t_account
{
	uint64 ID; 
	uint64 AccountID; 
	char* UserName; 
	uint64 LastLoginId; 
	uint32 Diamond; 
	uint32 RecentRecharge; 
	uint16 RechargeTimes; 
	uint32 RechargeTotal; 
	TYPE_DESCRIPTOR((KEY(ID,INDEXED),FIELD(AccountID),FIELD(UserName),FIELD(LastLoginId),FIELD(Diamond),FIELD(RecentRecharge),FIELD(RechargeTimes),FIELD(RechargeTotal)));
};
class account : public CDao , public dao_mysql::CMysqlDaoImpl , public dao_fastdb::CFastDBDaoImpl 
{
protected : 
	struct _t_account m_Record,*m_pRecord;
	dbCursor<_t_account> m_Cursor;
	bool m_bHasMoreRecord;	static uint64 g_Key;	uint64 generateKey();
	char UserName[26]; 
public : 
	 account();
	void reset();
	CDBDaoImpl* clone();
	 account(const account& Other);
	bool check(const CMysqlDaoImpl* pOther) const;
	bool readMDB();
	void readDB(CResultSet& ResultSet);
	void setID(const uint64 Value){m_Record.ID = Value;}
	const uint64 getID() const {return m_Record.ID;}
	void setAccountID(const uint64 Value){m_Record.AccountID = Value;}
	const uint64 getAccountID() const {return m_Record.AccountID;}
	void setUserName(const char* Value){snprintf((char*)UserName,sizeof(UserName),"%s",Value);}
	const char* getUserName() const {return UserName;}
	void setLastLoginId(const uint64 Value){m_Record.LastLoginId = Value;}
	const uint64 getLastLoginId() const {return m_Record.LastLoginId;}
	void setDiamond(const uint32 Value){m_Record.Diamond = Value;}
	const uint32 getDiamond() const {return m_Record.Diamond;}
	void setRecentRecharge(const uint32 Value){m_Record.RecentRecharge = Value;}
	const uint32 getRecentRecharge() const {return m_Record.RecentRecharge;}
	void setRechargeTimes(const uint16 Value){m_Record.RechargeTimes = Value;}
	const uint16 getRechargeTimes() const {return m_Record.RechargeTimes;}
	void setRechargeTotal(const uint32 Value){m_Record.RechargeTotal = Value;}
	const uint32 getRechargeTotal() const {return m_Record.RechargeTotal;}
	CMDBDaoImpl* getMDBDao() { return this; }
	CDBDaoImpl* getDBDao() { return this; }
	bool nextMDB();
	int select(dbQuery& Query);
	bool insertMDB();
	bool insertMDB(const account_record& record);
	bool insertDB(CConnection& Conn);
	string generateInsertSQL();
	bool updateMDB();
	bool updateDB(CConnection& Conn);
	string generateUpdateSQL();
	bool removeMDB();
	bool removeRecord(CConnection& Conn);
	string generateRemoveSQL();
	bool removeDB(CConnection& Conn);
	static bool removeAll();
	static void loadDB(CConnection& Conn);
	uint64 getKey();
};

struct _t_arena
{
	uint64 id; 
	uint64 player_id; 
	TYPE_DESCRIPTOR((KEY(id,INDEXED),KEY(player_id,INDEXED)));
};
class arena : public CDao , public dao_mysql::CMysqlDaoImpl , public dao_fastdb::CFastDBDaoImpl 
{
protected : 
	struct _t_arena m_Record,*m_pRecord;
	dbCursor<_t_arena> m_Cursor;
	bool m_bHasMoreRecord;	static uint64 g_Key;	uint64 generateKey();
public : 
	 arena();
	void reset();
	CDBDaoImpl* clone();
	 arena(const arena& Other);
	bool check(const CMysqlDaoImpl* pOther) const;
	bool readMDB();
	void readDB(CResultSet& ResultSet);
	void setid(const uint64 Value){m_Record.id = Value;}
	const uint64 getid() const {return m_Record.id;}
	void setplayer_id(const uint64 Value){m_Record.player_id = Value;}
	const uint64 getplayer_id() const {return m_Record.player_id;}
	CMDBDaoImpl* getMDBDao() { return this; }
	CDBDaoImpl* getDBDao() { return this; }
	bool nextMDB();
	int select(dbQuery& Query);
	bool insertMDB();
	bool insertMDB(const arena_record& record);
	bool insertDB(CConnection& Conn);
	string generateInsertSQL();
	bool updateMDB();
	bool updateDB(CConnection& Conn);
	string generateUpdateSQL();
	bool removeMDB();
	bool removeRecord(CConnection& Conn);
	string generateRemoveSQL();
	bool removeDB(CConnection& Conn);
	static bool removeAll();
	static void loadDB(CConnection& Conn);
	uint64 getKey();
};

struct _t_avatar
{
	uint64 ID; 
	uint64 AccountID; 
	uint64 PlayerID; 
	uint16 PlayerLevel; 
	uint64 CreateTime; 
	uint64 LastLogoutTime; 
	uint32 MaxVigor; 
	uint32 CurVigor; 
	uint32 Money; 
	uint32 Experience; 
	uint32 PlayerJob; 
	uint16 CurChapter; 
	uint32 PartnerExp; 
	uint64 NormalRecruitTime; 
	uint64 NormalBoxOpenTime; 
	uint64 LastAddVigorTime; 
	uint32 SmeltExp; 
	uint32 ServedPartnerId; 
	TYPE_DESCRIPTOR((KEY(ID,INDEXED),KEY(AccountID,INDEXED),KEY(PlayerID,INDEXED),FIELD(PlayerLevel),FIELD(CreateTime),FIELD(LastLogoutTime),FIELD(MaxVigor),FIELD(CurVigor),FIELD(Money),FIELD(Experience),FIELD(PlayerJob),FIELD(CurChapter),FIELD(PartnerExp),FIELD(NormalRecruitTime),FIELD(NormalBoxOpenTime),FIELD(LastAddVigorTime),FIELD(SmeltExp),FIELD(ServedPartnerId)));
};
class avatar : public CDao , public dao_mysql::CMysqlDaoImpl , public dao_fastdb::CFastDBDaoImpl 
{
protected : 
	struct _t_avatar m_Record,*m_pRecord;
	dbCursor<_t_avatar> m_Cursor;
	bool m_bHasMoreRecord;	static uint64 g_Key;	uint64 generateKey();
public : 
	 avatar();
	void reset();
	CDBDaoImpl* clone();
	 avatar(const avatar& Other);
	bool check(const CMysqlDaoImpl* pOther) const;
	bool readMDB();
	void readDB(CResultSet& ResultSet);
	void setID(const uint64 Value){m_Record.ID = Value;}
	const uint64 getID() const {return m_Record.ID;}
	void setAccountID(const uint64 Value){m_Record.AccountID = Value;}
	const uint64 getAccountID() const {return m_Record.AccountID;}
	void setPlayerID(const uint64 Value){m_Record.PlayerID = Value;}
	const uint64 getPlayerID() const {return m_Record.PlayerID;}
	void setPlayerLevel(const uint16 Value){m_Record.PlayerLevel = Value;}
	const uint16 getPlayerLevel() const {return m_Record.PlayerLevel;}
	void setCreateTime(const CDateTime Value){m_Record.CreateTime=Value.getTotalMills()/1000;}
	const CDateTime getCreateTime() const {return CDateTime(m_Record.CreateTime*1000);}
	void setLastLogoutTime(const CDateTime Value){m_Record.LastLogoutTime=Value.getTotalMills()/1000;}
	const CDateTime getLastLogoutTime() const {return CDateTime(m_Record.LastLogoutTime*1000);}
	void setMaxVigor(const uint32 Value){m_Record.MaxVigor = Value;}
	const uint32 getMaxVigor() const {return m_Record.MaxVigor;}
	void setCurVigor(const uint32 Value){m_Record.CurVigor = Value;}
	const uint32 getCurVigor() const {return m_Record.CurVigor;}
	void setMoney(const uint32 Value){m_Record.Money = Value;}
	const uint32 getMoney() const {return m_Record.Money;}
	void setExperience(const uint32 Value){m_Record.Experience = Value;}
	const uint32 getExperience() const {return m_Record.Experience;}
	void setPlayerJob(const uint32 Value){m_Record.PlayerJob = Value;}
	const uint32 getPlayerJob() const {return m_Record.PlayerJob;}
	void setCurChapter(const uint16 Value){m_Record.CurChapter = Value;}
	const uint16 getCurChapter() const {return m_Record.CurChapter;}
	void setPartnerExp(const uint32 Value){m_Record.PartnerExp = Value;}
	const uint32 getPartnerExp() const {return m_Record.PartnerExp;}
	void setNormalRecruitTime(const CDateTime Value){m_Record.NormalRecruitTime=Value.getTotalMills()/1000;}
	const CDateTime getNormalRecruitTime() const {return CDateTime(m_Record.NormalRecruitTime*1000);}
	void setNormalBoxOpenTime(const CDateTime Value){m_Record.NormalBoxOpenTime=Value.getTotalMills()/1000;}
	const CDateTime getNormalBoxOpenTime() const {return CDateTime(m_Record.NormalBoxOpenTime*1000);}
	void setLastAddVigorTime(const CDateTime Value){m_Record.LastAddVigorTime=Value.getTotalMills()/1000;}
	const CDateTime getLastAddVigorTime() const {return CDateTime(m_Record.LastAddVigorTime*1000);}
	void setSmeltExp(const uint32 Value){m_Record.SmeltExp = Value;}
	const uint32 getSmeltExp() const {return m_Record.SmeltExp;}
	void setServedPartnerId(const uint32 Value){m_Record.ServedPartnerId = Value;}
	const uint32 getServedPartnerId() const {return m_Record.ServedPartnerId;}
	CMDBDaoImpl* getMDBDao() { return this; }
	CDBDaoImpl* getDBDao() { return this; }
	bool nextMDB();
	int select(dbQuery& Query);
	bool insertMDB();
	bool insertMDB(const avatar_record& record);
	bool insertDB(CConnection& Conn);
	string generateInsertSQL();
	bool updateMDB();
	bool updateDB(CConnection& Conn);
	string generateUpdateSQL();
	bool removeMDB();
	bool removeRecord(CConnection& Conn);
	string generateRemoveSQL();
	bool removeDB(CConnection& Conn);
	static bool removeAll();
	static void loadDB(CConnection& Conn);
	uint64 getKey();
};

struct _t_battlechecklog
{
	uint64 id; 
	uint32 BattlemapId; 
	uint64 PlayerId; 
	uint16 CheckResult; 
	uint64 Create_time; 
	TYPE_DESCRIPTOR((KEY(id,INDEXED),FIELD(BattlemapId),FIELD(PlayerId),FIELD(CheckResult),FIELD(Create_time)));
};
class battlechecklog : public CDao , public dao_mysql::CMysqlDaoImpl , public dao_fastdb::CFastDBDaoImpl 
{
protected : 
	struct _t_battlechecklog m_Record,*m_pRecord;
	dbCursor<_t_battlechecklog> m_Cursor;
	bool m_bHasMoreRecord;	static uint64 g_Key;	uint64 generateKey();
public : 
	 battlechecklog();
	void reset();
	CDBDaoImpl* clone();
	 battlechecklog(const battlechecklog& Other);
	bool check(const CMysqlDaoImpl* pOther) const;
	bool readMDB();
	void readDB(CResultSet& ResultSet);
	void setid(const uint64 Value){m_Record.id = Value;}
	const uint64 getid() const {return m_Record.id;}
	void setBattlemapId(const uint32 Value){m_Record.BattlemapId = Value;}
	const uint32 getBattlemapId() const {return m_Record.BattlemapId;}
	void setPlayerId(const uint64 Value){m_Record.PlayerId = Value;}
	const uint64 getPlayerId() const {return m_Record.PlayerId;}
	void setCheckResult(const uint16 Value){m_Record.CheckResult = Value;}
	const uint16 getCheckResult() const {return m_Record.CheckResult;}
	void setCreate_time(const CDateTime Value){m_Record.Create_time=Value.getTotalMills()/1000;}
	const CDateTime getCreate_time() const {return CDateTime(m_Record.Create_time*1000);}
	CMDBDaoImpl* getMDBDao() { return this; }
	CDBDaoImpl* getDBDao() { return this; }
	bool nextMDB();
	int select(dbQuery& Query);
	bool insertMDB();
	bool insertMDB(const battlechecklog_record& record);
	bool insertDB(CConnection& Conn);
	string generateInsertSQL();
	bool updateMDB();
	bool updateDB(CConnection& Conn);
	string generateUpdateSQL();
	bool removeMDB();
	bool removeRecord(CConnection& Conn);
	string generateRemoveSQL();
	bool removeDB(CConnection& Conn);
	static bool removeAll();
	static void loadDB(CConnection& Conn);
	uint64 getKey();
};

struct _t_battleinfo
{
	uint64 id; 
	uint64 playerid; 
	uint32 battlemapid; 
	uint16 star; 
	uint8 valid; 
	uint64 last_modify_date; 
	uint64 create_date; 
	TYPE_DESCRIPTOR((KEY(id,INDEXED),KEY(playerid,INDEXED),FIELD(battlemapid),FIELD(star),FIELD(valid),FIELD(last_modify_date),FIELD(create_date)));
};
class battleinfo : public CDao , public dao_mysql::CMysqlDaoImpl , public dao_fastdb::CFastDBDaoImpl 
{
protected : 
	struct _t_battleinfo m_Record,*m_pRecord;
	dbCursor<_t_battleinfo> m_Cursor;
	bool m_bHasMoreRecord;	static uint64 g_Key;	uint64 generateKey();
public : 
	 battleinfo();
	void reset();
	CDBDaoImpl* clone();
	 battleinfo(const battleinfo& Other);
	bool check(const CMysqlDaoImpl* pOther) const;
	bool readMDB();
	void readDB(CResultSet& ResultSet);
	void setid(const uint64 Value){m_Record.id = Value;}
	const uint64 getid() const {return m_Record.id;}
	void setplayerid(const uint64 Value){m_Record.playerid = Value;}
	const uint64 getplayerid() const {return m_Record.playerid;}
	void setbattlemapid(const uint32 Value){m_Record.battlemapid = Value;}
	const uint32 getbattlemapid() const {return m_Record.battlemapid;}
	void setstar(const uint16 Value){m_Record.star = Value;}
	const uint16 getstar() const {return m_Record.star;}
	void setvalid(const uint8 Value){m_Record.valid = Value;}
	const uint8 getvalid() const {return m_Record.valid;}
	void setlast_modify_date(const CDateTime Value){m_Record.last_modify_date=Value.getTotalMills()/1000;}
	const CDateTime getlast_modify_date() const {return CDateTime(m_Record.last_modify_date*1000);}
	void setcreate_date(const CDateTime Value){m_Record.create_date=Value.getTotalMills()/1000;}
	const CDateTime getcreate_date() const {return CDateTime(m_Record.create_date*1000);}
	CMDBDaoImpl* getMDBDao() { return this; }
	CDBDaoImpl* getDBDao() { return this; }
	bool nextMDB();
	int select(dbQuery& Query);
	bool insertMDB();
	bool insertMDB(const battleinfo_record& record);
	bool insertDB(CConnection& Conn);
	string generateInsertSQL();
	bool updateMDB();
	bool updateDB(CConnection& Conn);
	string generateUpdateSQL();
	bool removeMDB();
	bool removeRecord(CConnection& Conn);
	string generateRemoveSQL();
	bool removeDB(CConnection& Conn);
	static bool removeAll();
	static void loadDB(CConnection& Conn);
	uint64 getKey();
};

struct _t_friend
{
	uint64 ID; 
	uint64 AccountID; 
	uint64 FriendAccountID; 
	uint64 LastGiveVigorTime; 
	uint64 LastAssistTime; 
	TYPE_DESCRIPTOR((KEY(ID,INDEXED),FIELD(AccountID),FIELD(FriendAccountID),FIELD(LastGiveVigorTime),FIELD(LastAssistTime)));
};
class friend : public CDao , public dao_mysql::CMysqlDaoImpl , public dao_fastdb::CFastDBDaoImpl 
{
protected : 
	struct _t_friend m_Record,*m_pRecord;
	dbCursor<_t_friend> m_Cursor;
	bool m_bHasMoreRecord;	static uint64 g_Key;	uint64 generateKey();
public : 
	 friend();
	void reset();
	CDBDaoImpl* clone();
	 friend(const friend& Other);
	bool check(const CMysqlDaoImpl* pOther) const;
	bool readMDB();
	void readDB(CResultSet& ResultSet);
	void setID(const uint64 Value){m_Record.ID = Value;}
	const uint64 getID() const {return m_Record.ID;}
	void setAccountID(const uint64 Value){m_Record.AccountID = Value;}
	const uint64 getAccountID() const {return m_Record.AccountID;}
	void setFriendAccountID(const uint64 Value){m_Record.FriendAccountID = Value;}
	const uint64 getFriendAccountID() const {return m_Record.FriendAccountID;}
	void setLastGiveVigorTime(const CDateTime Value){m_Record.LastGiveVigorTime=Value.getTotalMills()/1000;}
	const CDateTime getLastGiveVigorTime() const {return CDateTime(m_Record.LastGiveVigorTime*1000);}
	void setLastAssistTime(const CDateTime Value){m_Record.LastAssistTime=Value.getTotalMills()/1000;}
	const CDateTime getLastAssistTime() const {return CDateTime(m_Record.LastAssistTime*1000);}
	CMDBDaoImpl* getMDBDao() { return this; }
	CDBDaoImpl* getDBDao() { return this; }
	bool nextMDB();
	int select(dbQuery& Query);
	bool insertMDB();
	bool insertMDB(const friend_record& record);
	bool insertDB(CConnection& Conn);
	string generateInsertSQL();
	bool updateMDB();
	bool updateDB(CConnection& Conn);
	string generateUpdateSQL();
	bool removeMDB();
	bool removeRecord(CConnection& Conn);
	string generateRemoveSQL();
	bool removeDB(CConnection& Conn);
	static bool removeAll();
	static void loadDB(CConnection& Conn);
	uint64 getKey();
};

struct _t_friend_battle_award
{
	uint64 id; 
	uint64 player_id; 
	uint32 award_exp; 
	uint32 award_silver; 
	uint32 award_vigor; 
	uint64 create_time; 
	TYPE_DESCRIPTOR((KEY(id,INDEXED),KEY(player_id,INDEXED),FIELD(award_exp),FIELD(award_silver),FIELD(award_vigor),FIELD(create_time)));
};
class friend_battle_award : public CDao , public dao_mysql::CMysqlDaoImpl , public dao_fastdb::CFastDBDaoImpl 
{
protected : 
	struct _t_friend_battle_award m_Record,*m_pRecord;
	dbCursor<_t_friend_battle_award> m_Cursor;
	bool m_bHasMoreRecord;	static uint64 g_Key;	uint64 generateKey();
public : 
	 friend_battle_award();
	void reset();
	CDBDaoImpl* clone();
	 friend_battle_award(const friend_battle_award& Other);
	bool check(const CMysqlDaoImpl* pOther) const;
	bool readMDB();
	void readDB(CResultSet& ResultSet);
	void setid(const uint64 Value){m_Record.id = Value;}
	const uint64 getid() const {return m_Record.id;}
	void setplayer_id(const uint64 Value){m_Record.player_id = Value;}
	const uint64 getplayer_id() const {return m_Record.player_id;}
	void setaward_exp(const uint32 Value){m_Record.award_exp = Value;}
	const uint32 getaward_exp() const {return m_Record.award_exp;}
	void setaward_silver(const uint32 Value){m_Record.award_silver = Value;}
	const uint32 getaward_silver() const {return m_Record.award_silver;}
	void setaward_vigor(const uint32 Value){m_Record.award_vigor = Value;}
	const uint32 getaward_vigor() const {return m_Record.award_vigor;}
	void setcreate_time(const CDateTime Value){m_Record.create_time=Value.getTotalMills()/1000;}
	const CDateTime getcreate_time() const {return CDateTime(m_Record.create_time*1000);}
	CMDBDaoImpl* getMDBDao() { return this; }
	CDBDaoImpl* getDBDao() { return this; }
	bool nextMDB();
	int select(dbQuery& Query);
	bool insertMDB();
	bool insertMDB(const friend_battle_award_record& record);
	bool insertDB(CConnection& Conn);
	string generateInsertSQL();
	bool updateMDB();
	bool updateDB(CConnection& Conn);
	string generateUpdateSQL();
	bool removeMDB();
	bool removeRecord(CConnection& Conn);
	string generateRemoveSQL();
	bool removeDB(CConnection& Conn);
	static bool removeAll();
	static void loadDB(CConnection& Conn);
	uint64 getKey();
};

struct _t_item
{
	uint64 ID; 
	uint64 PlayerID; 
	uint32 ItemID; 
	uint8 ItemPos; 
	uint8 ItemNum; 
	TYPE_DESCRIPTOR((KEY(ID,INDEXED),KEY(PlayerID,INDEXED),FIELD(ItemID),FIELD(ItemPos),FIELD(ItemNum)));
};
class item : public CDao , public dao_mysql::CMysqlDaoImpl , public dao_fastdb::CFastDBDaoImpl 
{
protected : 
	struct _t_item m_Record,*m_pRecord;
	dbCursor<_t_item> m_Cursor;
	bool m_bHasMoreRecord;	static uint64 g_Key;	uint64 generateKey();
public : 
	 item();
	void reset();
	CDBDaoImpl* clone();
	 item(const item& Other);
	bool check(const CMysqlDaoImpl* pOther) const;
	bool readMDB();
	void readDB(CResultSet& ResultSet);
	void setID(const uint64 Value){m_Record.ID = Value;}
	const uint64 getID() const {return m_Record.ID;}
	void setPlayerID(const uint64 Value){m_Record.PlayerID = Value;}
	const uint64 getPlayerID() const {return m_Record.PlayerID;}
	void setItemID(const uint32 Value){m_Record.ItemID = Value;}
	const uint32 getItemID() const {return m_Record.ItemID;}
	void setItemPos(const uint8 Value){m_Record.ItemPos = Value;}
	const uint8 getItemPos() const {return m_Record.ItemPos;}
	void setItemNum(const uint8 Value){m_Record.ItemNum = Value;}
	const uint8 getItemNum() const {return m_Record.ItemNum;}
	CMDBDaoImpl* getMDBDao() { return this; }
	CDBDaoImpl* getDBDao() { return this; }
	bool nextMDB();
	int select(dbQuery& Query);
	bool insertMDB();
	bool insertMDB(const item_record& record);
	bool insertDB(CConnection& Conn);
	string generateInsertSQL();
	bool updateMDB();
	bool updateDB(CConnection& Conn);
	string generateUpdateSQL();
	bool removeMDB();
	bool removeRecord(CConnection& Conn);
	string generateRemoveSQL();
	bool removeDB(CConnection& Conn);
	static bool removeAll();
	static void loadDB(CConnection& Conn);
	uint64 getKey();
};

struct _t_mounts
{
	uint64 ID; 
	uint64 PlayerID; 
	uint32 MountsID; 
	uint16 MountsExp; 
	TYPE_DESCRIPTOR((KEY(ID,INDEXED),KEY(PlayerID,INDEXED),FIELD(MountsID),FIELD(MountsExp)));
};
class mounts : public CDao , public dao_mysql::CMysqlDaoImpl , public dao_fastdb::CFastDBDaoImpl 
{
protected : 
	struct _t_mounts m_Record,*m_pRecord;
	dbCursor<_t_mounts> m_Cursor;
	bool m_bHasMoreRecord;	static uint64 g_Key;	uint64 generateKey();
public : 
	 mounts();
	void reset();
	CDBDaoImpl* clone();
	 mounts(const mounts& Other);
	bool check(const CMysqlDaoImpl* pOther) const;
	bool readMDB();
	void readDB(CResultSet& ResultSet);
	void setID(const uint64 Value){m_Record.ID = Value;}
	const uint64 getID() const {return m_Record.ID;}
	void setPlayerID(const uint64 Value){m_Record.PlayerID = Value;}
	const uint64 getPlayerID() const {return m_Record.PlayerID;}
	void setMountsID(const uint32 Value){m_Record.MountsID = Value;}
	const uint32 getMountsID() const {return m_Record.MountsID;}
	void setMountsExp(const uint16 Value){m_Record.MountsExp = Value;}
	const uint16 getMountsExp() const {return m_Record.MountsExp;}
	CMDBDaoImpl* getMDBDao() { return this; }
	CDBDaoImpl* getDBDao() { return this; }
	bool nextMDB();
	int select(dbQuery& Query);
	bool insertMDB();
	bool insertMDB(const mounts_record& record);
	bool insertDB(CConnection& Conn);
	string generateInsertSQL();
	bool updateMDB();
	bool updateDB(CConnection& Conn);
	string generateUpdateSQL();
	bool removeMDB();
	bool removeRecord(CConnection& Conn);
	string generateRemoveSQL();
	bool removeDB(CConnection& Conn);
	static bool removeAll();
	static void loadDB(CConnection& Conn);
	uint64 getKey();
};

struct _t_pet
{
	uint64 ID; 
	uint64 PlayerID; 
	uint32 PetID; 
	uint16 PetExp; 
	TYPE_DESCRIPTOR((KEY(ID,INDEXED),KEY(PlayerID,INDEXED),FIELD(PetID),FIELD(PetExp)));
};
class pet : public CDao , public dao_mysql::CMysqlDaoImpl , public dao_fastdb::CFastDBDaoImpl 
{
protected : 
	struct _t_pet m_Record,*m_pRecord;
	dbCursor<_t_pet> m_Cursor;
	bool m_bHasMoreRecord;	static uint64 g_Key;	uint64 generateKey();
public : 
	 pet();
	void reset();
	CDBDaoImpl* clone();
	 pet(const pet& Other);
	bool check(const CMysqlDaoImpl* pOther) const;
	bool readMDB();
	void readDB(CResultSet& ResultSet);
	void setID(const uint64 Value){m_Record.ID = Value;}
	const uint64 getID() const {return m_Record.ID;}
	void setPlayerID(const uint64 Value){m_Record.PlayerID = Value;}
	const uint64 getPlayerID() const {return m_Record.PlayerID;}
	void setPetID(const uint32 Value){m_Record.PetID = Value;}
	const uint32 getPetID() const {return m_Record.PetID;}
	void setPetExp(const uint16 Value){m_Record.PetExp = Value;}
	const uint16 getPetExp() const {return m_Record.PetExp;}
	CMDBDaoImpl* getMDBDao() { return this; }
	CDBDaoImpl* getDBDao() { return this; }
	bool nextMDB();
	int select(dbQuery& Query);
	bool insertMDB();
	bool insertMDB(const pet_record& record);
	bool insertDB(CConnection& Conn);
	string generateInsertSQL();
	bool updateMDB();
	bool updateDB(CConnection& Conn);
	string generateUpdateSQL();
	bool removeMDB();
	bool removeRecord(CConnection& Conn);
	string generateRemoveSQL();
	bool removeDB(CConnection& Conn);
	static bool removeAll();
	static void loadDB(CConnection& Conn);
	uint64 getKey();
};

struct _t_player_arena_info
{
	uint64 id; 
	uint64 player_id; 
	uint32 num; 
	uint64 date; 
	uint32 cd_time; 
	uint64 cd_start; 
	TYPE_DESCRIPTOR((KEY(id,INDEXED),FIELD(player_id),FIELD(num),FIELD(date),FIELD(cd_time),FIELD(cd_start)));
};
class player_arena_info : public CDao , public dao_mysql::CMysqlDaoImpl , public dao_fastdb::CFastDBDaoImpl 
{
protected : 
	struct _t_player_arena_info m_Record,*m_pRecord;
	dbCursor<_t_player_arena_info> m_Cursor;
	bool m_bHasMoreRecord;	static uint64 g_Key;	uint64 generateKey();
public : 
	 player_arena_info();
	void reset();
	CDBDaoImpl* clone();
	 player_arena_info(const player_arena_info& Other);
	bool check(const CMysqlDaoImpl* pOther) const;
	bool readMDB();
	void readDB(CResultSet& ResultSet);
	void setid(const uint64 Value){m_Record.id = Value;}
	const uint64 getid() const {return m_Record.id;}
	void setplayer_id(const uint64 Value){m_Record.player_id = Value;}
	const uint64 getplayer_id() const {return m_Record.player_id;}
	void setnum(const uint32 Value){m_Record.num = Value;}
	const uint32 getnum() const {return m_Record.num;}
	void setdate(const CDateTime Value){m_Record.date=Value.getTotalMills()/1000;}
	const CDateTime getdate() const {return CDateTime(m_Record.date*1000);}
	void setcd_time(const uint32 Value){m_Record.cd_time = Value;}
	const uint32 getcd_time() const {return m_Record.cd_time;}
	void setcd_start(const CDateTime Value){m_Record.cd_start=Value.getTotalMills()/1000;}
	const CDateTime getcd_start() const {return CDateTime(m_Record.cd_start*1000);}
	CMDBDaoImpl* getMDBDao() { return this; }
	CDBDaoImpl* getDBDao() { return this; }
	bool nextMDB();
	int select(dbQuery& Query);
	bool insertMDB();
	bool insertMDB(const player_arena_info_record& record);
	bool insertDB(CConnection& Conn);
	string generateInsertSQL();
	bool updateMDB();
	bool updateDB(CConnection& Conn);
	string generateUpdateSQL();
	bool removeMDB();
	bool removeRecord(CConnection& Conn);
	string generateRemoveSQL();
	bool removeDB(CConnection& Conn);
	static bool removeAll();
	static void loadDB(CConnection& Conn);
	uint64 getKey();
};

struct _t_relice
{
	uint64 ID; 
	uint64 PlayerID; 
	uint32 ReliceID; 
	uint8 ActiveAttr; 
	uint16 GemHole1Level; 
	uint32 GemHole1CurExp; 
	uint16 GemHole2Level; 
	uint16 GemHole2CurExp; 
	uint16 GemHole3Level; 
	uint16 GemHole3CurExp; 
	TYPE_DESCRIPTOR((KEY(ID,INDEXED),KEY(PlayerID,INDEXED),FIELD(ReliceID),FIELD(ActiveAttr),FIELD(GemHole1Level),FIELD(GemHole1CurExp),FIELD(GemHole2Level),FIELD(GemHole2CurExp),FIELD(GemHole3Level),FIELD(GemHole3CurExp)));
};
class relice : public CDao , public dao_mysql::CMysqlDaoImpl , public dao_fastdb::CFastDBDaoImpl 
{
protected : 
	struct _t_relice m_Record,*m_pRecord;
	dbCursor<_t_relice> m_Cursor;
	bool m_bHasMoreRecord;	static uint64 g_Key;	uint64 generateKey();
public : 
	 relice();
	void reset();
	CDBDaoImpl* clone();
	 relice(const relice& Other);
	bool check(const CMysqlDaoImpl* pOther) const;
	bool readMDB();
	void readDB(CResultSet& ResultSet);
	void setID(const uint64 Value){m_Record.ID = Value;}
	const uint64 getID() const {return m_Record.ID;}
	void setPlayerID(const uint64 Value){m_Record.PlayerID = Value;}
	const uint64 getPlayerID() const {return m_Record.PlayerID;}
	void setReliceID(const uint32 Value){m_Record.ReliceID = Value;}
	const uint32 getReliceID() const {return m_Record.ReliceID;}
	void setActiveAttr(const uint8 Value){m_Record.ActiveAttr = Value;}
	const uint8 getActiveAttr() const {return m_Record.ActiveAttr;}
	void setGemHole1Level(const uint16 Value){m_Record.GemHole1Level = Value;}
	const uint16 getGemHole1Level() const {return m_Record.GemHole1Level;}
	void setGemHole1CurExp(const uint32 Value){m_Record.GemHole1CurExp = Value;}
	const uint32 getGemHole1CurExp() const {return m_Record.GemHole1CurExp;}
	void setGemHole2Level(const uint16 Value){m_Record.GemHole2Level = Value;}
	const uint16 getGemHole2Level() const {return m_Record.GemHole2Level;}
	void setGemHole2CurExp(const uint16 Value){m_Record.GemHole2CurExp = Value;}
	const uint16 getGemHole2CurExp() const {return m_Record.GemHole2CurExp;}
	void setGemHole3Level(const uint16 Value){m_Record.GemHole3Level = Value;}
	const uint16 getGemHole3Level() const {return m_Record.GemHole3Level;}
	void setGemHole3CurExp(const uint16 Value){m_Record.GemHole3CurExp = Value;}
	const uint16 getGemHole3CurExp() const {return m_Record.GemHole3CurExp;}
	CMDBDaoImpl* getMDBDao() { return this; }
	CDBDaoImpl* getDBDao() { return this; }
	bool nextMDB();
	int select(dbQuery& Query);
	bool insertMDB();
	bool insertMDB(const relice_record& record);
	bool insertDB(CConnection& Conn);
	string generateInsertSQL();
	bool updateMDB();
	bool updateDB(CConnection& Conn);
	string generateUpdateSQL();
	bool removeMDB();
	bool removeRecord(CConnection& Conn);
	string generateRemoveSQL();
	bool removeDB(CConnection& Conn);
	static bool removeAll();
	static void loadDB(CConnection& Conn);
	uint64 getKey();
};

struct _t_skill
{
	uint64 ID; 
	uint64 PlayerID; 
	uint32 SkillID; 
	uint8 SkillType; 
	uint8 SkillLevel; 
	uint8 SkillPos; 
	uint8 valid; 
	uint64 create_date; 
	uint64 last_modify_date; 
	TYPE_DESCRIPTOR((KEY(ID,INDEXED),KEY(PlayerID,INDEXED),FIELD(SkillID),FIELD(SkillType),FIELD(SkillLevel),FIELD(SkillPos),FIELD(valid),FIELD(create_date),FIELD(last_modify_date)));
};
class skill : public CDao , public dao_mysql::CMysqlDaoImpl , public dao_fastdb::CFastDBDaoImpl 
{
protected : 
	struct _t_skill m_Record,*m_pRecord;
	dbCursor<_t_skill> m_Cursor;
	bool m_bHasMoreRecord;	static uint64 g_Key;	uint64 generateKey();
public : 
	 skill();
	void reset();
	CDBDaoImpl* clone();
	 skill(const skill& Other);
	bool check(const CMysqlDaoImpl* pOther) const;
	bool readMDB();
	void readDB(CResultSet& ResultSet);
	void setID(const uint64 Value){m_Record.ID = Value;}
	const uint64 getID() const {return m_Record.ID;}
	void setPlayerID(const uint64 Value){m_Record.PlayerID = Value;}
	const uint64 getPlayerID() const {return m_Record.PlayerID;}
	void setSkillID(const uint32 Value){m_Record.SkillID = Value;}
	const uint32 getSkillID() const {return m_Record.SkillID;}
	void setSkillType(const uint8 Value){m_Record.SkillType = Value;}
	const uint8 getSkillType() const {return m_Record.SkillType;}
	void setSkillLevel(const uint8 Value){m_Record.SkillLevel = Value;}
	const uint8 getSkillLevel() const {return m_Record.SkillLevel;}
	void setSkillPos(const uint8 Value){m_Record.SkillPos = Value;}
	const uint8 getSkillPos() const {return m_Record.SkillPos;}
	void setvalid(const uint8 Value){m_Record.valid = Value;}
	const uint8 getvalid() const {return m_Record.valid;}
	void setcreate_date(const CDateTime Value){m_Record.create_date=Value.getTotalMills()/1000;}
	const CDateTime getcreate_date() const {return CDateTime(m_Record.create_date*1000);}
	void setlast_modify_date(const CDateTime Value){m_Record.last_modify_date=Value.getTotalMills()/1000;}
	const CDateTime getlast_modify_date() const {return CDateTime(m_Record.last_modify_date*1000);}
	CMDBDaoImpl* getMDBDao() { return this; }
	CDBDaoImpl* getDBDao() { return this; }
	bool nextMDB();
	int select(dbQuery& Query);
	bool insertMDB();
	bool insertMDB(const skill_record& record);
	bool insertDB(CConnection& Conn);
	string generateInsertSQL();
	bool updateMDB();
	bool updateDB(CConnection& Conn);
	string generateUpdateSQL();
	bool removeMDB();
	bool removeRecord(CConnection& Conn);
	string generateRemoveSQL();
	bool removeDB(CConnection& Conn);
	static bool removeAll();
	static void loadDB(CConnection& Conn);
	uint64 getKey();
};

	void updateMemDBByResponse( const CDBLoadDBPlayerDataResponsePtr& pResponse );
	uint32 loadPlayerData( uint32 nPlayerID, CDBLoadDBPlayerDataResponsePtr& pResponse );
	void unloadPlayrData( uint32 nPlayerID );
	uint32 loadPlayerDataFromDB( uint32 nPlayerID, CConnection& Conn );
	inline void checkMem();
	void loadAll(CConnection& Conn);
	void reLoadStaticTables(CConnection& Conn,const char* tableName);
	void statisticsTables(std::vector<TABLE_PROPERTY> &vecStatisticsTables);
#endif
