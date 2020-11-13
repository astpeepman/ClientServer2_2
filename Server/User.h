#pragma once

class User {
private:
	int u_id;
	string u_login;
	string u_password;
	Session u_session;
	bool active;
	clock_t lastActive;
public:

	User() {
		u_id = -1;
		u_login = "";
		u_password = "";
		u_session = Session();
		active = false;

	}

	User(string login, string password, Session session, clock_t time) {
		u_login = login;
		u_password = password;
		u_session = session;
		lastActive = time;
		active = true;
	}

	int getU_ID() {
		return u_id;
	}

	void setU_ID(int i) {
		u_id = i;
	}

	string getU_Login() {
		return u_login;
	}

	void setU_Login(string l) {
		u_login = l;
	}

	string getU_Pass() {
		return u_password;
	}

	void setU_Pass(string p) {
		u_password = p;
	}

	void AddMsg(Message m) {
		u_session.Add(m);
	}

	void Send(CSocket& s, clock_t c) {
		u_session.Send(s, u_login);
		lastActive = c;
	}

	void InActive() {
		active = false;
		u_session.DisconnectSession();
	}

	void Active() {
		active = true;
		u_session.ConnectSession();
	}

	bool GetStatus() {
		return active;
	}

	void SetActiveTime(clock_t t) {
		lastActive = t;
	}
	clock_t GetActiveTime() {
		return lastActive;
	}

};