#ifndef _DBTABLE_H_
#define _DBTABLE_H_

#include <public.h>
#include <framework/serial.h>
#include <framework/request.h>
#include <framework/response.h>
#include <framework/commonptl.h>



class account_record : public ISerial
{
public:
	uint64 ID;
	uint64 AccountID;
	std::string UserName;
	uint64 LastLoginId;
	uint32 Diamond;
	uint32 RecentRecharge;
	uint16 RechargeTimes;
	uint32 RechargeTotal;

	virtual bool serial(char *pOut, int &Length){return true;}
	virtual bool serial(IOutputStream& Output)
	{
		writeuint64(Output, ID);
		writeuint64(Output, AccountID);
		writestring(Output, UserName);
		writeuint64(Output, LastLoginId);
		writeuint32(Output, Diamond);
		writeuint32(Output, RecentRecharge);
		writeuint16(Output, RechargeTimes);
		writeuint32(Output, RechargeTotal);
		return true;
	}
	virtual bool unserial(const char *pIn, int Length){return true;}
	virtual bool unserial(IInputStream& Input)
	{
		readuint64(Input, ID);
		readuint64(Input, AccountID);
		readstring(Input, UserName);
		readuint64(Input, LastLoginId);
		readuint32(Input, Diamond);
		readuint32(Input, RecentRecharge);
		readuint16(Input, RechargeTimes);
		readuint32(Input, RechargeTotal);
		return true;
	}
	account_record()
	{
		ID = 0;
		AccountID = 0;
		LastLoginId = 0;
		Diamond = 0;
		RecentRecharge = 0;
		RechargeTimes = 0;
		RechargeTotal = 0;
	}
	account_record(const account_record &t)
	{
		ID = t.ID;
		AccountID = t.AccountID;
		UserName = t.UserName;
		LastLoginId = t.LastLoginId;
		Diamond = t.Diamond;
		RecentRecharge = t.RecentRecharge;
		RechargeTimes = t.RechargeTimes;
		RechargeTotal = t.RechargeTotal;
	}
	account_record& operator = (const account_record& t)
	{
		ID = t.ID;
		AccountID = t.AccountID;
		UserName = t.UserName;
		LastLoginId = t.LastLoginId;
		Diamond = t.Diamond;
		RecentRecharge = t.RecentRecharge;
		RechargeTimes = t.RechargeTimes;
		RechargeTotal = t.RechargeTotal;
		return *this;
	}
	virtual ~account_record()
	{
	}
};

typedef CRefObject<account_record> account_recordPtr;


class arena_record : public ISerial
{
public:
	uint64 id;
	uint64 player_id;

	virtual bool serial(char *pOut, int &Length){return true;}
	virtual bool serial(IOutputStream& Output)
	{
		writeuint64(Output, id);
		writeuint64(Output, player_id);
		return true;
	}
	virtual bool unserial(const char *pIn, int Length){return true;}
	virtual bool unserial(IInputStream& Input)
	{
		readuint64(Input, id);
		readuint64(Input, player_id);
		return true;
	}
	arena_record()
	{
		id = 0;
		player_id = 0;
	}
	arena_record(const arena_record &t)
	{
		id = t.id;
		player_id = t.player_id;
	}
	arena_record& operator = (const arena_record& t)
	{
		id = t.id;
		player_id = t.player_id;
		return *this;
	}
	virtual ~arena_record()
	{
	}
};

typedef CRefObject<arena_record> arena_recordPtr;


class avatar_record : public ISerial
{
public:
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

	virtual bool serial(char *pOut, int &Length){return true;}
	virtual bool serial(IOutputStream& Output)
	{
		writeuint64(Output, ID);
		writeuint64(Output, AccountID);
		writeuint64(Output, PlayerID);
		writeuint16(Output, PlayerLevel);
		writeuint64(Output, CreateTime);
		writeuint64(Output, LastLogoutTime);
		writeuint32(Output, MaxVigor);
		writeuint32(Output, CurVigor);
		writeuint32(Output, Money);
		writeuint32(Output, Experience);
		writeuint32(Output, PlayerJob);
		writeuint16(Output, CurChapter);
		writeuint32(Output, PartnerExp);
		writeuint64(Output, NormalRecruitTime);
		writeuint64(Output, NormalBoxOpenTime);
		writeuint64(Output, LastAddVigorTime);
		writeuint32(Output, SmeltExp);
		writeuint32(Output, ServedPartnerId);
		return true;
	}
	virtual bool unserial(const char *pIn, int Length){return true;}
	virtual bool unserial(IInputStream& Input)
	{
		readuint64(Input, ID);
		readuint64(Input, AccountID);
		readuint64(Input, PlayerID);
		readuint16(Input, PlayerLevel);
		readuint64(Input, CreateTime);
		readuint64(Input, LastLogoutTime);
		readuint32(Input, MaxVigor);
		readuint32(Input, CurVigor);
		readuint32(Input, Money);
		readuint32(Input, Experience);
		readuint32(Input, PlayerJob);
		readuint16(Input, CurChapter);
		readuint32(Input, PartnerExp);
		readuint64(Input, NormalRecruitTime);
		readuint64(Input, NormalBoxOpenTime);
		readuint64(Input, LastAddVigorTime);
		readuint32(Input, SmeltExp);
		readuint32(Input, ServedPartnerId);
		return true;
	}
	avatar_record()
	{
		ID = 0;
		AccountID = 0;
		PlayerID = 0;
		PlayerLevel = 0;
		CreateTime = 0;
		LastLogoutTime = 0;
		MaxVigor = 0;
		CurVigor = 0;
		Money = 0;
		Experience = 0;
		PlayerJob = 0;
		CurChapter = 0;
		PartnerExp = 0;
		NormalRecruitTime = 0;
		NormalBoxOpenTime = 0;
		LastAddVigorTime = 0;
		SmeltExp = 0;
		ServedPartnerId = 0;
	}
	avatar_record(const avatar_record &t)
	{
		ID = t.ID;
		AccountID = t.AccountID;
		PlayerID = t.PlayerID;
		PlayerLevel = t.PlayerLevel;
		CreateTime = t.CreateTime;
		LastLogoutTime = t.LastLogoutTime;
		MaxVigor = t.MaxVigor;
		CurVigor = t.CurVigor;
		Money = t.Money;
		Experience = t.Experience;
		PlayerJob = t.PlayerJob;
		CurChapter = t.CurChapter;
		PartnerExp = t.PartnerExp;
		NormalRecruitTime = t.NormalRecruitTime;
		NormalBoxOpenTime = t.NormalBoxOpenTime;
		LastAddVigorTime = t.LastAddVigorTime;
		SmeltExp = t.SmeltExp;
		ServedPartnerId = t.ServedPartnerId;
	}
	avatar_record& operator = (const avatar_record& t)
	{
		ID = t.ID;
		AccountID = t.AccountID;
		PlayerID = t.PlayerID;
		PlayerLevel = t.PlayerLevel;
		CreateTime = t.CreateTime;
		LastLogoutTime = t.LastLogoutTime;
		MaxVigor = t.MaxVigor;
		CurVigor = t.CurVigor;
		Money = t.Money;
		Experience = t.Experience;
		PlayerJob = t.PlayerJob;
		CurChapter = t.CurChapter;
		PartnerExp = t.PartnerExp;
		NormalRecruitTime = t.NormalRecruitTime;
		NormalBoxOpenTime = t.NormalBoxOpenTime;
		LastAddVigorTime = t.LastAddVigorTime;
		SmeltExp = t.SmeltExp;
		ServedPartnerId = t.ServedPartnerId;
		return *this;
	}
	virtual ~avatar_record()
	{
	}
};

typedef CRefObject<avatar_record> avatar_recordPtr;


class battlechecklog_record : public ISerial
{
public:
	uint64 id;
	uint32 BattlemapId;
	uint64 PlayerId;
	uint16 CheckResult;
	uint64 Create_time;

	virtual bool serial(char *pOut, int &Length){return true;}
	virtual bool serial(IOutputStream& Output)
	{
		writeuint64(Output, id);
		writeuint32(Output, BattlemapId);
		writeuint64(Output, PlayerId);
		writeuint16(Output, CheckResult);
		writeuint64(Output, Create_time);
		return true;
	}
	virtual bool unserial(const char *pIn, int Length){return true;}
	virtual bool unserial(IInputStream& Input)
	{
		readuint64(Input, id);
		readuint32(Input, BattlemapId);
		readuint64(Input, PlayerId);
		readuint16(Input, CheckResult);
		readuint64(Input, Create_time);
		return true;
	}
	battlechecklog_record()
	{
		id = 0;
		BattlemapId = 0;
		PlayerId = 0;
		CheckResult = 0;
		Create_time = 0;
	}
	battlechecklog_record(const battlechecklog_record &t)
	{
		id = t.id;
		BattlemapId = t.BattlemapId;
		PlayerId = t.PlayerId;
		CheckResult = t.CheckResult;
		Create_time = t.Create_time;
	}
	battlechecklog_record& operator = (const battlechecklog_record& t)
	{
		id = t.id;
		BattlemapId = t.BattlemapId;
		PlayerId = t.PlayerId;
		CheckResult = t.CheckResult;
		Create_time = t.Create_time;
		return *this;
	}
	virtual ~battlechecklog_record()
	{
	}
};

typedef CRefObject<battlechecklog_record> battlechecklog_recordPtr;


class battleinfo_record : public ISerial
{
public:
	uint64 id;
	uint64 playerid;
	uint32 battlemapid;
	uint16 star;
	uint8 valid;
	uint64 last_modify_date;
	uint64 create_date;

	virtual bool serial(char *pOut, int &Length){return true;}
	virtual bool serial(IOutputStream& Output)
	{
		writeuint64(Output, id);
		writeuint64(Output, playerid);
		writeuint32(Output, battlemapid);
		writeuint16(Output, star);
		writeuint8(Output, valid);
		writeuint64(Output, last_modify_date);
		writeuint64(Output, create_date);
		return true;
	}
	virtual bool unserial(const char *pIn, int Length){return true;}
	virtual bool unserial(IInputStream& Input)
	{
		readuint64(Input, id);
		readuint64(Input, playerid);
		readuint32(Input, battlemapid);
		readuint16(Input, star);
		readuint8(Input, valid);
		readuint64(Input, last_modify_date);
		readuint64(Input, create_date);
		return true;
	}
	battleinfo_record()
	{
		id = 0;
		playerid = 0;
		battlemapid = 0;
		star = 0;
		valid = 0;
		last_modify_date = 0;
		create_date = 0;
	}
	battleinfo_record(const battleinfo_record &t)
	{
		id = t.id;
		playerid = t.playerid;
		battlemapid = t.battlemapid;
		star = t.star;
		valid = t.valid;
		last_modify_date = t.last_modify_date;
		create_date = t.create_date;
	}
	battleinfo_record& operator = (const battleinfo_record& t)
	{
		id = t.id;
		playerid = t.playerid;
		battlemapid = t.battlemapid;
		star = t.star;
		valid = t.valid;
		last_modify_date = t.last_modify_date;
		create_date = t.create_date;
		return *this;
	}
	virtual ~battleinfo_record()
	{
	}
};

typedef CRefObject<battleinfo_record> battleinfo_recordPtr;


class friend_record : public ISerial
{
public:
	uint64 ID;
	uint64 AccountID;
	uint64 FriendAccountID;
	uint64 LastGiveVigorTime;
	uint64 LastAssistTime;

	virtual bool serial(char *pOut, int &Length){return true;}
	virtual bool serial(IOutputStream& Output)
	{
		writeuint64(Output, ID);
		writeuint64(Output, AccountID);
		writeuint64(Output, FriendAccountID);
		writeuint64(Output, LastGiveVigorTime);
		writeuint64(Output, LastAssistTime);
		return true;
	}
	virtual bool unserial(const char *pIn, int Length){return true;}
	virtual bool unserial(IInputStream& Input)
	{
		readuint64(Input, ID);
		readuint64(Input, AccountID);
		readuint64(Input, FriendAccountID);
		readuint64(Input, LastGiveVigorTime);
		readuint64(Input, LastAssistTime);
		return true;
	}
	friend_record()
	{
		ID = 0;
		AccountID = 0;
		FriendAccountID = 0;
		LastGiveVigorTime = 0;
		LastAssistTime = 0;
	}
	friend_record(const friend_record &t)
	{
		ID = t.ID;
		AccountID = t.AccountID;
		FriendAccountID = t.FriendAccountID;
		LastGiveVigorTime = t.LastGiveVigorTime;
		LastAssistTime = t.LastAssistTime;
	}
	friend_record& operator = (const friend_record& t)
	{
		ID = t.ID;
		AccountID = t.AccountID;
		FriendAccountID = t.FriendAccountID;
		LastGiveVigorTime = t.LastGiveVigorTime;
		LastAssistTime = t.LastAssistTime;
		return *this;
	}
	virtual ~friend_record()
	{
	}
};

typedef CRefObject<friend_record> friend_recordPtr;


class friend_battle_award_record : public ISerial
{
public:
	uint64 id;
	uint64 player_id;
	uint32 award_exp;
	uint32 award_silver;
	uint32 award_vigor;
	uint64 create_time;

	virtual bool serial(char *pOut, int &Length){return true;}
	virtual bool serial(IOutputStream& Output)
	{
		writeuint64(Output, id);
		writeuint64(Output, player_id);
		writeuint32(Output, award_exp);
		writeuint32(Output, award_silver);
		writeuint32(Output, award_vigor);
		writeuint64(Output, create_time);
		return true;
	}
	virtual bool unserial(const char *pIn, int Length){return true;}
	virtual bool unserial(IInputStream& Input)
	{
		readuint64(Input, id);
		readuint64(Input, player_id);
		readuint32(Input, award_exp);
		readuint32(Input, award_silver);
		readuint32(Input, award_vigor);
		readuint64(Input, create_time);
		return true;
	}
	friend_battle_award_record()
	{
		id = 0;
		player_id = 0;
		award_exp = 0;
		award_silver = 0;
		award_vigor = 0;
		create_time = 0;
	}
	friend_battle_award_record(const friend_battle_award_record &t)
	{
		id = t.id;
		player_id = t.player_id;
		award_exp = t.award_exp;
		award_silver = t.award_silver;
		award_vigor = t.award_vigor;
		create_time = t.create_time;
	}
	friend_battle_award_record& operator = (const friend_battle_award_record& t)
	{
		id = t.id;
		player_id = t.player_id;
		award_exp = t.award_exp;
		award_silver = t.award_silver;
		award_vigor = t.award_vigor;
		create_time = t.create_time;
		return *this;
	}
	virtual ~friend_battle_award_record()
	{
	}
};

typedef CRefObject<friend_battle_award_record> friend_battle_award_recordPtr;


class item_record : public ISerial
{
public:
	uint64 ID;
	uint64 PlayerID;
	uint32 ItemID;
	uint8 ItemPos;
	uint8 ItemNum;

	virtual bool serial(char *pOut, int &Length){return true;}
	virtual bool serial(IOutputStream& Output)
	{
		writeuint64(Output, ID);
		writeuint64(Output, PlayerID);
		writeuint32(Output, ItemID);
		writeuint8(Output, ItemPos);
		writeuint8(Output, ItemNum);
		return true;
	}
	virtual bool unserial(const char *pIn, int Length){return true;}
	virtual bool unserial(IInputStream& Input)
	{
		readuint64(Input, ID);
		readuint64(Input, PlayerID);
		readuint32(Input, ItemID);
		readuint8(Input, ItemPos);
		readuint8(Input, ItemNum);
		return true;
	}
	item_record()
	{
		ID = 0;
		PlayerID = 0;
		ItemID = 0;
		ItemPos = 0;
		ItemNum = 0;
	}
	item_record(const item_record &t)
	{
		ID = t.ID;
		PlayerID = t.PlayerID;
		ItemID = t.ItemID;
		ItemPos = t.ItemPos;
		ItemNum = t.ItemNum;
	}
	item_record& operator = (const item_record& t)
	{
		ID = t.ID;
		PlayerID = t.PlayerID;
		ItemID = t.ItemID;
		ItemPos = t.ItemPos;
		ItemNum = t.ItemNum;
		return *this;
	}
	virtual ~item_record()
	{
	}
};

typedef CRefObject<item_record> item_recordPtr;


class mounts_record : public ISerial
{
public:
	uint64 ID;
	uint64 PlayerID;
	uint32 MountsID;
	uint16 MountsExp;

	virtual bool serial(char *pOut, int &Length){return true;}
	virtual bool serial(IOutputStream& Output)
	{
		writeuint64(Output, ID);
		writeuint64(Output, PlayerID);
		writeuint32(Output, MountsID);
		writeuint16(Output, MountsExp);
		return true;
	}
	virtual bool unserial(const char *pIn, int Length){return true;}
	virtual bool unserial(IInputStream& Input)
	{
		readuint64(Input, ID);
		readuint64(Input, PlayerID);
		readuint32(Input, MountsID);
		readuint16(Input, MountsExp);
		return true;
	}
	mounts_record()
	{
		ID = 0;
		PlayerID = 0;
		MountsID = 0;
		MountsExp = 0;
	}
	mounts_record(const mounts_record &t)
	{
		ID = t.ID;
		PlayerID = t.PlayerID;
		MountsID = t.MountsID;
		MountsExp = t.MountsExp;
	}
	mounts_record& operator = (const mounts_record& t)
	{
		ID = t.ID;
		PlayerID = t.PlayerID;
		MountsID = t.MountsID;
		MountsExp = t.MountsExp;
		return *this;
	}
	virtual ~mounts_record()
	{
	}
};

typedef CRefObject<mounts_record> mounts_recordPtr;


class pet_record : public ISerial
{
public:
	uint64 ID;
	uint64 PlayerID;
	uint32 PetID;
	uint16 PetExp;

	virtual bool serial(char *pOut, int &Length){return true;}
	virtual bool serial(IOutputStream& Output)
	{
		writeuint64(Output, ID);
		writeuint64(Output, PlayerID);
		writeuint32(Output, PetID);
		writeuint16(Output, PetExp);
		return true;
	}
	virtual bool unserial(const char *pIn, int Length){return true;}
	virtual bool unserial(IInputStream& Input)
	{
		readuint64(Input, ID);
		readuint64(Input, PlayerID);
		readuint32(Input, PetID);
		readuint16(Input, PetExp);
		return true;
	}
	pet_record()
	{
		ID = 0;
		PlayerID = 0;
		PetID = 0;
		PetExp = 0;
	}
	pet_record(const pet_record &t)
	{
		ID = t.ID;
		PlayerID = t.PlayerID;
		PetID = t.PetID;
		PetExp = t.PetExp;
	}
	pet_record& operator = (const pet_record& t)
	{
		ID = t.ID;
		PlayerID = t.PlayerID;
		PetID = t.PetID;
		PetExp = t.PetExp;
		return *this;
	}
	virtual ~pet_record()
	{
	}
};

typedef CRefObject<pet_record> pet_recordPtr;


class player_arena_info_record : public ISerial
{
public:
	uint64 id;
	uint64 player_id;
	uint32 num;
	uint64 date;
	uint32 cd_time;
	uint64 cd_start;

	virtual bool serial(char *pOut, int &Length){return true;}
	virtual bool serial(IOutputStream& Output)
	{
		writeuint64(Output, id);
		writeuint64(Output, player_id);
		writeuint32(Output, num);
		writeuint64(Output, date);
		writeuint32(Output, cd_time);
		writeuint64(Output, cd_start);
		return true;
	}
	virtual bool unserial(const char *pIn, int Length){return true;}
	virtual bool unserial(IInputStream& Input)
	{
		readuint64(Input, id);
		readuint64(Input, player_id);
		readuint32(Input, num);
		readuint64(Input, date);
		readuint32(Input, cd_time);
		readuint64(Input, cd_start);
		return true;
	}
	player_arena_info_record()
	{
		id = 0;
		player_id = 0;
		num = 0;
		date = 0;
		cd_time = 0;
		cd_start = 0;
	}
	player_arena_info_record(const player_arena_info_record &t)
	{
		id = t.id;
		player_id = t.player_id;
		num = t.num;
		date = t.date;
		cd_time = t.cd_time;
		cd_start = t.cd_start;
	}
	player_arena_info_record& operator = (const player_arena_info_record& t)
	{
		id = t.id;
		player_id = t.player_id;
		num = t.num;
		date = t.date;
		cd_time = t.cd_time;
		cd_start = t.cd_start;
		return *this;
	}
	virtual ~player_arena_info_record()
	{
	}
};

typedef CRefObject<player_arena_info_record> player_arena_info_recordPtr;


class relice_record : public ISerial
{
public:
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

	virtual bool serial(char *pOut, int &Length){return true;}
	virtual bool serial(IOutputStream& Output)
	{
		writeuint64(Output, ID);
		writeuint64(Output, PlayerID);
		writeuint32(Output, ReliceID);
		writeuint8(Output, ActiveAttr);
		writeuint16(Output, GemHole1Level);
		writeuint32(Output, GemHole1CurExp);
		writeuint16(Output, GemHole2Level);
		writeuint16(Output, GemHole2CurExp);
		writeuint16(Output, GemHole3Level);
		writeuint16(Output, GemHole3CurExp);
		return true;
	}
	virtual bool unserial(const char *pIn, int Length){return true;}
	virtual bool unserial(IInputStream& Input)
	{
		readuint64(Input, ID);
		readuint64(Input, PlayerID);
		readuint32(Input, ReliceID);
		readuint8(Input, ActiveAttr);
		readuint16(Input, GemHole1Level);
		readuint32(Input, GemHole1CurExp);
		readuint16(Input, GemHole2Level);
		readuint16(Input, GemHole2CurExp);
		readuint16(Input, GemHole3Level);
		readuint16(Input, GemHole3CurExp);
		return true;
	}
	relice_record()
	{
		ID = 0;
		PlayerID = 0;
		ReliceID = 0;
		ActiveAttr = 0;
		GemHole1Level = 0;
		GemHole1CurExp = 0;
		GemHole2Level = 0;
		GemHole2CurExp = 0;
		GemHole3Level = 0;
		GemHole3CurExp = 0;
	}
	relice_record(const relice_record &t)
	{
		ID = t.ID;
		PlayerID = t.PlayerID;
		ReliceID = t.ReliceID;
		ActiveAttr = t.ActiveAttr;
		GemHole1Level = t.GemHole1Level;
		GemHole1CurExp = t.GemHole1CurExp;
		GemHole2Level = t.GemHole2Level;
		GemHole2CurExp = t.GemHole2CurExp;
		GemHole3Level = t.GemHole3Level;
		GemHole3CurExp = t.GemHole3CurExp;
	}
	relice_record& operator = (const relice_record& t)
	{
		ID = t.ID;
		PlayerID = t.PlayerID;
		ReliceID = t.ReliceID;
		ActiveAttr = t.ActiveAttr;
		GemHole1Level = t.GemHole1Level;
		GemHole1CurExp = t.GemHole1CurExp;
		GemHole2Level = t.GemHole2Level;
		GemHole2CurExp = t.GemHole2CurExp;
		GemHole3Level = t.GemHole3Level;
		GemHole3CurExp = t.GemHole3CurExp;
		return *this;
	}
	virtual ~relice_record()
	{
	}
};

typedef CRefObject<relice_record> relice_recordPtr;


class skill_record : public ISerial
{
public:
	uint64 ID;
	uint64 PlayerID;
	uint32 SkillID;
	uint8 SkillType;
	uint8 SkillLevel;
	uint8 SkillPos;
	uint8 valid;
	uint64 create_date;
	uint64 last_modify_date;

	virtual bool serial(char *pOut, int &Length){return true;}
	virtual bool serial(IOutputStream& Output)
	{
		writeuint64(Output, ID);
		writeuint64(Output, PlayerID);
		writeuint32(Output, SkillID);
		writeuint8(Output, SkillType);
		writeuint8(Output, SkillLevel);
		writeuint8(Output, SkillPos);
		writeuint8(Output, valid);
		writeuint64(Output, create_date);
		writeuint64(Output, last_modify_date);
		return true;
	}
	virtual bool unserial(const char *pIn, int Length){return true;}
	virtual bool unserial(IInputStream& Input)
	{
		readuint64(Input, ID);
		readuint64(Input, PlayerID);
		readuint32(Input, SkillID);
		readuint8(Input, SkillType);
		readuint8(Input, SkillLevel);
		readuint8(Input, SkillPos);
		readuint8(Input, valid);
		readuint64(Input, create_date);
		readuint64(Input, last_modify_date);
		return true;
	}
	skill_record()
	{
		ID = 0;
		PlayerID = 0;
		SkillID = 0;
		SkillType = 0;
		SkillLevel = 0;
		SkillPos = 0;
		valid = 0;
		create_date = 0;
		last_modify_date = 0;
	}
	skill_record(const skill_record &t)
	{
		ID = t.ID;
		PlayerID = t.PlayerID;
		SkillID = t.SkillID;
		SkillType = t.SkillType;
		SkillLevel = t.SkillLevel;
		SkillPos = t.SkillPos;
		valid = t.valid;
		create_date = t.create_date;
		last_modify_date = t.last_modify_date;
	}
	skill_record& operator = (const skill_record& t)
	{
		ID = t.ID;
		PlayerID = t.PlayerID;
		SkillID = t.SkillID;
		SkillType = t.SkillType;
		SkillLevel = t.SkillLevel;
		SkillPos = t.SkillPos;
		valid = t.valid;
		create_date = t.create_date;
		last_modify_date = t.last_modify_date;
		return *this;
	}
	virtual ~skill_record()
	{
	}
};

typedef CRefObject<skill_record> skill_recordPtr;

#endif

