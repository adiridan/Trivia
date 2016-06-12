#pragma once
#include "sqlit3.h"
#include <vector>
#include "Question.h"
#include <sstream>

using namespace std;

class DataBase
{
public:
	DataBase();
	~DataBase();
	bool isUserExist(string username);
	bool addNewUser(string username, string password, string email);
	bool isUserAndPassMatch(string username, string password);
	vector<Question*> initQuestions(int questionsNo);
	vector<string> getBestScores();
	vector<string> getPersonalStatus(string);
	int insertNewGame();
	bool updateGameStatus(int id);
	bool addAnswerToPlayer(int gameID, string username, int questionID, string answer, bool isCorrect, int answerTime);

private:
	static int callbackCount(void* notUsed, int argc, char** argv, char** azCol);
	static int callbackQuestions(void* notUsed, int argc, char** argv, char** azCol);
	static int callbackBestScores(void* notUsed, int argc, char** argv, char** azCol);
	static int callbackPersonalStatus(void* notUsed, int argc, char** argv, char** azCol);

	sqlite3* db;
	char* zErrMsg, count;
	int id = 0;
	vector<Question*> questions;
	vector<string*> scores;
};