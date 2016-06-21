#include "User.h"

User::User(string username, SOCKET sock) : _username(username), _socket(sock), _currRoom(nullptr), _currGame(nullptr){}

User::~User(){}

void User::send(string message)
{
	if (message.length() > 0)
	{
		try
		{
			Helper::sendData(_socket, message);
		}
		catch (std::exception e)
		{
//			cout << e.what() << endl;
			cout << "sending message:" << message << endl;
		}
	}
}

string User::getUsername()
{
	return _username;
}

SOCKET User::getSocket()
{
	return _socket;
}

Room * User::getRoom()
{
	return _currRoom;
}

void User::clearRoom()
{
	_currRoom = nullptr;
}

bool User::creatRoom(int roomId, string RoomName, int maxUsers, int questionsNo, int questionTime)
{
	bool re = _currRoom == nullptr;
	string message = SERVER_CREAT_ROOM_SECCESS;
	if (re)
		_currRoom = new Room(roomId, this, RoomName, maxUsers, questionsNo, questionTime);
	else
		message = SERVER_CREAT_ROOM_FAIL;
	send(message);
	return re;
}

bool User::joinRoom(Room * newRoom)
{
	bool re = false;
	if (_currRoom == nullptr && newRoom->joinRoom(this))
	{
		re = true;
		_currRoom = newRoom;
	}
	return re;
}

void User::leavRoom()
{
	if (_currRoom != nullptr)
	{
		_currRoom->leaveRoom(this);
		_currRoom = nullptr;
	}
}

int User::closeRoom()
{
	int re = -1;
	if (_currRoom != nullptr)
	{
		re = _currRoom->closeRoom(this);
		if (re != -1)
			clearRoom();
	}
	return re;
}

void User::setGame(Game* gm)
{
	_currRoom = nullptr;
	_currGame = gm;
}

Game* User::getGame()
{
	return _currGame;
}

void User::clearGame()
{
	_currGame = nullptr;
}

bool User::leavGame()
{
	bool re = false;
	if (_currGame != nullptr)
	{
		re = _currGame->leaveGame(this);
		_currGame = nullptr;
	}
	return re;
}