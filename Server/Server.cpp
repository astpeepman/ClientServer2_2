// Server.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include "pch.h"
#include "framework.h"
#include "Server.h"
#include "Message.h"
#include "Session.h"
#include "User.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// Единственный объект приложения

CWinApp theApp;

using namespace std;

map<string, User> Users;

void TimeOut() {
    while (true)
    {
        for (auto i : Users) {
            double workTime = clock() - i.second.GetActiveTime();
            if (workTime > 150000 && i.second.GetStatus() == true) {
                cout << "Client " << i.first << " has been disconnected due to long inactivity" << endl;
                i.second.InActive();
            }
        }
        Sleep(1000);
    }
}

void ProcessClient(SOCKET hSock) {
    CSocket s;
    s.Attach(hSock);
    Message m;
    string Password;
    switch (m.Receive(s, Password))
    {
    case M_CREATE: {
        bool findUser = false;
        for (auto i : Users) {
            if (m.getM_Header().m_From == i.second.getU_Login()) {

                Message::SendMessage(s, m.getM_Header().m_From, "@SERVER", M_EXIST);
                findUser = true;
                break;
            }
        }

        if (!findUser) {
            User u_buffer(m.getM_Header().m_From, Password, Session(), clock());
            Users[u_buffer.getU_Login()] = u_buffer;

            cout << "Client " << u_buffer.getU_Login() << " connected" << endl;
            Message::SendMessage(s, m.getM_Header().m_From, "@SERVER", M_CONFIRM);
        }
        break;
    }
    case M_INIT: {
        bool findUser = false;
        for (auto i = begin(Users); i != end(Users); ++i) {
            if (m.getM_Header().m_From == i->second.getU_Login()) {

                if (Password == i->second.getU_Pass()) {
                    if (i->second.GetStatus()) {
                        Message::SendMessage(s, m.getM_Header().m_From, "@SERVER", M_ACTIVE);
                    }
                    else {
                        
                        //i.second.Send(s, clock());
                        cout << "Client " << m.getM_Header().m_From << " connected" << endl;
                        i->second.Active();
                        Message::SendMessage(s, m.getM_Header().m_From, "@SERVER", M_CONFIRM);
                    }

                }
                else {
                    Message::SendMessage(s, m.getM_Header().m_From, "@SERVER", M_INCORRECT);
                }
                findUser = true;
                break;
            }
        }
        if (!findUser) {
            Message::SendMessage(s, m.getM_Header().m_From, "@SERVER", M_NOUSER);
        }
        break;
    }
    case M_EXIT: {
        if (Users[m.getM_Header().m_From].GetStatus()) {
            Users[m.getM_Header().m_From].InActive();
            cout << "Client " << m.getM_Header().m_From << " disconnected" << endl;
            Message::SendMessage(s, m.getM_Header().m_From, "@SERVER", M_CONFIRM);
        }
        else {
            Message::SendMessage(s, m.getM_Header().m_From, "@SERVER", M_INACTIVE);
        }
        break;
    }
    case M_GETDATA: {
        if (Users[m.getM_Header().m_From].GetStatus()) {
            Users[m.getM_Header().m_From].Send(s, clock());
        }
        break;
    }
    default: {
        if (Users.find(m.getM_Header().m_From) != Users.end()) {
            if (Users.find(m.getM_Header().m_To) != Users.end()) {
                Users[m.getM_Header().m_To].AddMsg(m);
                if (!Users[m.getM_Header().m_To].GetStatus()) {
                    Message::SendMessage(s, m.getM_Header().m_From, "@SERVER", M_INACTIVE);
                }
                else {
                    Message::SendMessage(s, m.getM_Header().m_From, "@SERVER", M_CONFIRM);
                }
            }
            else if (m.getM_Header().m_To == "@ALL") {
                for (auto i : Users) {
                    if (i.first != m.getM_Header().m_From) {
                        i.second.AddMsg(m);
                    }
                }
                Message::SendMessage(s, m.getM_Header().m_From, "@SERVER", M_CONFIRM);
            }
            else {
                Message::SendMessage(s, m.getM_Header().m_From, "@SERVER", M_ABSENT);
            }
            Users[m.getM_Header().m_From].SetActiveTime(clock());
        }
        break;
    }
    }
}

void start() {

    //ТЕСТИРУЕМ СЕРИАЛИЗАЦИЮ
    //Message m;
    //MsgHeader m_Header;
    //m_Header.m_To = "TestTO";
    //m_Header.m_From = "TEstFrom";
    //m_Header.m_Size = 2;
    //m_Header.m_Type = 3;

    //m.setM_Header(m_Header);

    //char* from = &m_Header.m_From[0u];
    //char* to = &m_Header.m_To[0u];
    //char* BUFFER=new char[1024];
    //getSerializeString(BUFFER, from, to, m_Header.m_Type, m_Header.m_Size);
    //

    //
    //MsgHeader newTest;
    //char* fromNew=new char[1024];
    //char* toNew = new char[1024];
    //getM_HeaderFromString(BUFFER, fromNew, toNew, newTest.m_Type, newTest.m_Size);
    //newTest.m_From = fromNew;
    //newTest.m_To = toNew;

    ////MsgHeader NewM_Header = getM_HeaderFromString(buffer);

    //cout << "Complete" << endl;

    AfxSocketInit();
    CSocket Server;
    Server.Create(11111);

    thread tt(TimeOut);
    tt.detach();

    while (true)
    {
        Server.Listen();
        CSocket s;
        Server.Accept(s);
        thread t(ProcessClient, s.Detach());
        t.detach();
    }
}

int main()
{
    int nRetCode = 0;

    HMODULE hModule = ::GetModuleHandle(nullptr);

    if (hModule != nullptr)
    {
        // инициализировать MFC, а также печать и сообщения об ошибках про сбое
        if (!AfxWinInit(hModule, nullptr, ::GetCommandLine(), 0))
        {
            // TODO: вставьте сюда код для приложения.
            wprintf(L"Критическая ошибка: сбой при инициализации MFC\n");
            nRetCode = 1;
        }
        else
        {
            // TODO: вставьте сюда код для приложения.
            start();
        }
    }
    else
    {
        // TODO: измените код ошибки в соответствии с потребностями
        wprintf(L"Критическая ошибка: сбой GetModuleHandle\n");
        nRetCode = 1;
    }

    return nRetCode;
}
