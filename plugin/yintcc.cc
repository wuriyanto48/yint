#include <napi.h>
#include <array>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <arpa/inet.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>

namespace yint {

typedef unsigned char Byte;

const char* HTTP_PORT = "80";
const char* SECURE_HTTP_PORT = "443";
const size_t BUFFER = 1024;

char* _StringAsCharArr(std::string* str)
{
    return str->empty() ? NULL : &*str->begin();
}

int _GetIP(const char* web_url, char** ip, char** hostname)
{
    hostent* he = gethostbyname(web_url);
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

int _SendReqWriteOut(int sock_FD, char* hostname, std::ostream& out) 
{
    // TODO
    // add redirect to https
    std::ostringstream msg;
    msg << "GET / HTTP/1.1\r\n"
        << "Host: " << hostname << "\r\n"
        << "Accept: text/html\r\n"
        << "Connection: close\r\n"
        << "\r\n\r\n";

    std::string msg_request = msg.str();

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

    return 0;
}

Napi::Value GetIP(const Napi::CallbackInfo& info)
{

    Napi::Env env = info.Env();

    if (info.Length() <= 0)
    {
        Napi::TypeError::New(env, "required one argument").ThrowAsJavaScriptException();
        return env.Null();
    }

    if (!info[0].IsString()) {
        Napi::TypeError::New(env, "argument should be string").ThrowAsJavaScriptException();
        return env.Null();
    }

    std::string url_val = info[0].As<Napi::String>().Utf8Value();

    char* ip = nullptr;
    int res_ip = _GetIP(_StringAsCharArr(&url_val), &ip, NULL);
    if (res_ip < 0)
    {
        Napi::TypeError::New(env, "cannot get ip").ThrowAsJavaScriptException();
        return env.Null();
    }

    Napi::String ret_val = Napi::String::New(env, ip);
    return ret_val;

}

Napi::Value HTTPGet(const Napi::CallbackInfo& info)
{
    Napi::Env env = info.Env();

    if (info.Length() <= 0)
    {
        Napi::TypeError::New(env, "required one argument").ThrowAsJavaScriptException();
        return env.Null();
    }

    if (!info[0].IsString()) {
        Napi::TypeError::New(env, "argument should be string").ThrowAsJavaScriptException();
        return env.Null();
    }

    std::string url_val = info[0].As<Napi::String>().Utf8Value();

    char* ip = nullptr;
    char* hostname = nullptr;
    int res_ip = _GetIP(_StringAsCharArr(&url_val), &ip, &hostname);
    if (res_ip < 0)
    {
        Napi::TypeError::New(env, "cannot get ip").ThrowAsJavaScriptException();
        return env.Null();
    }

    int sock_FD = _CreateSocket(ip, HTTP_PORT);
    if (sock_FD == -1) 
    {
        Napi::TypeError::New(env, "error: creating socket").ThrowAsJavaScriptException();
        return env.Null();
    }

    printf("%s\n", "---------");
    printf("%s\n", ip);
    printf("%s\n", hostname);
    printf("%d\n", sock_FD);

    // write response to file
    std::ofstream out_file;
    out_file.open("out.txt");

    int sent = _SendReqWriteOut(sock_FD, hostname, out_file);
    if (sent == -1) 
    {
        Napi::TypeError::New(env, "error: sending request").ThrowAsJavaScriptException();
        return env.Null();
    }

    //close file
    out_file.close();

    // close socket connection
    close(sock_FD);

    // return something
    Napi::String ret_val = Napi::String::New(env, "fu*k");
    return ret_val;
}

Napi::Object Init(Napi::Env env, Napi::Object exports)
{
    exports.Set(Napi::String::New(env, "getIP"), Napi::Function::New(env, GetIP));
    exports.Set(Napi::String::New(env, "httpGet"), Napi::Function::New(env, HTTPGet));
    return exports;
}

NODE_API_MODULE(yintcc, Init);

}