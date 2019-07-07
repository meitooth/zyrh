#pragma once
#include <string>
#include <mutex>
#include <map>
#include <vector>
#ifdef   UWBESOCKET_EXPORT
#define UWBESOCKET_EXPORTIMPL 
#else
#define UWBESOCKET_EXPORTIMPL 
#endif 
class UWebSocketsClientEvent;
enum OpCode : unsigned char {
	TEXT = 1,
	BINARY = 2,
	CLOSE = 8,
	PING = 9,
	PONG = 10
};
class  UWebSocketsClient:public std::enable_shared_from_this<UWebSocketsClient>
{   
public:
	UWebSocketsClient();
	~UWebSocketsClient();
	void sendTextMessage(std::string, OpCode);
	void Start(std::string url, UWebSocketsClientEvent* event);
	void StartTimeEvent(int nMiliseconds);
	void StopTimeEvent();
	void Stop();
	void onTimeEvent();
	void Init();
	void SetExtraHeaders(std::map<std::string, std::string>& extraHeaders);
private:	
	std::string m_url;
	void* m_pus = NULL;
	UWebSocketsClientEvent* m_event;
	std::shared_ptr<bool> m_bStop;
	int m_timerid;
	std::recursive_mutex m_asyncMutex;
	std::map<std::string, std::string> m_extraHeaders;
	std::vector<std::string> m_messagelist;
	std::shared_ptr<std::thread> m_messageThread;
};
class UWebSocketsClientEvent
{
public:
	UWebSocketsClientEvent()
	{

	}
	~UWebSocketsClientEvent()
	{

	}
	virtual void onConnection() = 0;
	virtual void onDisconnection() = 0;
	virtual void OnMessage(std::string message, UWebSocketsClient* client) = 0;
	virtual void onTimeEvent() = 0;
};