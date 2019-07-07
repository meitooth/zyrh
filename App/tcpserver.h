#pragma  once
#include <thread>
#include <string>
#include <mutex>
#include <vector>
#include <winsock2.h>
#include "serverwork.h"
#include <map>
class CTcpServer
{
public:
	CTcpServer()
	{

	}
	~CTcpServer()
	{

	}
	int Run(int Port);
	void Stop();
private:
	int m_nPort = 0;
	std::map<void*, void*> m_clientList;
	std::mutex m_mutex;
	SOCKET     ListeningSocket = INVALID_SOCKET;

};