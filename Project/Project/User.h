#pragma once
#include"Room.h"
#include <WinSock2.h>
#include <Windows.h>
#include <Ws2tcpip.h>
#include "Helper.h"
#include <iostream>
#include "Game.h"

using namespace std;
class Game;
class Room;
class User
{
public:
	User(string username, SOCKET sock);
	~User();

	//send a string using Helper::sendData
	void send(string message);
	
	string getUsername();
	SOCKET getSocket();
	
	Game* getGame();
	void setGame(Game* gm);
	void clearGame();
	bool leavGame();
	
	Room* getRoom();
	void clearRoom();
	bool creatRoom(int roomId, string RoomName, int maxUsers, int questionsNo, int questionTime);
	bool joinRoom(Room* newRoom);
	void leavRoom();
	int closeRoom();

private:
	string _username;
	Room* _currRoom;
	Game* _currGame;
	SOCKET _socket;
};