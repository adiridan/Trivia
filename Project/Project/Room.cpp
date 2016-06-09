#include "Room.h"

Room::Room(int id, User * admin, string name, int maxUsers, int questionNo, int questionTime) : _id(id), _admin(admin), _maxUsers(maxUsers), _questionsNo(questionNo), _questionTime(questionTime)
{
	_users.push_back(_admin);
}

Room::~Room(){}

bool Room::joinRoom(User * user)
{
	bool re = _users.size() <= _maxUsers;

	if (re)
	{
		
		string message = SERVER_JOIN_ROOM_SECCESS;//"1100"
		_users.push_back(user);
		
		message += to_string(_questionsNo);
		message += to_string(_questionTime);

		user->send(message);
		sendMessage(getUsersListMessage());
	}
	else
		user->send(SERVER_JOIN_ROOM_FULL);
	return re;
}

void Room::leaveRoom(User * user)
{
	for (int i = 0;i < _users.size();i++)
	{
		if (_users[i]->getSocket() == user->getSocket())
		{
			_users.erase(_users.begin() + i);
			user->send(SERVER_LEAVE_ROOM);
			sendMessage(user, getUsersListMessage());
		}
	}
}

int Room::closeRoom(User * user)
{
	int re = -1;
	if (user->getSocket() == _admin->getSocket())
	{
		sendMessage(SERVER_CLOSE_ROOM);
		for (int i = 0; i < _users.size();i++)
		{
			if (_users[i]->getSocket() != _admin->getSocket())
				_users[i]->clearRoom();
		}
		re = _id;
	}

	return re;
}

vector<User*> Room::getUsers()
{
	return _users;
}

string Room::getUsersAsString(vector<User*> usersList, User * excludeUser)
{
	string users;
	for (int i = 0; i < usersList.size(); i++)
	{
		if (usersList[i]->getSocket() != excludeUser->getSocket())
			users += ',' + usersList[i]->getUsername();
	}
	return users;
}

void Room::sendMessage(string message)
{
	sendMessage(nullptr, message);
}

void Room::sendMessage(User * excludeUser, string message)
{
	for (int i = 0; i < _users.size();i++)
	{
		if (excludeUser)
		{
			//excludeUser != null
			if (excludeUser->getSocket() != _users[i]->getSocket())
				_users[i]->send(message);
		}
		else
			_users[i]->send(message);
	}
}

string Room::getUsersListMessage()
{
	string message = SERVER_USER_IN_ROOM;
	
	message += to_string(_users.size());
	for (int i = 0; i < _users.size();i++)
	{
		if (_users[i]->getUsername().length() < 10)
			message += '0';
		message += to_string(_users[i]->getUsername().length());
		message += _users[i]->getUsername();
	}
	return message;
}

int Room::getQuestionsNo()
{
	return _questionsNo;
}

int Room::getId()
{
	return _id;
}

string Room::getName()
{
	return _name;
}
