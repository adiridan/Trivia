#include "Room.h"

Room::Room(int id, User * admin, string name, int maxUsers, int questionNo, int questionTime) : _id(id), _name(name), _admin(admin), _maxUsers(maxUsers), _questionsNo(questionNo), _questionTime(questionTime)
{
	_users.push_back(_admin);
}

Room::~Room(){}

bool Room::joinRoom(User * user)
{
	bool re = _users.size() < _maxUsers;

	if (re)
	{
		
		string message = SERVER_JOIN_ROOM_SECCESS;//"1100"
		_users.push_back(user);
		
		message += Helper::getPaddedNumber(_questionsNo, 2);
		message += Helper::getPaddedNumber(_questionTime, 2);

		user->send(message);
		sendMessage(getUsersListMessage());
	}
	else
		user->send(SERVER_JOIN_ROOM_FULL);
	return re;
}

void Room::leaveRoom(User * user)
{
	unsigned int i;
	for (i = 0;i < _users.size() && _users[i]->getSocket() != user->getSocket() ;i++);
	_users.erase(_users.begin() + i);
	user->send(SERVER_LEAVE_ROOM);
	sendMessage(user, getUsersListMessage());
}

int Room::closeRoom(User * user)
{
	int re = -1;
	if (user->getSocket() == _admin->getSocket())
	{
		sendMessage(SERVER_CLOSE_ROOM);
		for (unsigned int i = 0; i < _users.size();i++)
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
	for (unsigned int i = 0; i < usersList.size(); i++)
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
	for (unsigned int i = 0; i < _users.size();i++)
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
	for (unsigned int i = 0; i < _users.size();i++)
	{
		message += Helper::getPaddedNumber(_users[i]->getUsername().length(), 2);
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
