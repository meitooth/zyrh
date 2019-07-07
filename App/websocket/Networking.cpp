#include "Networking.h"
//#include <openssl/opensslv.h>
#if OPENSSL_VERSION_NUMBER < 0x10100000L
//#define SSL_CTX_up_ref(x) x->references++
//#define SSL_up_ref(x) x->references++
#endif
namespace uS {

namespace TLS {

Context::Context(const Context &other)
{
    if (other.context) {
        context = other.context;
        //SSL_CTX_up_ref(context);
    }
}

Context &Context::operator=(const Context &other) {
    if (other.context) {
        context = other.context;
       // SSL_CTX_up_ref(context);
    }
    return *this;
}

Context::~Context()
{
    if (context) {
        //SSL_CTX_free(context);
    }
}

struct Init {
    Init() {
		//SSL_library_init();
	}
    ~Init() {/*EVP_cleanup();*/}
} init;

Context createContext(std::string certChainFileName, std::string keyFileName, std::string keyFilePassword)
{
//     Context context(SSL_CTX_new(SSLv23_server_method()));
//     if (!context.context) {
//         return nullptr;
//     }
// 
//     if (keyFilePassword.length()) {
//         context.password.reset(new std::string(keyFilePassword));
//         SSL_CTX_set_default_passwd_cb_userdata(context.context, context.password.get());
//         SSL_CTX_set_default_passwd_cb(context.context, Context::passwordCallback);
//     }
// 
//     SSL_CTX_set_options(context.context, SSL_OP_NO_SSLv3);
// 
//     if (SSL_CTX_use_certificate_chain_file(context.context, certChainFileName.c_str()) != 1) {
//         return nullptr;
//     } else if (SSL_CTX_use_PrivateKey_file(context.context, keyFileName.c_str(), SSL_FILETYPE_PEM) != 1) {
//         return nullptr;
//     }

    return NULL;
}

}

#ifndef _WIN32
struct Init {
    Init() {signal(SIGPIPE, SIG_IGN);}
} init;
#endif

#ifdef _WIN32


struct WindowsInit {
    WSADATA wsaData;
    WindowsInit() {WSAStartup(MAKEWORD(2, 2), &wsaData);}
    ~WindowsInit() {WSACleanup();}
} windowsInit;

#endif

}
