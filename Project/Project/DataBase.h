#pragma once
#include "sqlit3.h"
#include <vector>
#include "Question.h"
#include <sstream>
#include "Helper.h"
#include <iostream>
#include "CryptoDevice.h"

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
	vector<string> getPersonalStatus(string username);
	int insertNewGame();
	bool updateGameStatus(int id);
	bool addAnswerToPlayer(int gameID, string username, int questionID, string answer, bool isCorrect, int answerTime);

private:
	static int callbackCount(void* notUsed, int argc, char** argv, char** azCol);
	static int callbackQuestions(void* notUsed, int argc, char** argv, char** azCol);
	static int callbackBestScores(void* notUsed, int argc, char** argv, char** azCol);
	static int callbackPersonalStatus(void* notUsed, int argc, char** argv, char** azCol);
	string encryptUsing_md5(string str);
	string encryptAES(string str);
	string decryptAES(string str);

	sqlite3* db;
	CryptoDevice _cryptoDevice;
};