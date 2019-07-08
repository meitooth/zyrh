#pragma  once
#include <thread>
#include <string>
#include <mutex>
#include <vector>
#include <winsock2.h>
#include <memory>
class CServerWork: public std::enable_shared_from_this<CServerWork>
{
public:
	CServerWork()
	{

	}
	~CServerWork()
	{

	}
	virtual void OnConnected(SOCKET  NewConnection);
	virtual void OnDisconnect();
	virtual void OnMessage(char* pdata, int datalen);
	bool Send_i(char* buf, int len);
	bool Recv_i(char* buf, int len);
	void Send(std::string str);
	void CloseSocket();
private:
	SOCKET m_s = INVALID_SOCKET;
	bool m_bStop = false;
	bool m_bConncet = false;
	std::vector<std::string> m_SendMsg;
	std::mutex m_mutex;
public:
	int m_keeplivetime = 0;
};
