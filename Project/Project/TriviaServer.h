#pragma once

#include <WinSock2.h>
#include <Windows.h>
#include <map>
#include <queue>
#include "RecievedMessage.h"
#include <thread>
#include <mutex>
#include "User.h"
#include <condition_variable>
#include "Validator.h"
#include "DataBase.h"

#define PORT 8820

using namespace std;

class TriviaServer
{
public:
	TriviaServer();
	~TriviaServer();

	void server();

	void bindAndListen();
	void accept();
	void clientHandler(SOCKET client_socket);
	void safeDeleteUesr(RecievedMessage* msg);

	User* getUserByName(string username);
	User* getUserBySocket(SOCKET client_socket);
	Room* getRoomById(int roomId);

	RecievedMessage* buildRecieveMessage(SOCKET client_socket, int msgCode);
	void addRecievedMessage(RecievedMessage* msg);
	void handleRecivedMessages();

	void handleGetBestScores(RecievedMessage* msg);
	void handleGetPersonalStatus(RecievedMessage* msg);

	User* handleSignin(RecievedMessage* msg);
	bool handleSignup(RecievedMessage* msg);
	void handleSignout(RecievedMessage* msg);
	
	void handleLeaveGame(RecievedMessage* msg);
	void handleStartGame(RecievedMessage* msg);
	void handlePlayerAnswer(RecievedMessage* msg);

	bool handleCreateRoom(RecievedMessage* msg);
	bool handleCloseRoom(RecievedMessage* msg);
	bool handleJoinRoom(RecievedMessage* msg);
	bool handleLeaveRoom(RecievedMessage* msg);
	void handleGetUsersInRoom(RecievedMessage* msg);
	void handleGetRooms(RecievedMessage* msg);

	

private:

	SOCKET _socket;
	map <SOCKET, User*> _connectedUsers;
	//DataBase _db;
	map <int, Room*> _roomsList;

	mutex _mtxRecievedMessages;
	queue <RecievedMessage*> _queRcvMessages;

	int _roomldSequence;

	condition_variable cv;

};

