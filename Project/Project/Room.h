#pragma once
#include "User.h"
#include <vector>
#include "Protocol.h"

using namespace std;

class User;
class Room
{
public:
	Room(int id, User* admin, string name, int maxUsers, int questionNo, int questionTime);
	~Room();
	bool joinRoom(User* user);
	void leaveRoom(User* user);
	int closeRoom(User* user);
	vector<User*> getUsers();
	string getUsersListMessage();
	int getQuestionsNo();
	int getId();
	string getName();


private:
	//DEBUGING
	string getUsersAsString(vector<User*> usersList, User* excludeUser);
	void sendMessage(string message);
	void sendMessage(User* excludeUser, string message);

	vector<User*> _users;
	User* _admin;
	unsigned int _maxUsers, _questionTime, _questionsNo, _id;
	string _name;
};