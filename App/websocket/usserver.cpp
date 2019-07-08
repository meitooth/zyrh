#include "usserver.h"
#include "Hub.h"
UWebSocketsServer::UWebSocketsServer()
{
	 

}
UWebSocketsServer::~UWebSocketsServer()
{
	 


}

void UWebSocketsServer::Start(int nPort)
{
	m_nPort = nPort;
	std::thread jthread([=]
	{
		std::weak_ptr<UWebSocketsServer> ServerWPtr = shared_from_this();
		uWS::Hub h;
		h.onMessage([=](uWS::WebSocket<uWS::SERVER> *ws, char *message, size_t length, uWS::OpCode opCode) {
			UWebSocketsServerClient* pclient = (UWebSocketsServerClient*)ws->getUserData();
			std::string strmessage(message, length);
			pclient->OnMessage(strmessage);
		});
		h.onConnection([=](uWS::WebSocket<uWS::SERVER> *ws, uWS::HttpRequest req) {
			UWebSocketsServerClient* pclient = new UWebSocketsServerClient;
			ws->setUserData(pclient);
			pclient->m_ws = (void*)ws;
			pclient->onConnection();
		});
		h.onDisconnection([=](uWS::WebSocket<uWS::SERVER> *ws, int code, char *message, size_t length) {
			UWebSocketsServerClient* pclient = (UWebSocketsServerClient*)ws->getUserData();
			pclient->m_ws = NULL;
			pclient->onDisconnection();
			delete pclient;
		});
		h.listen(m_nPort);
		h.run();

	});
	jthread.detach();
}
void UWebSocketsServerClient::SendData(std::string str)
{
	uWS::WebSocket<uWS::SERVER> *ws = (uWS::WebSocket<uWS::SERVER> *)m_ws;
	if (ws)
	{
		ws->send(str.c_str(), str.length(), uWS::TEXT);
	}
}
