#include <iostream>
#include "Game.h"

int main()
{	
	DataBase db;
	User *u = new User("user", 55), *u2 = new User("user", 65), *u3 = new User("user", 45);
	Game *game;
	u->creatRoom(5, "room", 3, 2, 5);
	u2->joinRoom(u->getRoom());
	u3->joinRoom(u->getRoom());

	try
	{
		game = new Game(u->getRoom()->getUsers(), u->getRoom()->getQuestionsNo(), db);
	}
	catch (exception e)
	{
		cout << e.what() << endl;
		delete u;
		delete u2;
		delete u3;
		system("pause");
		return 0;
	}

	game->sendFirstQuestion();
	game->handleAnswerFromUser(u, 2, 5);
	game->handleAnswerFromUser(u2, 5, 10);
	game->leaveGame(u3);
	game->handleAnswerFromUser(u, 2, 5);
	game->handleAnswerFromUser(u2, 5, 10);

	delete u;
	delete u2;
	delete u3;
	system("pause");
	return 0;
}
