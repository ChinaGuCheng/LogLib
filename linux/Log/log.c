/*************************************************************************
	> File Name: log.c
	> Author: 
	> Mail: 
	> Created Time: Thu 03 Jan 2019 04:22:52 PM CST
 ************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <time.h>
#include <fcntl.h>
#include "log.h"

char* LogPreFix = NULL;
char* LogDebugName = NULL;
char* LogInfoName = NULL;

//�ļ���Ŀ¼�Ƿ����  ���ڷ���0 �����ڷ���-1
int LOG_FileIsExist(const char* pathname)
{
	return access(pathname, F_OK);
}

//linux��C���Դ����༶Ŀ¼ �����ļ�·��
int LOG_CreateDir_FromFile(const char *filePathAndName)  
{  
	//ȡ�ļ�Ŀ¼
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

int LOG_pwd(char* outdata, int outdata_size)
{
	if(!outdata || outdata_size == 0)
		return -1;
		
	//�����ȡ����Ŀ¼ pass
	getcwd(outdata, outdata_size);
	return 1;
	
/*	
	//���Ҳ��ȡ����Ŀ¼������ pass
	realpath("./", outdata);
	return 0;


	//���µ�Ҳ��ȡ����Ŀ¼ pass
    char tmp_buffer[50]={0};
    char command[] = "pwd";
    FILE *fp = popen(command, "r");
    if(fp != NULL)
    {
        int read_len =  fread(tmp_buffer,1,sizeof(tmp_buffer),fp);

        pclose(fp);
        if(read_len == 0)
        {
            // LOG_TRACE("Can't find ssid in wireless file and exit() ...");
            //          return 0;
            //                  
        }
        else
        {
            int tmp_len = (outdata_size-1) < (read_len-1) ? (outdata_size-1) : (read_len-1);//ɾ�������ַ�
            if(tmp_len == 0)
                tmp_len = 1;    //����Ǹ�Ŀ¼����/
            strncpy(outdata, tmp_buffer, tmp_len);
            outdata[tmp_len] = 0;
            return 1;
        }
    }
    else
    {
        return 0;
    }
*/
}

int getFilePath(const char* fileName, char *out_filePath, int out_filePath_maxlen)
{
	char filePath[256];
	int len;
	
	if(!fileName)
	{
		return -1;
	}
	len = strlen(fileName);
	if(len == 0)
	{
		return -2;
	}
	if(fileName[0] != '/')
	{
		return -3;
	}
	
	if(!out_filePath)
	{
		return -4;
	}
		
	//����������/
	int i = 0;
	for(i = len - 1; i >= 0; i--)
	{
		if(fileName[i] == '/')
		{
			strncpy(filePath, fileName, i);
			filePath[i] = 0;
			break;
		}
	}
	int out_len = strlen(filePath);
	out_len = (out_len > out_filePath_maxlen-1 ? out_filePath_maxlen-1 : out_len);
	strncpy(out_filePath, filePath, out_len);
	out_filePath[out_len] = 0;
	return 0;
}

void log_init(const char *argv_0, const char* logprefix, const char *log_debug_name, const char *log_info_name)
{
	char logprefix_tmpbuf[256];
	char log_debug_name_tmpbuf[256];
	char log_info_name_tmpbuf[256];
	char curPath[256];
	
	if(argv_0[0] == '/')	//����·������
	{
		//����·�������ĳ���
		int ret = getFilePath(argv_0, curPath, sizeof(curPath));
		if( ret != 0)
		{
			printf("getFilePath[%s] error[%d]\n", argv_0, ret);
			return;
		}
	}
	else
	{
		LOG_pwd(curPath, sizeof(curPath));
	}
	
	if(!logprefix)
		logprefix = "GC_LOG";
	if(!log_debug_name)
		log_debug_name = "debug.log";
	if(!log_info_name)
		log_info_name = "info.log";
		
	strcpy(logprefix_tmpbuf, logprefix);
	strcpy(log_debug_name_tmpbuf, log_debug_name);
	strcpy(log_info_name_tmpbuf, log_info_name);

	//�жϾ���·���������·��
	if(log_debug_name_tmpbuf[0] != '/')
	{
		//���·�� ǰ�����pwd��Ŀ¼
		
		char tmp_buf[256];
		strcpy(tmp_buf, log_debug_name_tmpbuf);	//��ʱ��������
		
		strcpy(log_debug_name_tmpbuf, curPath);	// root
		strcat(log_debug_name_tmpbuf, "/");		// root/
		strcat(log_debug_name_tmpbuf, tmp_buf);	// root/xxx.log
	}
	if(log_info_name_tmpbuf[0] != '/')
	{
		//���·�� ǰ�����pwd��Ŀ¼
		char tmp_buf[256];
		strcpy(tmp_buf, log_info_name_tmpbuf);	//��ʱ��������
		
		strcpy(log_info_name_tmpbuf, curPath);	// root
		strcat(log_info_name_tmpbuf, "/");		// root/
		strcat(log_info_name_tmpbuf, tmp_buf);	// root/xxx.log
	}

	LogPreFix = malloc(strlen(logprefix + 1));
	LogDebugName = malloc(strlen(log_debug_name_tmpbuf + 1));
	LogInfoName = malloc(strlen(log_info_name_tmpbuf + 1));
	
	if(!LogPreFix || !LogDebugName || !LogInfoName)
	{
		printf("log malloc error\n");
		exit(0);
	}
	
	strcpy(LogPreFix, logprefix);
	strcpy(LogDebugName, log_debug_name_tmpbuf);
	strcpy(LogInfoName, log_info_name_tmpbuf);
	
	printf("LogDebugName[%s]\n", LogDebugName);
	printf("LogInfoName[%s]\n", LogInfoName);
	
	//Ŀ¼�����ھʹ���
	if(LOG_FileIsExist(LogDebugName) == -1)
	{
		LOG_CreateDir_FromFile(LogDebugName);
	}
	if(LOG_FileIsExist(LogInfoName) == -1)
	{
		LOG_CreateDir_FromFile(LogInfoName);
	}
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

    //�����־��С
	if(getFileSize(logName) > 1024 * 1024 * 2)
	{
		remove(logName);
	}
    int fd = open(logName, O_RDWR | O_CREAT | O_APPEND, 0644);
    if(fd < 0)
    {
        return;
    }
    //дʱ��
    time_t t;
    struct tm *lt;
    time(&t);
    lt = localtime (&t);//תΪʱ��ṹ��
    sprintf (buf, "[%s/%s] [%d/%d/%d %02d:%02d:%02d] ", LogPreFix, LogType, lt->tm_year+1900, lt->tm_mon + 1, lt->tm_mday, lt->tm_hour, lt->tm_min, lt->tm_sec );

    write(fd,buf,strlen(buf));
    //д����
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
