#include "db_classes.h"
#include <db/connection.h>
#ifndef log_info
#define log_info printf
#endif
void sync_db(CDateTime& BeginTime, CConnection& Conn)
{
		CDaoTransaction trans(TRANSACTION_CACHE_ONLY);
		try
		{
				account tmp_account;
				tmp_account.prepare("last_modified_date >= ? ");
				tmp_account.setDateTime(1, BeginTime);
				tmp_account.find();
				log_info("account - (%d)", tmp_account.getRowCount());
				Conn.begin();
				trans.begin();
				while ( tmp_account.next() )
				{
				if ( ! tmp_account.updateDB(*&Conn) )
						tmp_account.insertDB(*&Conn);
				}
				trans.commit();
				Conn.commit();

				arena tmp_arena;
				tmp_arena.prepare("last_modified_date >= ? ");
				tmp_arena.setDateTime(1, BeginTime);
				tmp_arena.find();
				log_info("arena - (%d)", tmp_arena.getRowCount());
				Conn.begin();
				trans.begin();
				while ( tmp_arena.next() )
				{
				if ( ! tmp_arena.updateDB(*&Conn) )
						tmp_arena.insertDB(*&Conn);
				}
				trans.commit();
				Conn.commit();

				avatar tmp_avatar;
				tmp_avatar.prepare("last_modified_date >= ? ");
				tmp_avatar.setDateTime(1, BeginTime);
				tmp_avatar.find();
				log_info("avatar - (%d)", tmp_avatar.getRowCount());
				Conn.begin();
				trans.begin();
				while ( tmp_avatar.next() )
				{
				if ( ! tmp_avatar.updateDB(*&Conn) )
						tmp_avatar.insertDB(*&Conn);
				}
				trans.commit();
				Conn.commit();

				battlechecklog tmp_battlechecklog;
				tmp_battlechecklog.prepare("last_modified_date >= ? ");
				tmp_battlechecklog.setDateTime(1, BeginTime);
				tmp_battlechecklog.find();
				log_info("battlechecklog - (%d)", tmp_battlechecklog.getRowCount());
				Conn.begin();
				trans.begin();
				while ( tmp_battlechecklog.next() )
				{
				if ( ! tmp_battlechecklog.updateDB(*&Conn) )
						tmp_battlechecklog.insertDB(*&Conn);
				}
				trans.commit();
				Conn.commit();

				battleinfo tmp_battleinfo;
				tmp_battleinfo.prepare("last_modified_date >= ? ");
				tmp_battleinfo.setDateTime(1, BeginTime);
				tmp_battleinfo.find();
				log_info("battleinfo - (%d)", tmp_battleinfo.getRowCount());
				Conn.begin();
				trans.begin();
				while ( tmp_battleinfo.next() )
				{
				if ( ! tmp_battleinfo.updateDB(*&Conn) )
						tmp_battleinfo.insertDB(*&Conn);
				}
				trans.commit();
				Conn.commit();

				friend tmp_friend;
				tmp_friend.prepare("last_modified_date >= ? ");
				tmp_friend.setDateTime(1, BeginTime);
				tmp_friend.find();
				log_info("friend - (%d)", tmp_friend.getRowCount());
				Conn.begin();
				trans.begin();
				while ( tmp_friend.next() )
				{
				if ( ! tmp_friend.updateDB(*&Conn) )
						tmp_friend.insertDB(*&Conn);
				}
				trans.commit();
				Conn.commit();

				friend_battle_award tmp_friend_battle_award;
				tmp_friend_battle_award.prepare("last_modified_date >= ? ");
				tmp_friend_battle_award.setDateTime(1, BeginTime);
				tmp_friend_battle_award.find();
				log_info("friend_battle_award - (%d)", tmp_friend_battle_award.getRowCount());
				Conn.begin();
				trans.begin();
				while ( tmp_friend_battle_award.next() )
				{
				if ( ! tmp_friend_battle_award.updateDB(*&Conn) )
						tmp_friend_battle_award.insertDB(*&Conn);
				}
				trans.commit();
				Conn.commit();

				item tmp_item;
				tmp_item.prepare("last_modified_date >= ? ");
				tmp_item.setDateTime(1, BeginTime);
				tmp_item.find();
				log_info("item - (%d)", tmp_item.getRowCount());
				Conn.begin();
				trans.begin();
				while ( tmp_item.next() )
				{
				if ( ! tmp_item.updateDB(*&Conn) )
						tmp_item.insertDB(*&Conn);
				}
				trans.commit();
				Conn.commit();

				mounts tmp_mounts;
				tmp_mounts.prepare("last_modified_date >= ? ");
				tmp_mounts.setDateTime(1, BeginTime);
				tmp_mounts.find();
				log_info("mounts - (%d)", tmp_mounts.getRowCount());
				Conn.begin();
				trans.begin();
				while ( tmp_mounts.next() )
				{
				if ( ! tmp_mounts.updateDB(*&Conn) )
						tmp_mounts.insertDB(*&Conn);
				}
				trans.commit();
				Conn.commit();

				pet tmp_pet;
				tmp_pet.prepare("last_modified_date >= ? ");
				tmp_pet.setDateTime(1, BeginTime);
				tmp_pet.find();
				log_info("pet - (%d)", tmp_pet.getRowCount());
				Conn.begin();
				trans.begin();
				while ( tmp_pet.next() )
				{
				if ( ! tmp_pet.updateDB(*&Conn) )
						tmp_pet.insertDB(*&Conn);
				}
				trans.commit();
				Conn.commit();

				player_arena_info tmp_player_arena_info;
				tmp_player_arena_info.prepare("last_modified_date >= ? ");
				tmp_player_arena_info.setDateTime(1, BeginTime);
				tmp_player_arena_info.find();
				log_info("player_arena_info - (%d)", tmp_player_arena_info.getRowCount());
				Conn.begin();
				trans.begin();
				while ( tmp_player_arena_info.next() )
				{
				if ( ! tmp_player_arena_info.updateDB(*&Conn) )
						tmp_player_arena_info.insertDB(*&Conn);
				}
				trans.commit();
				Conn.commit();

				relice tmp_relice;
				tmp_relice.prepare("last_modified_date >= ? ");
				tmp_relice.setDateTime(1, BeginTime);
				tmp_relice.find();
				log_info("relice - (%d)", tmp_relice.getRowCount());
				Conn.begin();
				trans.begin();
				while ( tmp_relice.next() )
				{
				if ( ! tmp_relice.updateDB(*&Conn) )
						tmp_relice.insertDB(*&Conn);
				}
				trans.commit();
				Conn.commit();

				skill tmp_skill;
				tmp_skill.prepare("last_modified_date >= ? ");
				tmp_skill.setDateTime(1, BeginTime);
				tmp_skill.find();
				log_info("skill - (%d)", tmp_skill.getRowCount());
				Conn.begin();
				trans.begin();
				while ( tmp_skill.next() )
				{
				if ( ! tmp_skill.updateDB(*&Conn) )
						tmp_skill.insertDB(*&Conn);
				}
				trans.commit();
				Conn.commit();

		}
		catch(CSQLException& sqle)
		{
			Conn.rollback();
		log_info("sqlexecption:%s\n", sqle.what());
		trans.rollback();
		}
		catch(CCException& e)
		{
			Conn.rollback();
		log_info("execption:%s\n", e.what());
		trans.rollback();
		}
		catch(...)
		{
			Conn.rollback();
		log_info("unknown execption\n");
		trans.rollback();
		}
}

void checkdb()
{
		CDaoTransaction trans(TRANSACTION_CACHE_ONLY);
		try
		{
				trans.begin();

				account tmp_account;
				tmp_account.prepare("");
				tmp_account.find();
				log_info("account - (%d)", tmp_account.getRowCount());
				arena tmp_arena;
				tmp_arena.prepare("");
				tmp_arena.find();
				log_info("arena - (%d)", tmp_arena.getRowCount());
				avatar tmp_avatar;
				tmp_avatar.prepare("");
				tmp_avatar.find();
				log_info("avatar - (%d)", tmp_avatar.getRowCount());
				battlechecklog tmp_battlechecklog;
				tmp_battlechecklog.prepare("");
				tmp_battlechecklog.find();
				log_info("battlechecklog - (%d)", tmp_battlechecklog.getRowCount());
				battleinfo tmp_battleinfo;
				tmp_battleinfo.prepare("");
				tmp_battleinfo.find();
				log_info("battleinfo - (%d)", tmp_battleinfo.getRowCount());
				friend tmp_friend;
				tmp_friend.prepare("");
				tmp_friend.find();
				log_info("friend - (%d)", tmp_friend.getRowCount());
				friend_battle_award tmp_friend_battle_award;
				tmp_friend_battle_award.prepare("");
				tmp_friend_battle_award.find();
				log_info("friend_battle_award - (%d)", tmp_friend_battle_award.getRowCount());
				item tmp_item;
				tmp_item.prepare("");
				tmp_item.find();
				log_info("item - (%d)", tmp_item.getRowCount());
				mounts tmp_mounts;
				tmp_mounts.prepare("");
				tmp_mounts.find();
				log_info("mounts - (%d)", tmp_mounts.getRowCount());
				pet tmp_pet;
				tmp_pet.prepare("");
				tmp_pet.find();
				log_info("pet - (%d)", tmp_pet.getRowCount());
				player_arena_info tmp_player_arena_info;
				tmp_player_arena_info.prepare("");
				tmp_player_arena_info.find();
				log_info("player_arena_info - (%d)", tmp_player_arena_info.getRowCount());
				relice tmp_relice;
				tmp_relice.prepare("");
				tmp_relice.find();
				log_info("relice - (%d)", tmp_relice.getRowCount());
				skill tmp_skill;
				tmp_skill.prepare("");
				tmp_skill.find();
				log_info("skill - (%d)", tmp_skill.getRowCount());
		trans.commit();
		}
		catch(CSQLException& sqle)
		{
		log_info("sqlexecption:%s\n", sqle.what());
		trans.rollback();
		}
		catch(CCException& e)
		{
		log_info("execption:%s\n", e.what());
		trans.rollback();
		}
		catch(...)
		{
		log_info("unknown execption\n");
		trans.rollback();
		}
}

