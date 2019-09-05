#include "Log.h"

CLog::CLog(void)
{
	m_LogLevel = 0;
}

CLog::~CLog(void)
{
	CloseLog();
}

CLog* CLog::GetInstancePtr()
{
	static CLog Log;

	return &Log;
}

bool CLog::StartLog(std::string strPrefix, std::string strLogDir)
{
	if(!CommonFunc::CreateDir(strLogDir))
	{
		return false;
	}

	m_strPrefix = strPrefix;

	m_pLogFile = NULL;
	uint64_t curtime = NowTime;
	tm CurTime = *localtime((time_t*)&curtime);
	char szFileName[512];


	snprintf(szFileName, 512, "%s/%s-%02d%02d%02d-%02d%02d%02d.log",  strLogDir.c_str(), strPrefix.c_str(), CurTime.tm_year, CurTime.tm_mon + 1, CurTime.tm_mday, CurTime.tm_hour, CurTime.tm_min, CurTime.tm_sec);

	m_pLogFile = fopen(szFileName, "w+");

	if(m_pLogFile == NULL)
	{
		return false;
	}

	return true;
}

bool CLog::CloseLog()
{
	if(m_pLogFile == NULL)
	{
		return false;
	}

	fflush(m_pLogFile);

	fclose(m_pLogFile);

	m_pLogFile = NULL;

	return true;
}

void CLog::LogWarnning( char* lpszFormat, ... )
{
	if(m_LogLevel >= Log_Info)
	{
		return ;
	}

	if(m_pLogFile == NULL)
	{
		return ;
	}

	uint64_t curtime = NowTime;
	tm CurTime = *localtime((time_t*)&curtime);

	char szLog[512];
	snprintf(szLog, 512, "[%s][%02d-%02d-%02d %02d:%02d:%02d][%04x] ", m_strPrefix.c_str(), CurTime.tm_year % 100, CurTime.tm_mon + 1, CurTime.tm_mday, CurTime.tm_hour, CurTime.tm_min, CurTime.tm_sec, 1/*0xffff & CommonFunc::GetCurThreadID()*/);

	va_list argList;
	va_start( argList, lpszFormat );
	vsnprintf(szLog + 28 + m_strPrefix.size(), 512 - 28 - m_strPrefix.size(),  lpszFormat, argList);
	va_end( argList );

	strncat(szLog, "\n", 10);

	m_CritSec.Lock();
	fputs(szLog, m_pLogFile);
	fflush(m_pLogFile);
	printf(szLog);
	m_CritSec.Unlock();

	return ;
}

void CLog::LogError( char* lpszFormat, ... )
{
	if(m_LogLevel >= Log_Error)
	{
		return ;
	}

	if(m_pLogFile == NULL)
	{
		return ;
	}
	char szLog[512];
	
	uint64_t curtime = NowTime;
	tm CurTime = *localtime((time_t*)&curtime);

	snprintf(szLog, 512, "[%s][%02d-%02d-%02d %02d:%02d:%02d][%04x] ", m_strPrefix.c_str(), CurTime.tm_year % 100, CurTime.tm_mon + 1, CurTime.tm_mday, CurTime.tm_hour, CurTime.tm_min, CurTime.tm_sec, 1/*0xffff & CommonFunc::GetCurThreadID()*/);

	va_list argList;
	va_start( argList, lpszFormat );
	vsnprintf(szLog + 28 + m_strPrefix.size(), 512 - 28 - m_strPrefix.size(), lpszFormat, argList);
	va_end( argList );

	strncat(szLog, "\n", 10);
	
	m_CritSec.Lock();
	fputs(szLog, m_pLogFile);
	fflush(m_pLogFile);
	printf(szLog);
	m_CritSec.Unlock();

	return ;
}

void CLog::LogInfo( char* lpszFormat, ... )
{
	if(m_LogLevel >= Log_Info)
	{
		return ;
	}

	if(m_pLogFile == NULL)
	{
		return ;
	}

	char szLog[512];

	uint64_t curtime = NowTime;
	tm CurTime = *localtime((time_t*)&curtime);

	snprintf(szLog, 512, "[%s][%02d-%02d-%02d %02d:%02d:%02d][%04x] ", m_strPrefix.c_str(), CurTime.tm_year % 100, CurTime.tm_mon + 1, CurTime.tm_mday, CurTime.tm_hour, CurTime.tm_min, CurTime.tm_sec, 1 /*0xffff & CommonFunc::GetCurThreadID()*/);

	va_list argList;
	va_start( argList, lpszFormat );
	vsnprintf(szLog + 28 + m_strPrefix.size(), 512 - 28 - m_strPrefix.size(),  lpszFormat, argList);
	va_end( argList );

	strncat(szLog, "\n", 10);

	m_CritSec.Lock();
	fputs(szLog, m_pLogFile);
	fflush(m_pLogFile);
	printf(szLog);
	m_CritSec.Unlock();

	return ;
}

void CLog::SetLogLevel( int Level )
{
	m_LogLevel = Level;

	return ;
}
