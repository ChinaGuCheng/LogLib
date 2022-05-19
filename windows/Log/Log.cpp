#include  "Log.h"
#include <string>  
#include <Windows.h>  
#include <stdio.h>
#include <iostream>
using namespace std;
#include <io.h>		//_access
#include <direct.h>	//_mkdir

#define MAX_LOG_SIZE (1024 * 1024 * 2)  //2M

static char g_pLogPath[1024];
static int g_nLogLevel;
static bool isInit;


/*文件不存在则创建
参数:
  strFullPath:  要创建的文件目录, 如"C:\\111\\222\\333.txt", 支持递归创建
返回:
  成功返回true,失败返回false.
*/
static bool _NotExistThenCreateFile(std::string strFullPath)
{
    if (strFullPath.empty())
    {
        return false;
    }

    //创建目录
    char tmpDirPath[256] = { 0 };
    for (size_t i = 0; i < strFullPath.length(); i++)
    {
        tmpDirPath[i] = strFullPath[i];
        if (tmpDirPath[i] == '\\' || tmpDirPath[i] == '/')
        {
            if (_access(tmpDirPath, 0) == -1)
            {
                int ret = _mkdir(tmpDirPath);
                if (ret == -1)
                {
                    return false;	//创建目录失败
                }
            }
        }
    }

    //创建文件
    if (strFullPath[strFullPath.size() - 1] != '/' && strFullPath[strFullPath.size() - 1] != '\\')
    {
        if (_access(strFullPath.c_str(), 0) == -1)
        {
            FILE* fp = nullptr;
            fopen_s(&fp, strFullPath.c_str(), "wb");
            if (fp)
            {
                fclose(fp);
                return true;
            }
            else
            {
                return false;
            }
        }
        return false;
    }
    return false;
}

static int getFileSize(const char* filename)
{
    FILE* fp = nullptr;
    fopen_s(&fp, filename, "rb");
    if (!fp) return -1;
    fseek(fp, 0L, SEEK_END);
    int size = ftell(fp);
    fclose(fp);
    return size;
}

static string GetTime()
{
    SYSTEMTIME st;
    ::GetLocalTime(&st);
    char szTime[30] = { 0 };
    sprintf_s(szTime, sizeof(szTime) - 1, "%04d-%02d-%02d %02d:%02d:%02d %03d ", st.wYear, st.wMonth, st.wDay, st.wHour, \
        st.wMinute, st.wSecond, st.wMilliseconds);
    return szTime;
}

bool LogInit(const char* szLogFilePath, int __logLevel)
{
    if (!szLogFilePath)
    {
        isInit = false;
        return false;
    }
    _NotExistThenCreateFile(szLogFilePath);
    strcpy_s(g_pLogPath, sizeof(g_pLogPath), szLogFilePath);
    isInit = true;
    g_nLogLevel = __logLevel;
    LOG_WRITE("g_nLogLevel[0x%x][0x%x]", g_nLogLevel, __logLevel);
    return true;
}

int myLog(int nLogLevel, const char* pFileName, const char* pFunName, const long lLine, const char* fmt, ...)
{
    if (!isInit)
    {
        return -1;
    }
    if (nLogLevel != LOG_LEVEL_WRITE && (nLogLevel & g_nLogLevel) == 0)
    {
        return 0;
    }

    int ret = 0;
    va_list args;
    va_start(args, fmt);
    if (getFileSize(g_pLogPath) > MAX_LOG_SIZE)
    {
        remove(g_pLogPath);
    }
    FILE* fp = nullptr;
    fopen_s(&fp, g_pLogPath, "a+");
    if (!fp) return -1;
    //写时间
    string strTime = GetTime();
    fprintf(fp, "%s ", strTime.c_str());

    //写日志等级
    string strLogLevel;
    switch (nLogLevel)
    {
    case LOG_LEVEL_DEBUG:
        strLogLevel = "[debug]";
        break;
    case LOG_LEVEL_WARNING:
        strLogLevel = "[warning]";
        break;
    case LOG_LEVEL_INFO:
        strLogLevel = "[info]";
        break;
    case LOG_LEVEL_WRITE:
        strLogLevel = "[write]";
        break;
    default:
        break;
    }
    fprintf(fp, "%s ", strLogLevel.c_str());
    //写文件名、行号、函数名
    fprintf(fp, "%s:%ld:%s :  ", pFileName, lLine, pFunName);
    //写内容
    ret = vfprintf(fp, fmt, args);
    fwrite("\n", 1, 1, fp);

    //获取完所有参数之后，为了避免发生程序瘫痪，需要将 ap指针关闭，其实这个函数相当于将args设置为NULL   
    va_end(args);
    fflush(fp);
    fclose(fp);
    return ret;
}

Logger::Logger(int nLogLevel, const char* pSourcePath, const char* pFunName, const long lLine)
{
    this->nLogLevel = nLogLevel;
    strcpy_s(this->pSourcePath, sizeof(this->pSourcePath), pSourcePath);
    strcpy_s(this->pFunName, sizeof(this->pFunName), pFunName);
    this->lLine = lLine;
    myLog(this->nLogLevel, this->pSourcePath, this->pFunName, this->lLine, "Function(%s) Enter", this->pFunName);
}
Logger::~Logger()
{
    myLog(this->nLogLevel, this->pSourcePath, this->pFunName, this->lLine, "Function(%s) Quit", this->pFunName);
}