#pragma once  

//#ifdef _DEBUG
#define WRITE_LOG_ENABLE        //启用日志打印
//#endif

#include <string>  
#include <Windows.h>  
#include <stdio.h>

enum ENUM_LOG_LEVEL
{
	LOG_LEVEL_NOLOG = 0,
	LOG_LEVEL_WARNING = 1 << 0,
	LOG_LEVEL_DEBUG = 1 << 1,
	LOG_LEVEL_INFO = 1 << 2,
	LOG_LEVEL_ALL = 0xFF,
	LOG_LEVEL_WRITE = 0xFF + 1
};

bool LogInit(const char* szLogFilePath, /* enum ENUM_LOG_LEVEL */ int __logLevel);
int  myLog(int nLogLevel, const char* pSourcePath, const char* pFunName, const long lLine, const char* fmt, ...);

#ifdef WRITE_LOG_ENABLE
#ifdef _WIN32
#define __FILENAME__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#else
#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#endif
#define LOG_WRITE(format, ...)    myLog(LOG_LEVEL_WRITE, __FILENAME__, __FUNCTION__, __LINE__, format, ##__VA_ARGS__)
#define LOG_DEBUG(format, ...)    myLog(LOG_LEVEL_DEBUG, __FILENAME__, __FUNCTION__, __LINE__, format, ##__VA_ARGS__)
#define LOG_WARNING(format, ...)    myLog(LOG_LEVEL_WARNING, __FILENAME__, __FUNCTION__, __LINE__, format, ##__VA_ARGS__)
#define LOG_INFO(format, ...)    myLog(LOG_LEVEL_INFO, __FILENAME__, __FUNCTION__, __LINE__, format, ##__VA_ARGS__)

class Logger
{
public:
	Logger(int nLogLevel, const char* pSourcePath, const char* pFunName, const long lLine);
	~Logger();
private:
	int nLogLevel;
	char pSourcePath[256];
	char pFunName[256];
	long lLine;
};
#define LOGGER  Logger ____logger____1__FUNCTION__2__LINE__(LOG_LEVEL_DEBUG, __FILENAME__, __FUNCTION__, __LINE__)

#else 
#define LOG_WRITE(format, ...)  
#define LOG_DEBUG(format, ...)    
#define LOG_WARNING(format, ...)    
#define LOG_INFO(format, ...)    

#endif 