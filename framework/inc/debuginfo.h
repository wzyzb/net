#ifndef _DEBUGINFO_H_
#define _DEBUGINFO_H_

#include <framework/framepublic.h>


#ifdef _DEBUGFRAMEWORK
#define DEBUG_LOG log_debug
#define DEBUG_BLOG CBinLog::writeBinLog
#else
#define DEBUG_LOG
#define DEBUG_BLOG
#endif

#endif
