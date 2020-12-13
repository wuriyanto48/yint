#include <array>
#include <iostream>
#include <sstream>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <vector>
#include "YintSocket.h"

namespace yint {

std::ostream& operator<<(std::ostream& out, Method method)
{
    switch (method)
    {
        case Method::GET    : out << "GET"; break;
        case Method::POST   : out << "POST"; break;
        case Method::PUT    : out << "PUT"; break;
        case Method::DELETE : out << "DELETE"; break;
        default             : out.setstate(std::ios_base::failbit);
    }
    return out;
}

char* _StringAsCharArr(std::string* str)
{
    return str->empty() ? NULL : &*str->begin();
}

int _ExtractURL(const char* web_url, const char** ip, const char** hostname, char** path)
{

    std::string web_url_str(web_url);
    std::stringstream web_url_str_stream(web_url_str);

    std::vector<std::string> web_url_vec;

    std::string web_url_part;
    while (std::getline(web_url_str_stream, web_url_part, '/'))
    {
        web_url_vec.push_back(web_url_part);
    }

    for(size_t i = 0; i < web_url_vec.size(); i++)
    {
        std::cout << web_url_vec[i] << std::endl;
    }

    std::string path_("/");
    if (web_url_vec.size() > 1)
    {
        path_ = path_.append(web_url_vec[1]);
    }

    // copy path
    strcpy(*path, path_.c_str());

    hostent* he = gethostbyname(web_url_vec[0].c_str());
    if (he == NULL)
    {
        return -1;
    }

    *hostname = he->h_name;
    *ip = inet_ntoa(*(struct in_addr*)he->h_addr_list[0]);

    return 0;
}

int _CreateSocket(const char* ip, const char* port)
{

    addrinfo addr_i, *addr_i_p;
    memset(&addr_i, 0, sizeof(addr_i));

    addr_i.ai_family = AF_INET;
    addr_i.ai_socktype = SOCK_STREAM;
    addr_i.ai_flags = AI_PASSIVE;

    int addr_info_res = getaddrinfo(ip, port, &addr_i, &addr_i_p);
    if (addr_info_res != 0)
    {
        return -1;
    }

    if (addr_i_p == NULL)
    {
        return -1;
    }

    // create new socket
    int sock_FD = socket(addr_i_p->ai_family, addr_i_p->ai_socktype, addr_i_p->ai_protocol);
    if (sock_FD == -1) 
    {
        return -1;
    }

    // estabilish new TCP connection
    int connect_R = connect(sock_FD, addr_i_p->ai_addr, addr_i_p->ai_addrlen);
    if (connect_R == -1) 
    {
        close(sock_FD);
        return -1;
    }

    freeaddrinfo(addr_i_p);

    return sock_FD;
}

int _CloseSocket(const int sock_FD)
{
    return close(sock_FD);
}

int _ReadHTTPCode(std::iostream& resp, uint32_t* http_code)
{
    std::string line;
    std::getline(resp, line);
    std::cout << "---------------" << std::endl;
    std::cout << line << std::endl;
    return 0;
}

int _SendReqWriteOut(int sock_FD, Method method, const char* path, const char* hostname, std::ostream& out) 
{
    // TODO
    // add redirect to https

    std::ostringstream msg;
    msg << method << " " << path << " HTTP/1.1\r\n"
        << "Host: " << hostname << "\r\n"
        << "Accept: text/html\r\n"
        << "Connection: close\r\n"
        << "\r\n\r\n";

    std::string msg_request = msg.str();
    std::cout << "----- request -----" << std::endl;
    std::cout << msg_request << std::endl;

    int sent = send(sock_FD, msg_request.c_str(), msg_request.length(), 0);
    if (sent == -1) 
    {
        return -1;
    }

    std::array<Byte, BUFFER> rec;
    while(recv(sock_FD, rec.data(), rec.size(), 0) > 0)
    {
        out << rec.data();
    }

    //TODO
    //_ReadHTTPCode(out, NULL);

    return 0;
}

}