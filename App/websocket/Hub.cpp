#include "Hub.h"
#include "HTTPSocket.h"
#include <openssl/sha.h>

namespace uWS {

char *Hub::inflate(char *data, size_t &length, size_t maxPayload) {
    dynamicInflationBuffer.clear();

    inflationStream.next_in = (Bytef *) data;
    inflationStream.avail_in = length;

    int err = 0;
    do {
        inflationStream.next_out = (Bytef *) inflationBuffer;
        inflationStream.avail_out = LARGE_BUFFER_SIZE;
        err = ::inflate(&inflationStream, Z_FINISH);
        if (!inflationStream.avail_in) {
            break;
        }

        dynamicInflationBuffer.append(inflationBuffer, LARGE_BUFFER_SIZE - inflationStream.avail_out);
    } while (err == Z_BUF_ERROR && dynamicInflationBuffer.length() <= maxPayload);

    inflateReset(&inflationStream);

    if ((err != Z_BUF_ERROR && err != Z_OK) || dynamicInflationBuffer.length() > maxPayload) {
        length = 0;
        return nullptr;
    }

    if (dynamicInflationBuffer.length()) {
        dynamicInflationBuffer.append(inflationBuffer, LARGE_BUFFER_SIZE - inflationStream.avail_out);

        length = dynamicInflationBuffer.length();
        return (char *) dynamicInflationBuffer.data();
    }

    length = LARGE_BUFFER_SIZE - inflationStream.avail_out;
    return inflationBuffer;
}

void Hub::onServerAccept(uS::Socket *s) {
    HttpSocket<SERVER> *httpSocket = new HttpSocket<SERVER>(s);
    delete s;

    httpSocket->setState<HttpSocket<SERVER>>();
    httpSocket->start(httpSocket->nodeData->loop, httpSocket, httpSocket->setPoll(UV_READABLE));
    httpSocket->setNoDelay(true);
    Group<SERVER>::from(httpSocket)->addHttpSocket(httpSocket);
    Group<SERVER>::from(httpSocket)->httpConnectionHandler(httpSocket);
}

void Hub::onClientConnection(uS::Socket *s, bool error) {
    HttpSocket<CLIENT> *httpSocket = (HttpSocket<CLIENT> *) s;

    if (error) {
        httpSocket->onEnd(httpSocket);
    } else {
        httpSocket->setState<HttpSocket<CLIENT>>();
        httpSocket->change(httpSocket->nodeData->loop, httpSocket, httpSocket->setPoll(UV_READABLE));
        httpSocket->setNoDelay(true);
        httpSocket->upgrade(nullptr, nullptr, 0, nullptr, 0, nullptr);
    }
}

bool Hub::listen(const char *host, int port, uS::TLS::Context sslContext, int options, Group<SERVER> *eh) {
    if (!eh) {
        eh = (Group<SERVER> *) this;
    }

    if (uS::Node::listen<onServerAccept>(host, port, sslContext, options, (uS::NodeData *) eh, nullptr)) {
        eh->errorHandler(port);
        return false;
    }
    return true;
}

bool Hub::listen(int port, uS::TLS::Context sslContext, int options, Group<SERVER> *eh) {
    return listen(nullptr, port, sslContext, options, eh);
}

uS::Socket *allocateHttpSocket(uS::Socket *s) {
    return (uS::Socket *) new HttpSocket<CLIENT>(s);
}
/**
* @brief URLEncode 对字符串URL编码
*
* @param str 原字符串
* @param strSize 原字符串长度(不包括最后的\0)
* @param result 结果缓冲区的地址
* @param resultSize 结果缓冲区的大小(包括最后的\0)
*
* @return: >0:resultstring 里实际有效的长度
*            0: 解码失败.
*/
int URLEncode(const char* str, const int strSize, char* result, const int resultSize)
{
	int i;
	int j = 0;//for result index
	char ch;

	if ((str == NULL) || (result == NULL) || (strSize <= 0) || (resultSize <= 0)) {
		return 0;
	}

	for (i = 0; (i < strSize) && (j < resultSize); ++i) {
		ch = str[i];
		if (((ch >= 'A') && (ch <= 'Z')) ||
			((ch >= 'a') && (ch <= 'z')) ||
			((ch >= '0') && (ch <= '9'))) {
			result[j++] = ch;
		}
		else if (ch == ' ') {
			result[j++] = '+';
		}
		else if (ch == '.' || ch == '-' || ch == '_' || ch == '*' || ch == ':' || ch == ']' || ch == '[' || ch == '{' || ch == '}' || ch == '?' || ch == '=' || ch == ',') {
			result[j++] = ch;
		}
		else {
			if (j + 3 < resultSize) {
				sprintf(result + j, "%%%02X", (unsigned char)ch);
				j += 3;
			}
			else {
				return 0;
			}
		}
	}

	result[j] = '\0';
	return j;
}
bool parseURI(std::string &uri, bool &secure, std::string &hostname, int &port, std::string &path) {
    port = 80;
    secure = false;
    size_t offset = 5;
    if (!uri.compare(0, 6, "wss://")) {
        port = 443;
        secure = true;
        offset = 6;
    } else if (uri.compare(0, 5, "ws://")) {
        return false;
    }

    if (offset == uri.length()) {
        return false;
    }

    if (uri[offset] == '[') {
        if (++offset == uri.length()) {
            return false;
        }
        size_t endBracket = uri.find(']', offset);
        if (endBracket == std::string::npos) {
            return false;
        }
        hostname = uri.substr(offset, endBracket - offset);
        offset = endBracket + 1;
    } else {
        hostname = uri.substr(offset, uri.find_first_of(":/", offset) - offset);
        offset += hostname.length();
    }

    if (offset == uri.length()) {
        path.clear();
        return true;
    }

    if (uri[offset] == ':') {
        offset++;
        std::string portStr = uri.substr(offset, uri.find('/', offset) - offset);
        if (portStr.length()) {
            try {
                port = stoi(portStr);
            } catch (...) {
                return false;
            }
        } else {
            return false;
        }
        offset += portStr.length();
    }

    if (offset == uri.length()) {
		offset = uri.find('?', 0);
		if (offset  != std::string::npos)
		{
			path = uri.substr(offset, uri.length() - offset);
		}
		else
		{
			path.clear();
		}        
        return true;
    }

    if (uri[offset] == '/') {
        path = uri.substr(++offset);
    }
    return true;
}

void Hub::connect(std::string uri, void *user, std::map<std::string, std::string> extraHeaders, int timeoutMs, Group<CLIENT> *eh) {
    if (!eh) {
        eh = (Group<CLIENT> *) this;
    }

    int port;
    bool secure;
    std::string hostname, path;

    if (!parseURI(uri, secure, hostname, port, path)) {
        eh->errorHandler(user);
    } else {
        HttpSocket<CLIENT> *httpSocket = (HttpSocket<CLIENT> *) uS::Node::connect<allocateHttpSocket, onClientConnection>(hostname.c_str(), port, secure, eh);
        if (httpSocket) {
            // startTimeout occupies the user
            httpSocket->startTimeout<HttpSocket<CLIENT>::onEnd>(timeoutMs);
            httpSocket->httpUser = user;

            std::string randomKey = "x3JJHMbDL1EzLkh9GBhXDw==";
//            for (int i = 0; i < 22; i++) {
//                randomKey[i] = rand() %
//            }
//  			char result[20480];
//  			int resultlen = 20480;
//  			resultlen = URLEncode(path.c_str(), path.length(), result, resultlen);
// 			path = result;
//             httpSocket->httpBuffer = "GET /" + path + " HTTP/1.1\r\n"
//                                      "Upgrade: websocket\r\n"
//                                      "Connection: Upgrade\r\n"
// 									 "Accept-Encoding: gzip, deflate, sdch\r\n"
// 									 "Accept-Encoding: Accept-Language: zh-CN,zh;q=0.8\r\n"
//                                      "Sec-WebSocket-Key: " + randomKey + "\r\n"
//                                      "Host: " + hostname + "\r\n" +
//                                      "Sec-WebSocket-Version: 13\r\nSec-WebSocket-Extensions: permessage-deflate; client_max_window_bits\r\n";

			httpSocket->httpBuffer = "GET /" + path + " HTTP/1.1\r\n"
				"Upgrade: websocket\r\n"
				"Connection: Upgrade\r\n"
				"Sec-WebSocket-Key: " + randomKey + "\r\n"
				"Host: " + hostname + "\r\n" +
				"Sec-WebSocket-Version: 13\r\n";

            for (std::pair<std::string, std::string> header : extraHeaders) {
                httpSocket->httpBuffer += header.first + ": " + header.second + "\r\n";
            }

            httpSocket->httpBuffer += "\r\n";
        } else {
            eh->errorHandler(user);
        }
    }
}

void Hub::upgrade(uv_os_sock_t fd, const char *secKey, SSL *ssl, const char *extensions, size_t extensionsLength, const char *subprotocol, size_t subprotocolLength, Group<SERVER> *serverGroup) {
    if (!serverGroup) {
        serverGroup = &getDefaultGroup<SERVER>();
    }

    uS::Socket s((uS::NodeData *) serverGroup, serverGroup->loop, fd, ssl);
    s.setNoDelay(true);

    // todo: skip httpSocket -> it cannot fail anyways!
    HttpSocket<SERVER> *httpSocket = new HttpSocket<SERVER>(&s);
    httpSocket->setState<HttpSocket<SERVER>>();
    httpSocket->change(httpSocket->nodeData->loop, httpSocket, httpSocket->setPoll(UV_READABLE));
    bool perMessageDeflate;
    httpSocket->upgrade(secKey, extensions, extensionsLength, subprotocol, subprotocolLength, &perMessageDeflate);

    WebSocket<SERVER> *webSocket = new WebSocket<SERVER>(perMessageDeflate, httpSocket);
    delete httpSocket;
    webSocket->setState<WebSocket<SERVER>>();
    webSocket->change(webSocket->nodeData->loop, webSocket, webSocket->setPoll(UV_READABLE));
    serverGroup->addWebSocket(webSocket);
    serverGroup->connectionHandler(webSocket, {});
}

}
