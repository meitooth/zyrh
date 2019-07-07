#pragma  once
#include <thread>
#include <string>
#include <mutex>
#include <vector>
class ClientEvent
{
public:
	ClientEvent()
	{

	}
	~ClientEvent()
	{

	}
	virtual void OnConnected()
	{

	}
	virtual void OnDisconnect()
	{

	}
	virtual void OnMessage(char* pdata, int datalen)
	{

	}
	virtual void OnTimer()
	{

	}
};
class CTcpClient
{
public:
	CTcpClient();
	~CTcpClient();
	void Connect(std::string ip, int nPort, ClientEvent*,bool bUseProxy = false,std::string proxyIp = "",int nProxyPort = 1080);
	void Send(std::string str);
	void OnTime();
	void Stop();
	void Close();
	int check_up_socket_4(int sClient, char* str_username, char* str_pwd, char* str_destaddr, int nDestPort);
	int check_up_socket_5(int sClient, char* str_username, char* str_pwd, char* str_destaddr, int nDestPort);
	bool IsConnect()
	{
		return m_bConncet;
	}
private:
	int m_TimerId = -1;
	std::string m_ip;
	int m_nPort = -1;
	bool m_bStop = false;
	bool m_bConncet = false;
	ClientEvent* m_clientEvent = NULL;
	std::vector<std::string> m_SendMsg;
	std::mutex m_mutex;
	SOCKET m_s = INVALID_SOCKET;
	bool m_bUseProxy = false;
	std::string m_ProxyIp;
	int m_nProxyPort = 1080;
private:
	bool Send_i(char* buf, int len);
	bool Recv_i(char* buf, int len);
	void CloseSocket();
};