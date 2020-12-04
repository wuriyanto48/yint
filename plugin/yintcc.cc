#include <napi.h>
#include <stdio.h>
#include <iostream>
#include <arpa/inet.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>

namespace yint {

const int PORT = 80;

char* _StringAsCharArr(std::string* str)
{
    return str->empty() ? NULL : &*str->begin();
}

int _GetIP(const char* web_url, char** ip)
{
    hostent* he = gethostbyname(web_url);
    if (he == NULL)
    {
        return -1;
    }

    int l = sizeof(he->h_addr_list)/sizeof(char*);
    printf("%d\n", l);
    for (int i = 0; i < l; i++)
    {
        char* ll = inet_ntoa(*(struct in_addr*)he->h_addr_list[i]);
        printf("%s\n", ll);
    }

    *ip = inet_ntoa(*(struct in_addr*)he->h_addr_list[0]);

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
    int res_ip = _GetIP(_StringAsCharArr(&url_val), &ip);
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
    int res_ip = _GetIP(_StringAsCharArr(&url_val), &ip);
    if (res_ip < 0)
    {
        Napi::TypeError::New(env, "cannot get ip").ThrowAsJavaScriptException();
        return env.Null();
    }

    printf("%s\n", ip);

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