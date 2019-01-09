#ifndef _LOG_H
#define _LOG_H



/*************************************************************************
	> File Name: log.h
	> Author: 
	> Mail: 
	> Created Time: Thu 03 Jan 2019 04:22:55 PM CST
 ************************************************************************/

#ifdef __cplusplus
extern "C"{
#endif

//argv[0]	日志前缀	debug日志名	info日志名
void log_init(const char *argv_0, const char* logPrefix, const char *log_debug_name, const char *log_info_name);

typedef enum{
	LOG_NONE=0,
	LOG_INFO=1,
	LOG_DEBUG=2,
	LOG_WARN=3,
	LOG_ERR=4,
	LOG_ALL=255
}LOGLEVEL;


#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
/*
 * Use the syslog output log and include the name and number of rows at the call
 */
#define uwsc_log(priority, fmt...) __uwsc_log(__FILENAME__, __LINE__, priority, fmt)

//#define uwsc_log_debug(fmt...)     uwsc_log(LOG_DEBUG, fmt)
//#define uwsc_log_info(fmt...)      uwsc_log(LOG_INFO, fmt)
//#define uwsc_log_err(fmt...)       uwsc_log(LOG_ERR, fmt)
#define LOG_DEBUG(fmt...)          uwsc_log(LOG_DEBUG, fmt)
#define LOG_INFO(fmt...)          uwsc_log(LOG_INFO, fmt)
#define LOG_ERR(fmt...)           uwsc_log(LOG_ERR, fmt)

void  __uwsc_log(const char *filename, int line, int priority, const char *fmt, ...);




#ifdef __cplusplus
}
#endif


#endif
