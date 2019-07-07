#include "usclient.h"
#include "Hub.h"
#include <openssl/sha.h>
static std::mutex g_threadmutex;
UWebSocketsClient::UWebSocketsClient()
{
	m_event = NULL;
	m_timerid = -1;

}
UWebSocketsClient::~UWebSocketsClient()
{
	StopTimeEvent();
	Stop();


}

void UWebSocketsClient::Init()
{
	if (!m_messageThread)
	{
		m_messageThread.reset(new std::thread([=]
		{
			std::weak_ptr<UWebSocketsClient> ClientWPtr = shared_from_this();
			while (1)
			{
				std::shared_ptr<UWebSocketsClient> ClientPtr = ClientWPtr.lock();
				if (ClientPtr)
				{
					m_asyncMutex.lock();
					std::vector<std::string> messagelist = m_messagelist;
					m_messagelist.clear();
					m_asyncMutex.unlock();
					if (messagelist.size() > 0)
					{
						for (int i = 0;i < messagelist.size();i++)
						{
							m_asyncMutex.lock();
							if (m_bStop&&m_event)
							{
								m_event->OnMessage(messagelist[i], this);
							}
							m_asyncMutex.unlock();
						}
					}
					else
					{
						Sleep(1);
					}
				}
				else
				{
					return;
				}

			}

		}
		));
		m_messageThread->detach();
	}
}
void UWebSocketsClient::Start(std::string url, UWebSocketsClientEvent* event)
{
	Stop();
	m_asyncMutex.lock();
	m_url = url;
	m_bStop.reset(new bool(false));
	m_event = event;
	m_asyncMutex.unlock();
	std::shared_ptr<bool> tmpBStop = m_bStop;
	std::thread jthread([=, &tmpBStop]
	{
		std::weak_ptr<UWebSocketsClient> ClientWPtr = shared_from_this();
		std::weak_ptr<bool> bStop = tmpBStop;
		tmpBStop.reset();
		while (1)
		{
			std::shared_ptr<UWebSocketsClient> ClientPtr = ClientWPtr.lock();
			if (ClientPtr&& bStop.expired() == 0)
			{
				uWS::Hub h;
				m_pus = &h;
				h.onMessage([=, &bStop](uWS::WebSocket<uWS::CLIENT> *ws, char *message, size_t length, uWS::OpCode opCode) {
					if (!bStop.expired())
					{
						m_asyncMutex.lock();
						std::string strmsg(message, message + length);
						m_messagelist.push_back(strmsg);
						m_asyncMutex.unlock();
					}
				});
				int length = 0;
				h.onConnection([=, &bStop](uWS::WebSocket<uWS::CLIENT> *ws, uWS::HttpRequest req) {
					if (!bStop.expired() && m_event)
					{
						m_event->onConnection();
					}
				});
				h.onDisconnection([=](uWS::WebSocket<uWS::CLIENT> *ws, int code, char *message, size_t length) {
					if (bStop.lock() && m_event&&code != 1000)
					{
						m_event->onDisconnection();
					}
				});
				h.connect(m_url, this, m_extraHeaders, 3000);
				h.run();
				if (bStop.lock())
				{
					Sleep(300);
				}
			}
			else
			{
				return;
			}


		}

	});
	if (tmpBStop)
	{
		Sleep(1);
	}
	jthread.detach();
}
void WINAPI onTimeEventFunc(UINT wTimerID, UINT msg, DWORD_PTR dwUser, DWORD_PTR dwl, DWORD_PTR dw2)
{
	UWebSocketsClient* p = (UWebSocketsClient*)dwUser;
	p->onTimeEvent();
	return;
}
void UWebSocketsClient::StartTimeEvent(int nMiliseconds)
{
	m_timerid = timeSetEvent(nMiliseconds, 1, (LPTIMECALLBACK)onTimeEventFunc, (DWORD_PTR)this, TIME_PERIODIC);
}
void UWebSocketsClient::StopTimeEvent()
{
	if (m_timerid != -1)
	{
		timeKillEvent(m_timerid);
		m_timerid = -1;
	}
}
void UWebSocketsClient::onTimeEvent()
{
	m_asyncMutex.lock();
	if (m_pus)
	{
		if (m_event)
		{
			m_event->onTimeEvent();
		}
		((uWS::Hub*)m_pus)->getDefaultGroup<uWS::CLIENT>().broadcast(0, 0, uWS::OpCode::PING);
	}
	m_asyncMutex.unlock();
}


void UWebSocketsClient::sendTextMessage(std::string text, OpCode type)
{
	m_asyncMutex.lock();
	if (m_pus)
	{
		((uWS::Hub*)m_pus)->getDefaultGroup<uWS::CLIENT>().broadcast(text.c_str(), text.length(), uWS::OpCode(type));
	}
	m_asyncMutex.unlock();
}

void UWebSocketsClient::Stop()
{
	m_asyncMutex.lock();
	m_bStop.reset();
	if (m_pus)
	{
		((uWS::Hub*)m_pus)->getDefaultGroup<uWS::CLIENT>().close();
		m_pus = NULL;
	}
	m_event = NULL;
	m_asyncMutex.unlock();
}
void UWebSocketsClient::SetExtraHeaders(std::map<std::string, std::string>& extraHeaders)
{
	m_extraHeaders = extraHeaders;
}