#include "DataBase.h"

DataBase::DataBase()
{
	if (sqlite3_open("server.db", &db))
	{
		sqlite3_close(db);
		stringstream s;
		s << "Can't open database: " << sqlite3_errmsg(db);
		throw exception(s.str().c_str());
	}
}

DataBase::~DataBase()
{
	sqlite3_close(db);
}

bool DataBase::isUserExist(string username)
{
	bool re;
	stringstream s, err;
	err << "SQL error: ";
	s << "select count(*) from t_users where userName == " << username << ";";
	if (sqlite3_exec(db, s.str().c_str(), /*callbackCount*/ NULL, 0, &zErrMsg) != SQLITE_OK)
	{
		err << zErrMsg;
		throw exception(err.str().c_str());
	}
	re = (count != '0');
	return re;
}

bool DataBase::addNewUser(string username, string password, string email)
{
	bool re = true;
	stringstream s, err;
	err << "SQL error: ";
	s << "insert into t_users values (" << username << "," << password << "," << email << ");";
	if (sqlite3_exec(db, s.str().c_str(), NULL, 0, &zErrMsg) != SQLITE_OK)
	{
		err << zErrMsg;
		throw exception(err.str().c_str());
		re = false;
	}
	return re;
}

bool DataBase::isUserAndPassMatch(string username, string password)
{
	bool re;
	stringstream s, err;
	err << "SQL error: ";
	s << "select count(*) from t_users where userName == " << username << " and password == " << password << ";";
	if (sqlite3_exec(db, s.str().c_str(), /*callbackCount*/ NULL, 0, &zErrMsg) != SQLITE_OK)
	{
		err << zErrMsg;
		throw exception(err.str().c_str());
	}
	re = (count != '0');
	return re;
}

vector<Question*> DataBase::initQuestions(int questionsNo)
{
	stringstream s, err;
	err << "SQL error: ";
	s << "select * from t_question order by abs(random()) limit" << questionsNo << ";";
	if (sqlite3_exec(db, s.str().c_str(), /*callbackQuestions*/ NULL, 0, &zErrMsg) != SQLITE_OK)
	{
		err << zErrMsg;
		throw exception(err.str().c_str());
	}
	return questions;
}

int DataBase::insertNewGame()
{
	stringstream s, err;
	err << "SQL error: ";
	s << "insert into t_games values(" << (++id) << ", 0, datetime('now'), NULL);";
	if (sqlite3_exec(db, s.str().c_str(), NULL, 0, &zErrMsg) != SQLITE_OK)
	{
		err << zErrMsg;
		throw exception(err.str().c_str());
	}
	return id;
}

bool DataBase::updateGameStatus(int id)
{
	bool re = true;
	stringstream s, err;
	err << "SQL error: ";
	s << "update t_games set status = 0, end_time = datetime('now') where id == " << id << ";";
	if (sqlite3_exec(db, s.str().c_str(), NULL, 0, &zErrMsg) != SQLITE_OK)
	{
		re = false;
		err << zErrMsg;
		throw exception(err.str().c_str());
	}
	return re;
}

bool DataBase::addAnswerToPlayer(int gameID, string username, int questionID, string answer, bool isCorrect, int answerTime)
{
	bool re = true;
	stringstream s, err;
	err << "SQL error: ";
	s << "insert into t_players_answers values(" << gameID << ", '" << username << "', " << questionID << ", '" << answer << "', " << (isCorrect ? 1:0) << ", " << answerTime << ", " << ";";
	if (sqlite3_exec(db, s.str().c_str(), NULL, 0, &zErrMsg) != SQLITE_OK)
	{
		re = false;
		err << zErrMsg;
		throw exception(err.str().c_str());
	}
	return re;
}

