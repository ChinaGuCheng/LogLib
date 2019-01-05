/*************************************************************************
	> File Name: log.c
	> Author: 
	> Mail: 
	> Created Time: Thu 03 Jan 2019 04:22:52 PM CST
 ************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <time.h>
#include <fcntl.h>
#include "log.h"

const char* LogPreFix = NULL;
const char* LogDebugName = NULL;
const char* LogInfoName = NULL;

//文件或目录是否存在  存在返回0 不存在返回-1
int FileIsExist(const char* pathname)
{
	return access(pathname, F_OK);
}

//linux下C语言创建多级目录 传入文件路径
int CreateDir_FromFile(const char *filePathAndName)  
{  
	//取文件目录
	char sPathName[128] = { 0 };
	int len = strlen(filePathAndName);
	int i;
	for(i=len-1; i>=0; i--)
	{
		if(filePathAndName[i] == '/')
		{
			strncpy(sPathName, filePathAndName, i);
			sPathName[i] = 0;
			break;
		}
	}
	if(strlen(sPathName) == 0)
	{
		return 0;
	}

	char   DirName[256];  
	strcpy(DirName,   sPathName);  
	len   =   strlen(DirName);  
	if(DirName[len-1]!='/')  
		strcat(DirName,   "/");  

	len   =   strlen(DirName);  

	for(i=1;   i<len;   i++)  
	{  
		if(DirName[i]=='/')  
		{  
			DirName[i]   =   0;  
			if(  access(DirName,   F_OK)!=0   )  
			{  
				if(mkdir(DirName,   0755)==-1)  
				{   
					perror("mkdir   error");   
					return   -1;   
				}  
			}  
			DirName[i]   =   '/';  
		}  
	}  

	return   0;  
}

void log_init(const char* logprefix, const char *log_debug_name, const char *log_info_name)
{
	if(!logprefix)
		logprefix = "notLogPreFix";
	if(!log_debug_name)
		log_debug_name = "not_debug_name.log";
	if(!log_info_name)
		log_info_name = "not_info_name.log";
	
	//目录不存在就创建
	if(FileIsExist(log_debug_name) == -1)
	{
		CreateDir_FromFile(log_debug_name);
	}
	if(FileIsExist(log_info_name) == -1)
	{
		CreateDir_FromFile(log_info_name);
	}

	
	LogPreFix = logprefix;
	LogDebugName = log_debug_name;
	LogInfoName = log_info_name;
}


int getFileSize(const char* fileName)
{
	if(!fileName)return 0;
	struct stat statbuf; 
  	stat(fileName,&statbuf); 
  	int size=statbuf.st_size; 
  	return size;
}

void WriteLog(int priority, const char *data)
{
    char buf[512];
    char logName[64];
    char LogType[10];

    switch(priority)
    {
    	case LOG_NONE:
    		strcpy(LogType, "NONE");
    		strcpy(logName, "NONE.log");
    		break;
    	case LOG_INFO:
    		strcpy(LogType, "INFO");
    		strcpy(logName, LogInfoName);
    		break;
    	case LOG_DEBUG:
    		strcpy(LogType, "DEBUG");
    		strcpy(logName, LogDebugName);
    		break;
    	case LOG_WARN:
    		strcpy(LogType, "WARNING");
    		strcpy(logName, "WARNING.log");
    		break;
    	case LOG_ERR:
    		strcpy(LogType, "ERROR");
    		strcpy(logName, "ERROR.log");
    		break;
    	case LOG_ALL:
    		strcpy(LogType, "ALL");
    		strcpy(logName, "ALL.log");
    		break;
    	default:
    		return;
    }

    //检查日志大小
	if(getFileSize(logName) > 1024 * 1024 * 2)
	{
		remove(logName);
	}
    int fd = open(logName, O_RDWR | O_CREAT | O_APPEND, 0777);
    if(fd < 0)
    {
        return;
    }
    //写时间
    time_t t;
    struct tm *lt;
    time(&t);
    lt = localtime (&t);//转为时间结构。
    sprintf (buf, "[%s/%s] [%d/%d/%d %02d:%02d:%02d] ", LogPreFix, LogType, lt->tm_year+1900, lt->tm_mon + 1, lt->tm_mday, lt->tm_hour, lt->tm_min, lt->tm_sec );

    write(fd,buf,strlen(buf));
    //写内容
    write(fd,data,strlen(data));
    write(fd,"\n",1);
    close(fd);
}

void __uwsc_log(const char *filename, int line, int priority, const char *fmt, ...)
{
    va_list ap;
    static char buf[512];

    snprintf(buf, sizeof(buf), "(%s:%d) ", filename, line);
    
    va_start(ap, fmt);
    vsnprintf(buf + strlen(buf), sizeof(buf) - strlen(buf), fmt, ap);
    va_end(ap);

    if (priority == LOG_ERR && errno > 0) {
        snprintf(buf + strlen(buf), sizeof(buf) - strlen(buf), ":%s", strerror(errno));
        errno = 0;
    }

	WriteLog(priority, buf);
	//printf("%s\n", buf);
}
