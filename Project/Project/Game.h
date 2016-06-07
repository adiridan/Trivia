#pragma once
#include <map>
#include<vector>
#include "User.h"
#include "DataBase.h"

using namespace std;

class Game
{
public:
	Game(const vector<User*>& players, int questionNo, DataBase& db);

private:
	map<string, int> _results;
	//vector<Questions*> _questions
	vector<User*> _players;
	int _question_no, _currQuestionIndex, _currTurnAnswers;
	//DataBase& _db;
};