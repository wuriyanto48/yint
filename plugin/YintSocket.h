#ifndef HEADER_YINT_SOCKET_H
#define HEADER_YINT_SOCKET_H

#include <string>

namespace yint {

typedef unsigned char Byte;

static const char* HTTP_PORT = "80";
static const char* SECURE_HTTP_PORT = "443";
static const size_t BUFFER = 1024;

enum Method: uint32_t { GET, POST, PUT, DELETE };

char* _StringAsCharArr(std::string* str);

int _ExtractURL(const char* web_url, const char** ip, const char** hostname, char** path);

int _CreateSocket(const char* ip, const char* port);

int _CloseSocket(const int sock_FD);

int _SendReqWriteOut(int sock_FD, Method method, const char* path, const char* hostname, std::ostream& out) ;

}

#endif