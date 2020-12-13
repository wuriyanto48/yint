#include <napi.h>
#include <fstream>
#include "YintSocket.h"

namespace yint {

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

    const char* ip = nullptr;
    int res_ip = _ExtractURL(_StringAsCharArr(&url_val), &ip, NULL, NULL);
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

    const char* ip = nullptr;
    const char* hostname = nullptr;
    //char* path = (char*) malloc(sizeof(char*) * 1024);
    char* path = static_cast<char*>(malloc(sizeof(char*) * 1024));
    int res_ip = _ExtractURL(_StringAsCharArr(&url_val), &ip, &hostname, &path);
    if (res_ip < 0)
    {
        Napi::TypeError::New(env, "cannot get ip").ThrowAsJavaScriptException();
        return env.Null();
    }

    int sock_FD = _CreateSocket(ip, HTTP_PORT);
    if (sock_FD == -1) 
    {
        free(path);
        Napi::TypeError::New(env, "error: creating socket").ThrowAsJavaScriptException();
        return env.Null();
    }

    printf("%s\n", "---------");
    printf("ip : %s\n", ip);
    printf("host : %s\n", hostname);
    printf("%d\n", sock_FD);
    printf("path : %s\n", path);

    // write response to file
    std::ofstream out_file;
    out_file.open("out.txt");

    int sent = _SendReqWriteOut(sock_FD, Method::GET, path, hostname, out_file);
    if (sent == -1) 
    {
        free(path);
        Napi::TypeError::New(env, "error: sending request").ThrowAsJavaScriptException();
        return env.Null();
    }

    // free path
    free(path);

    //close file
    out_file.close();

    // close socket connection
    _CloseSocket(sock_FD);

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