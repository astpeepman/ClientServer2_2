#pragma once
class Session {
private:
	int s_id;
	queue<Message> m_messages;
	CRITICAL_SECTION m_CS;
	bool connect;
public:

	int getS_ID() {
		return s_id;
	}
	
	void DisconnectSession() {
		connect = false;
	}

	void ConnectSession() {
		connect = true;
	}

	bool getConnect() {
		return connect;
	}




	Session()
		: connect(true)
	{
		
		InitializeCriticalSection(&m_CS);
	}
	~Session()
	{
		DeleteCriticalSection(&m_CS);
	}

	void Add(Message& msg)
	{
		EnterCriticalSection(&m_CS);
		m_messages.push(msg);
		LeaveCriticalSection(&m_CS);
	}
	void Send(CSocket& s, string login)
	{
		if (connect) {
			EnterCriticalSection(&m_CS);
			if (m_messages.empty())
			{
				Message::SendMessage(s, login, "SERVER", M_NODATA);
			}
			else
			{
				m_messages.front().Send(s);
				m_messages.pop();
			}

			LeaveCriticalSection(&m_CS);
		}
	}


};