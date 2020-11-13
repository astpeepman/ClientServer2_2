// dllmain.cpp : Определяет точку входа для приложения DLL.
#include "framework.h"
#include "MsgHeader.pb.h"
#include <string>
#include <windows.h>
#include <stdlib.h>

using namespace std;


#pragma once

struct MsgHeader {
    string m_To;
    string m_From;
    int m_Type;
    int m_Size;
};



#define DLL_EXPORT __declspec(dllexport) 

//char* ch;
//
//char* fooStrTo_Dynamic_CH(const string a) {
//    size_t len = a.size() + 1;
//    ch = new char[len];
//    return strcpy(ch, a.c_str());
//}
//
//extern "C" DLL_EXPORT void DeleteChar() {
//    delete[] ch;
//}
//
////extern "C" DLL_EXPORT void getHeaderFromString(char* buffer, char**)
//
//extern "C" DLL_EXPORT char* getStringFromHeader(char* From, char* To, int type, int size) {
//    string buff;
//    Serialize::MsgHeaderProto m_buffer;
//    m_buffer.set_m_from(From);
//    m_buffer.set_m_to(To);
//    m_buffer.set_m_type(type);
//    m_buffer.set_m_size(size);
//    m_buffer.SerializeToString(&buff);
//    //sprintf_s(buffer, buff.size()+1, buff.c_str());
//    //buffer = fooStrTo_Dynamic_CH(buff);
//    
//    //printf((const char*)yo);
//    //strcpy_s(*str, buffer.size(), buffer.c_str());
//    //strcpy(m_buffer.m_From, m->m_From);
//    //strcpy(m_buffer.m_To, m->m_);
//    return fooStrTo_Dynamic_CH(buff);
//}

//extern "C" DLL_EXPORT void getHeaderFromString(char* buffer) {
//    Serialize::MsgHeaderProto HeaderBuffer;
//    string buff="";
//   /* To = (char*)(*buffer);
//    for (int i = 0; i < sizeof(buffer); i++) {
//        buff += (*buffer)[i];
//    }
//    HeaderBuffer.ParseFromString(buff);
//    return fooStrTo_Dynamic_CH(HeaderBuffer.m_from());*/
//    /*char i;
//    *pSize = 4;
//    *ppMem = (unsigned char*)malloc(*pSize);
//    for (i = 0; i < *pSize; i++)
//        (*ppMem)[i] = 'y';*/
//}

extern "C" __declspec(dllexport) void getM_HeaderFromString(char* buffer, char from[], char to[], int &type, int &size) {
    MsgHeader m_header;
    Serialize::MsgHeaderProto HeaderBuffer;
    HeaderBuffer.ParseFromString(buffer);

    const char* frombuf = HeaderBuffer.m_from().c_str();
    sprintf_s(from, HeaderBuffer.m_from().size() + 1, frombuf);
    from[HeaderBuffer.m_from().size()] = '\0';

    const char* Tobuf = HeaderBuffer.m_to().c_str();
    sprintf_s(to, HeaderBuffer.m_to().size() + 1, Tobuf);
    to[HeaderBuffer.m_to().size()] = '\0';

    type = HeaderBuffer.m_type();
    size = HeaderBuffer.m_size();

}

extern "C" __declspec(dllexport) void  getSerializeString(char* str, char* from, char* to, int type, int size) {
    string buffer;
    Serialize::MsgHeaderProto HeaderBuffer;

    HeaderBuffer.set_m_from(from);
    HeaderBuffer.set_m_to(to);
    HeaderBuffer.set_m_size(size);
    HeaderBuffer.set_m_type(type);

    HeaderBuffer.SerializeToString(&buffer);

    //const char* source = buffer.c_str();
    //strcpy_s(*str, buffer.size(), buffer.c_str());
    //strcpy_s(*str, buffer.size(), buffer);
    sprintf_s(str, buffer.size()+1, buffer.c_str());
    str[buffer.size()] = '\0';
}


extern "C" __declspec(dllexport) BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}