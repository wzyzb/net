#include <framework/framepublic.h>
#include <io/locallogstream.h>
#include <file/fileutil.h>


#ifdef WINDOWS
#include <direct.h>
#else
#include <unistd.h>
#endif


CLogPtr g_Log = NULL;

CLocalLogOutputStreamPtr g_LogOut = NULL ;

const char* getServerName()
{
	static char buffer[1024];
	gethostname(buffer,sizeof(buffer));
	return buffer;
}

const char* getProcessName(const char* argv)
{
	static char buffer[1024];
	snprintf(buffer,sizeof(buffer)-1,"%s",argv);
	char* p = strtok(buffer,"/\\");
	char* name = p;
	while ( p != NULL )
	{
		name = p;
		p = strtok(NULL,"/\\");
	}
	return name;
}

ELOGLEVEL getLogLevel(const char* LogLevel)
{
    if ( 0 == strcasecmp(LogLevel,"debug") )
        return LOG_DEBUG;
    if ( 0 == strcasecmp(LogLevel,"info") )
        return LOG_INFO;
    if ( 0 == strcasecmp(LogLevel,"warning") )
        return LOG_WARNING;
    if ( 0 == strcasecmp(LogLevel,"error") )
        return LOG_ERROR;
    return LOG_FATAL;
}

const struct option default_opts[] =
{
	{"config",      required_argument, NULL, 'c'},
	{"machine",      required_argument, NULL, 'm'},
	{"log",         required_argument, NULL, 'l'},
	{"port",        required_argument, NULL, 'p'},
	{"serviceport", required_argument, NULL, 's'},
	{"workpath", required_argument, NULL, 'w'},
	{NULL,0,NULL,0},
};

map<int,string> g_CmdArgs;

int parseCmdArgs(int argc, char *argv[],const struct option* opts)
{
	char** args = NULL;
	if ( argc > 0 )
	{
		int indexsize = argc*sizeof(void*);
		args = (char**)malloc(indexsize + argc*sizeof(char)*1024);
		for ( int i = 0 ; i < argc ; i ++ )
		{
			args[i] = (char*)args+indexsize + i*sizeof(char)*1024;
			snprintf(args[i],sizeof(char)*1024,"%s",argv[i]);
		}
	}
	extern char *optarg;
	extern int optind;
	optind = 0;
	g_CmdArgs.clear();
	int nOpt = 0;
	opterr = 0;
	while( ( nOpt = getopt_long_only(argc, args,"",opts, NULL)) != -1 )
	{
		g_CmdArgs[nOpt] = optarg;
	}// while
	free(args);
	return g_CmdArgs.size();
}

bool getCmdArgs(int nOpt,const char*& Value)
{
	map<int,string>::iterator i = g_CmdArgs.find(nOpt);
	if ( i == g_CmdArgs.end() )
		return false;
	Value = i->second.c_str();
	return true;
}

bool initFrameWorkLog(const char* ProcessName)
{
	if ( g_LogOut != NULL )
	{
		//停止线程，准备删除
		g_LogOut->stop();
	}
	//初始化日志
	g_Log = new CLog(getProcessName(ProcessName),getServerName());
	const char* logdir = NULL;
	if ( !getCmdArgs('l',logdir) )
		logdir = "./log";
	//写本地文件
	g_LogOut = new CLocalLogOutputStream(logdir,getProcessName(ProcessName));
	if ( !g_LogOut->init() )
		return false;
	g_Log->addOutput(g_LogOut.get());
	g_LogOut->start();
	CBinLog::setBinLogPath(logdir);
	CBinLog::setBinLogName(getProcessName(ProcessName));
	return true;
}

typedef struct _t_global_prof
{
	CProfile m_Profile;
	string m_strProfilePath;
	time_t m_ProfileLastModifiedTime;
	string m_strMachineProfPath;
	time_t m_MachineProfLastModifiedTime;
}GLOBAL_PROF;

GLOBAL_PROF g_Profile;

bool reloadFrameWorkProf()
{
	if( !g_Profile.m_Profile.open( g_Profile.m_strProfilePath.c_str() ) )
	{
		log_error("frameworklib|load config file[%s] error",g_Profile.m_strProfilePath.c_str());
		return false;
	}
	CFileUtil::getLastModifiedTime(g_Profile.m_strProfilePath.c_str(),g_Profile.m_ProfileLastModifiedTime);
	log_info("frameworklib|load config file[%s] success , last modified time[%d]",
		g_Profile.m_strProfilePath.c_str(),g_Profile.m_ProfileLastModifiedTime);
	CProfile machineprof;
	//打开机器配置文件
	if ( ! machineprof.open(g_Profile.m_strMachineProfPath.c_str()) )
	{
		log_warning("frameworklib|load machine config file[%s] error",g_Profile.m_strMachineProfPath.c_str());
	}
	else
	{
		CFileUtil::getLastModifiedTime(g_Profile.m_strMachineProfPath.c_str(),g_Profile.m_MachineProfLastModifiedTime);
		g_Profile.m_Profile.setKeyPairs(machineprof);
		log_info("frameworklib|load machine config file[%s] success",g_Profile.m_strMachineProfPath.c_str());
	}
	//根据配置文件，重新设置日志的打印级别
	g_Log->setLogLevel(getLogLevel(g_Profile.m_Profile.getString("app settings","loglevel","debug")));
	g_Log->info("frameworklib|set loglevel[%s]",g_Profile.m_Profile.getString("app settings","loglevel","debug"));

	return true;
}

bool initFrameWorkProf()
{
	//读取命令行参数
	const char* optarg = NULL;
	//根据参数进行设置
	if ( getCmdArgs('c',optarg) )
	{
		g_Profile.m_strProfilePath = optarg;
	}
	else
	{
		g_Profile.m_strProfilePath = "./etc/conf";
	}

	if ( getCmdArgs('m',optarg) )
	{
		g_Profile.m_strMachineProfPath = optarg;
	}
	else
	{
		g_Profile.m_strMachineProfPath = "../../server-config/machine.conf";
	}
	return reloadFrameWorkProf();
}

CProfile* getFrameWorkProf()
{
	return &g_Profile.m_Profile;
}

bool isFrameWorkProfUpdated()
{
	time_t filetm;
	if ( 0 == CFileUtil::getLastModifiedTime(g_Profile.m_strProfilePath.c_str(),filetm) )
	{
		//检查配置文件是否有更新
		if ( filetm != g_Profile.m_ProfileLastModifiedTime )
		{
 			return true;
		}
	}
	if ( 0 == CFileUtil::getLastModifiedTime(g_Profile.m_strMachineProfPath.c_str(),filetm) )
	{
		//检查机器文件是否有更新
		if ( filetm != g_Profile.m_MachineProfLastModifiedTime )
		{
			return true;
		}
	}
	return false;
}

bool initFrameWorkLib(const char* ProcessName)
{
#ifdef WINDOWS
	WSADATA wsadata;
	int errcode = WSAStartup(MAKEWORD(2,2),&wsadata);
	if ( 0 != errcode )
		return false;
#else
	// 屏蔽 pthread 的 SIGPIPE
	sigset_t   signal_mask;
	sigemptyset (&signal_mask);
	sigaddset (&signal_mask, SIGPIPE);
	int rc = pthread_sigmask (SIG_BLOCK, &signal_mask, NULL);
	signal(SIGPIPE,SIG_IGN);
	signal(SIGHUP,SIG_IGN);

	if (rc != 0) 
	{
		return false;
	}
#endif
	const char* workspace = NULL;
        if ( getCmdArgs('w',workspace) )
                chdir(workspace);

	// 初始化线程库
	if( !CThreadFactory::initialize() )
		return false;

	//初始化日志
	if ( !initFrameWorkLog(ProcessName) )
		return false;

	//初始化配置文件
	if ( ! initFrameWorkProf() )
		return false;

	return true;
}


void uninitFrameWorkLib()
{
	if ( g_LogOut != NULL )
		g_LogOut->stop();
	CThreadFactory::finalize();
}
