#pragma once
#include "Logger.h"
#include <thread>
#include <memory>
#include <vector>
class CLogManager
{
private:
	CLogManager();
	~CLogManager();
	static CLogManager* m_gLogManager;
	
	std::vector<Logger*> m_loggerList;
	std::shared_ptr<std::thread> thread;
	bool bclose;
public:
	static CLogManager* GetInstance();
    void AddLog(Logger* pLogger);
	void DoWriteLog();
	void StartWriteLogThread();

};
