#pragma  once
#include <thread>
#include <string>
#include <mutex>
#include <vector>
#include <winsock2.h>
#include <memory>
class CHttpClient : public std::enable_shared_from_this<CHttpClient>
{
public:
	CHttpClient()
	{

	}
	~CHttpClient()
	{

	}
	virtual void OnConnected(SOCKET  NewConnection);
	virtual void OnDisconnect();
	virtual void OnMessage(char* pdata, int datalen);
	bool Send_i(char* buf, int len);
	bool Recv_i(char* buf, int len);
	void Send(std::string str);
	void CloseSocket();
	bool HandleHead(char* pdata, int len);
	unsigned int  ResquestHttpHeadAndData(unsigned int httpCode, const std::string sHttpData, bool bIsNeedConvertUtf8);
private:
	SOCKET m_s = INVALID_SOCKET;
	bool m_bStop = false;
	bool m_bConncet = false;
	std::vector<std::string> m_SendMsg;
	std::mutex m_mutex;
	std::string m_strHttpPath;
	std::string m_strHttpParam;
	std::string m_sHttpType;
};