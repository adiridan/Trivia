#pragma once

#include <iostream>
#include <vector>
#include <WinSock2.h>
#include <Windows.h>
#include <WS2tcpip.h>
#include "User.h"

using namespace std;

class RecievedMessage
{
public:
	RecievedMessage(SOCKET sock, int messageCode);
	RecievedMessage(SOCKET sock, int messageCode, vector <string> values);
	~RecievedMessage();

	SOCKET getSock();
	User* getUser();
	void setUser(User* user);
	int getMessageCode();
	vector <string>& getValues();
	



private:
	SOCKET _sock;
	User* _user;
	int _messageCode;
	vector <string> _values;
};