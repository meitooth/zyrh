#include "LogManager.h"
#include <windows.h>
CLogManager* CLogManager::m_gLogManager = NULL;

CLogManager::CLogManager()
{
	bclose = false;

}
CLogManager::~CLogManager()
{
	bclose = true;
	if (thread)
	{
		thread->join();
		thread.reset();
	}

}

void CLogManager::AddLog(Logger* pLogger)
{
	m_loggerList.push_back(pLogger);
}

CLogManager* CLogManager::GetInstance()
{
	if (m_gLogManager == NULL)
	{
		m_gLogManager = new CLogManager();
		m_gLogManager->StartWriteLogThread();
	}
	return m_gLogManager;
}
void CLogManager::StartWriteLogThread()
{
	if (!thread)
	{
		thread.reset(new std::thread([=]
		{
			DoWriteLog();
		}));
	}

}
void CLogManager::DoWriteLog()
{
	while (!bclose&&!Logger::bexit)
	{
		Sleep(10);
		std::vector<Logger*> vcLogList;
		vcLogList = m_loggerList;
		for (int i = 0;i<vcLogList.size();i++)
		{
			if (vcLogList[i])
			{
				vcLogList[i]->DoWriteLog();
			}
		}
	}

}
