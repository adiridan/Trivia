#include <iostream>
#include "TriviaServer.h"

int main()
{
	TriviaServer *server = nullptr;
	try
	{
		server = new TriviaServer();
	}
	catch (exception e)
	{
		cout << e.what() << endl;
	}
	if (server)
		server->server();
	system("pause");
	return 0;
}

/*
vector<User*> users;
DataBase db;
Game* game = nullptr;
int ans;

users.push_back(new User("user",52));
users.push_back(new User("user1", 58));
users.push_back(new User("user2", 612));
try
{
game = new Game(users, 3, db);
}
catch (exception e)
{
cout << e.what() << endl;
}

if (game)
{
game->sendFirstQuestion();
do
{
cin >> ans;
game->handleAnswerFromUser(users[0], 5, 0);
game->handleAnswerFromUser(users[1], 3, 5);
} while (game->handleAnswerFromUser(users[2], ans, 8));
}
*/