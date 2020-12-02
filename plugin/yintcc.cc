#include <node.h>
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

namespace plugin {

const int PORT = 80;

char* GetStringFromLocal(v8::Local<v8::String> key)
{
    uint32_t utf8_length = key->Utf8Length();
    char* buffer = new char[utf8_length];
    key->WriteUtf8(buffer);
    // smart pointers can't be used as WriteUtf8 takes plain char *
    // e.g. std::unique_ptr<char*> buffer = new char[utf8_length];
    // so we have to clean memory explicitly
    //delete[] buffer;
    return buffer;
}

int _GetIP(const char* web_url, char** ip)
{
    hostent* he = gethostbyname(web_url);
    if (he == NULL)
    {
        return -1;
    }

    *ip = inet_ntoa(*(struct in_addr*)he->h_addr_list[0]);

    return 0;
}

void GetIP(const v8::FunctionCallbackInfo<v8::Value>& args)
{

    v8::Isolate* isolate = args.GetIsolate();

    if (args.Length() <= 0)
    {
        v8::Local<v8::String> err_val = v8::String::NewFromUtf8(isolate, "required one argument");
        isolate->ThrowException(v8::Exception::TypeError(err_val));
        return;
    }

    v8::Local<v8::String> url_val = args[0].As<v8::String>();
    char* url_buffer = GetStringFromLocal(url_val);

    char* ip = nullptr;
    int res_ip = _GetIP(url_buffer, &ip);
    if (res_ip < 0)
    {
        v8::Local<v8::String> err_val = v8::String::NewFromUtf8(isolate, "cannot get ip");
        isolate->ThrowException(v8::Exception::TypeError(err_val));
        return;
    }

    v8::Local<v8::String> ret_val = v8::String::NewFromUtf8(isolate, ip);
    args.GetReturnValue().Set(ret_val);

}

void DoKurl(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Isolate* isolate = args.GetIsolate();

    if (args.Length() <= 0)
    {
        v8::Local<v8::String> err_val = v8::String::NewFromUtf8(isolate, "required one argument");
        isolate->ThrowException(v8::Exception::TypeError(err_val));
        return;
    }

    v8::Local<v8::String> url_val = args[0].As<v8::String>();
    char* url_buffer = GetStringFromLocal(url_val);

    std::cout << url_buffer << std::endl;

    char* ip = nullptr;
    int res_ip = _GetIP(url_buffer, &ip);
    if (res_ip < 0)
    {
        v8::Local<v8::String> err_val = v8::String::NewFromUtf8(isolate, "cannot get host information: NULL");
        isolate->ThrowException(v8::Exception::TypeError(err_val));
        return;
    }
    printf("%s\n", ip);

    v8::Local<v8::String> ret_val = v8::String::NewFromUtf8(isolate, "fu*k");
    args.GetReturnValue().Set(ret_val);
    delete url_buffer;
}

void Init(v8::Local<v8::Object> exports)
{
    NODE_SET_METHOD(exports, "getIP", GetIP);
    NODE_SET_METHOD(exports, "doKurl", DoKurl);
}

NODE_MODULE(NODE_GYP_MODULE_NAME, Init);

}