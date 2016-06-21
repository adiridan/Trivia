#pragma once

#include <vector>
#include "Question.h"
#include "User.h"
#include "DataBase.h"
#include <map>

using namespace std;

class User;
class Game
{
public:
	Game(const vector<User*>& players, int questionsNo, DataBase& db);
	~Game();

	void sendFirstQuestion();
	void handleFinishGame();
	bool handleNextTurn();
	bool handleAnswerFromUser(User* user, int answerNo, int time);
	bool leaveGame(User* currUser);
	//int getID();




private:
	vector<Question*> _questions;
	vector<User*> _players;
	int _question_no;
	int _currQuestionIndex;
	DataBase& _db;
	map<string, int> _results;
	int _currentTurnAnswers, _id;

	//bool insertGameToDB();
//	void initQuestionsFromDB();
	void sendQuestionToAllUsers();

};