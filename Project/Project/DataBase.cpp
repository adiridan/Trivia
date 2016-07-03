#include "DataBase.h"

string _count;
vector<Question*> questions;
vector<string> scores; // BestScores or personalScore- status

DataBase::DataBase()
{
	stringstream s;
	if (sqlite3_open("trivia.db", &db))
	{
		sqlite3_close(db);
		s << "Can't open database: " << sqlite3_errmsg(db);
		throw exception(s.str().c_str());
		s.clear();
	}
}

DataBase::~DataBase()
{
	sqlite3_close(db);
}

bool DataBase::isUserExist(string username)
{
	bool re;
	char* zErrMsg;
	stringstream s, err;
	err << "SQL error: ";
	s << "select count(*) from t_users where userName == '" << username << "';";
	if (sqlite3_exec(db, s.str().c_str(), callbackCount, 0, &zErrMsg) != SQLITE_OK)
	{
		err << zErrMsg;
		sqlite3_free(zErrMsg);
		throw exception(err.str().c_str());
	}
	re = (_count != "0");
	return re;
}

bool DataBase::addNewUser(string username, string password, string email)
{
	bool re = true;
	char* zErrMsg;
	stringstream s, err;
	err << "SQL error: ";
	s << "insert into t_users values ('" << username << "','" << encryptUsing_md5(password) << "','" << email << "');";
	if (sqlite3_exec(db, s.str().c_str(), NULL, 0, &zErrMsg) != SQLITE_OK)
	{
		re = false;
		err << zErrMsg;
		sqlite3_free(zErrMsg);
		throw exception(err.str().c_str());
	}
	return re;
}

bool DataBase::isUserAndPassMatch(string username, string password)
{
	bool re;
	char* zErrMsg;
	stringstream s, err;
	err << "SQL error: ";
	s << "select count(*) from t_users where userName == '" << username << "' and password == '" << encryptUsing_md5(password) << "';";
	if (sqlite3_exec(db, s.str().c_str(), callbackCount, 0, &zErrMsg) != SQLITE_OK)
	{
		err << zErrMsg;
		sqlite3_free(zErrMsg);
		throw exception(err.str().c_str());
	}
	re = (_count != "0");
	return re;
}

vector<Question*> DataBase::initQuestions(int questionsNo)
{
	stringstream s, err;
	char* zErrMsg;
	err << "SQL error: ";
	s << "select * from t_questions order by abs(random()) limit " << questionsNo << ";";
	questions.clear();

	if (sqlite3_exec(db, s.str().c_str(), callbackQuestions, 0, &zErrMsg) != SQLITE_OK)
	{
		err << zErrMsg;
		sqlite3_free(zErrMsg);
		throw exception(err.str().c_str());
	}
	return questions;
}

vector<string> DataBase::getBestScores()
{
	stringstream s, err;
	char* zErrMsg;
	err << "SQL error: ";
	s << "select username,max(win_count) as max_count from (select username,count(*) as win_count from t_players_answers where is_correct == 1 group by username) group by username order by max_count desc limit 3;";
	scores.clear();

	if (sqlite3_exec(db, s.str().c_str(), callbackBestScores, 0, &zErrMsg) != SQLITE_OK)
	{
		err << zErrMsg;
		sqlite3_free(zErrMsg);
		throw exception(err.str().c_str());
	}
	return scores;
}

int DataBase::insertNewGame()
{
	stringstream err;
	char* zErrMsg;
	int re;
	err << "SQL error: ";
	if (sqlite3_exec(db, "insert into t_games(status, start_time, end_time) values(0, datetime('now'), NULL);", NULL, 0, &zErrMsg) != SQLITE_OK)
	{
		re = -1;
		err << zErrMsg;
		sqlite3_free(zErrMsg);
		throw exception(err.str().c_str());
	}
	else
		re = sqlite3_last_insert_rowid(db);
	return re;
}

bool DataBase::updateGameStatus(int id)
{
	bool re = true;
	char* zErrMsg;
	stringstream s, err;
	err << "SQL error: ";
	s << "update t_games set status = 0, end_time = datetime('now') where game_id == " << id << ";";
	if (sqlite3_exec(db, s.str().c_str(), NULL, 0, &zErrMsg) != SQLITE_OK)
	{
		re = false;
		err << zErrMsg;
		sqlite3_free(zErrMsg);
		throw exception(err.str().c_str());
	}
	return re;
}

bool DataBase::addAnswerToPlayer(int gameID, string username, int questionID, string answer, bool isCorrect, int answerTime)
{
	bool re = true;
	char* zErrMsg;
	stringstream s, err;
	err << "SQL error: ";
	s << "insert into t_players_answers values(" << gameID << ",'" << username << "'," << questionID << ",'" << answer << "'," << (isCorrect ? 1:0) << "," << answerTime << ");";
	
	if (sqlite3_exec(db, s.str().c_str(), NULL, 0, &zErrMsg) != SQLITE_OK)
	{
		re = false;
		err << zErrMsg;
		sqlite3_free(zErrMsg);
		throw exception(err.str().c_str());
	}
	return re;
}

vector<string> DataBase::getPersonalStatus(string username)
{
	char* zErrMsg;
	stringstream s, err;
	err << "SQL error: ";
	s << "select count(distinct game_id), sum(is_correct), count(*) - sum(is_correct), avg(answer_time) from t_players_answers where username='" << username << "'";
	scores.clear();
	
	if (sqlite3_exec(db, s.str().c_str(), callbackPersonalStatus, 0, &zErrMsg) != SQLITE_OK)
	{
		err << zErrMsg;
		sqlite3_free(zErrMsg);
		throw exception(err.str().c_str());
	}
	return scores;
}


int DataBase::callbackCount(void * notUsed, int argc, char ** argv, char ** azCol)
{
	if (argc > 0)
		_count = argv[0];
	return 0;
}

int DataBase::callbackQuestions(void * notUsed, int argc, char ** argv, char ** azCol)
{
	Question* currQuestion = nullptr;
	currQuestion = new Question(atoi(argv[0]), argv[1], argv[2], argv[3], argv[4], argv[5]);
	if (currQuestion)
		questions.push_back(currQuestion);
	return 0;
}

int DataBase::callbackBestScores(void * notUsed, int argc, char ** argv, char ** azCol)
{
	string currScore;
	currScore = Helper::getPaddedNumber(string(argv[0]).length(), 2);
	currScore += argv[0];
	currScore += Helper::getPaddedNumber(atoi(argv[1]), 6);
	scores.push_back(currScore);
	return 0;
}

int DataBase::callbackPersonalStatus(void * notUsed, int argc, char ** argv, char ** azCol)
{
	stringstream sstr;
	if (argv[3] == nullptr)
		sstr << /*number of games*/"0000" << /*number of right answers*/"000000" << /*number of wrong answers*/"000000" << /*average time for answer*/"0000";
	else
	{
		string s = argv[3];
		sstr << Helper::getPaddedNumber(atoi(argv[0]), 4) << Helper::getPaddedNumber(atoi(argv[1]), 6) << Helper::getPaddedNumber(atoi(argv[2]), 6) << Helper::getPaddedNumber(atoi(s.substr(0, s.find('.')).c_str()), 2) << Helper::getPaddedNumber(atoi(s.substr(s.find('.') + 1, s.find('.') + 1).c_str()), 2);
	}
	scores.push_back(sstr.str());
	return 0;
}

string DataBase::encryptUsing_md5(string str)
{
	return _cryptoDevice.md5(_cryptoDevice.md5("4326446" + str + "some ramdom string") + "End Project!!");
}

string DataBase::encryptAES(string str)
{
	return _cryptoDevice.encryptAES(str);
}

string DataBase::decryptAES(string str)
{
	return _cryptoDevice.decryptAES(str);
}
