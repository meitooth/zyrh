#ifndef LOGGER_H_
#define LOGGER_H_
#include <stdio.h>
#include "log_deffine.h"
#include <vector>
#include <mutex>
 
class Logger
{
public:
	// 
	Logger(int log_level_ = LogLevelDebug,const char * strLogPath = "log",const char* pStrFileName = "file");
	// 
	virtual ~Logger();
public:	
	// 
	void  Write2Caching(int log_level_,const char * strInfo, ...);
	void  Write2Caching(const char * strInfo);
	void DoWriteLog();
	static bool bexit;
private:

	std::string Time2String(time_t time_t_);
	// 
	void GenerateLogName();
	// 
	void CreateLogPath();

	const char* logLevelToString(int l);
private:
	// 
	FILE * m_pFileStream;

	std::string m_strLogPath;

	int m_log_level_;

	std::string m_strCurLogName;

	
	char* log_buf = NULL;
	std::vector<std::string> m_vcStrList;

	int  m_nCurrentDay;
	std::mutex mutex_;

};
#endif

