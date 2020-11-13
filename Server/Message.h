#pragma once
int confirm = 1;

enum Messages {
	M_CREATE,
	M_EXIST,
	M_NOUSER,
	M_INIT,
	M_EXIT,
	M_GETDATA,
	M_NODATA,
	M_TEXT,
	M_CONFIRM,
	M_INCORRECT,
	M_ACTIVE,
	M_INACTIVE,
	M_ABSENT
};

struct MsgHeader {
	string m_To;
	string m_From;
	int m_Type;
	int m_Size;
};

extern "C" {
	__declspec(dllimport) void getM_HeaderFromString(char* buffer, char from[], char to[], int& type, int& size);
	__declspec(dllimport) void getSerializeString(char* str, char* from, char* to, int type, int size);
}

class Message {
private:
	MsgHeader m_header;
	string m_data;
public:
	MsgHeader getM_Header() {
		return m_header;
	}

	void setM_Header(MsgHeader h) {
		m_header = h;
	}

	string getM_Data() {
		return m_data;
	}

	void setM_Data(string d) {
		m_data = d;
	}
	Message() {
		m_header.m_From="";
		m_header.m_To="";
		m_header.m_Type = -1;
		m_header.m_Size=0;

	}

	Message(string to, string from, int type = M_TEXT, const string& data = "") {
		m_header.m_From=from;
		m_header.m_To=to;
		m_header.m_Type=type;
		m_header.m_Size=data.length();
		m_data = data;
	}

	void Send(CSocket& s) {
		char* from = &m_header.m_From[0u];
		char* to = &m_header.m_To[0u];
		char* BUFFER=new char[1024];
		getSerializeString(BUFFER, from, to, m_header.m_Type, m_header.m_Size);

		string buffer = BUFFER;
		int len = buffer.size();
		s.Send(&len, sizeof(int));
		s.Send(buffer.c_str(), len);

		if (m_header.m_Type == M_TEXT) {
			s.Send(m_data.c_str(), m_header.m_Size);
		}

		//delete[] from;
		delete[] BUFFER;

	}



	int Receive(CSocket& s, string& Password) {
		string buffer;
		int len; 
		s.Receive(&len, sizeof(int));
		char* pBuff = new char[len + 1];
		s.Receive(pBuff, len);
		pBuff[len] = '\0';
		

		char* fromNew = new char[1024];
		char* toNew = new char[1024];
		getM_HeaderFromString(pBuff, fromNew, toNew, m_header.m_Type, m_header.m_Size);
		m_header.m_From = fromNew;
		m_header.m_To = toNew;
		delete[] pBuff;
		delete[] fromNew;
		delete[] toNew;
		if (m_header.m_Type == M_TEXT) {
			pBuff = new char[m_header.m_Size + 1];
			s.Receive(pBuff, m_header.m_Size);
			pBuff[m_header.m_Size] = '\0';
			m_data = pBuff;
			delete[] pBuff;
		}
		

		if (m_header.m_Type == M_INIT || m_header.m_Type == M_CREATE) {
			//Получаем пароль
			s.Receive(&len, sizeof(int));
			pBuff = new char[len + 1];
			s.Receive(pBuff, len);
			pBuff[len] = '\0';
			Password = pBuff;
			delete[] pBuff;
		}
		
		
		return m_header.m_Type;
	}

	static void SendMessage(CSocket& s, string To, string From, int Type = M_TEXT, const string& Data = "")
	{
		Message msg(To, From, Type, Data);
		msg.Send(s);
	}
};