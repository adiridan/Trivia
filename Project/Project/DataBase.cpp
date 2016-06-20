#include "DataBase.h"

string _count;
vector<Question*> questions;
vector<string> scores; // BestScores or personalScore- status

DataBase::DataBase()
{
	stringstream s;
	char* zErrMsg;
	if (sqlite3_open("trivia.db", &db))
	{
		sqlite3_close(db);
		s << "Can't open database: " << sqlite3_errmsg(db);
		throw exception(s.str().c_str());
		s.clear();
	}
	else if (sqlite3_exec(db, "pragma primary_key = 1;", NULL, 0, &zErrMsg) != SQLITE_OK)
	{
		s << "Can't activate primary key: " << zErrMsg;
		throw exception(s.str().c_str());
		s.clear();
	}
	else if (sqlite3_exec(db, "create table if not exists t_users (username text primary key not null, password text not null, email text not null);", NULL, 0, &zErrMsg) != SQLITE_OK)
	{
		s << "Can't create table t_users: " << zErrMsg;
		throw exception(s.str().c_str());
		s.clear();
	}
	else if (sqlite3_exec(db, "create table if not exists t_games (game_id integer primary key autoincrement not null, status integer not null, start_time datetime not null, end_time datetime);", NULL, 0, &zErrMsg) != SQLITE_OK)
	{
		s << "Can't create table t_games: " << zErrMsg;
		throw exception(s.str().c_str());
		s.clear();
	}
	else if (sqlite3_exec(db, "pragma foreign_keys=1;", NULL, 0, &zErrMsg) != SQLITE_OK)
	{
		s << "Can't activate forigen key: " << zErrMsg;
		throw exception(s.str().c_str());
		s.clear();
	}
	else if (sqlite3_exec(db, "create table if not exists t_players_answers (game_id integer not null, username text not null, question_id				";
"		integer not null, player_answer text not null, is_correct integer not null, answer_time integer not null, primary key(game_id,username,			";
"		question_id), foreign key(game_id) references t_games(game_id),foreign key(username) references t_users(username),foreign key(question_id)		";
"		references t_questions(question_id));", NULL, 0, &zErrMsg) != SQLITE_OK)
	{
		s << "Can't create table t_players_answers: " << zErrMsg;
		throw exception(s.str().c_str());
		s.clear();
	}
	
	//adding questions:
	/*
	else if (sqlite3_exec(db, ";", NULL, 0, &zErrMsg) != SQLITE_OK)
	{
	s << "Can't create table t_players_answers: " << zErrMsg;
	throw exception(s.str().c_str());
	s.clear();
	}
	*/
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
	s << "insert into t_users values ('" << username << "','" << password << "','" << email << "');";
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
	char* zErrMsg;
	stringstream s, err;
	err << "SQL error: ";
	s << "select count(*) from t_users where userName == '" << username << "' and password == '" << password << "';";
	if (sqlite3_exec(db, s.str().c_str(), callbackCount, 0, &zErrMsg) != SQLITE_OK)
	{
		err << zErrMsg;
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
	if (sqlite3_exec(db, s.str().c_str(), callbackQuestions, 0, &zErrMsg) != SQLITE_OK)
	{
		err << zErrMsg;
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
	if (sqlite3_exec(db, s.str().c_str(), callbackBestScores, 0, &zErrMsg) != SQLITE_OK)
	{
		err << zErrMsg;
		throw exception(err.str().c_str());
	}
	return scores;
}

int DataBase::insertNewGame()
{
	stringstream s, err;
	char* zErrMsg;
	err << "SQL error: ";
	s << "insert into t_games(status, start_time, end_time) values(0, datetime('now'), NULL);";
	if (sqlite3_exec(db, s.str().c_str(), NULL, 0, &zErrMsg) != SQLITE_OK)
	{
		err << zErrMsg;
		throw exception(err.str().c_str());
	}
	return sqlite3_last_insert_rowid(db);
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
	
	
	s << "insert into t_players_answers(game_id, username, question_id, player_answer, is_correct, answer_time) values(1,'adi', 6, 'answer', 0, 5);";
	
	
	//s << "insert into t_players_answers values(" << gameID << ",'" << username << "'," << questionID << ",'" << answer << "'," << (isCorrect ? 1:0) << "," << answerTime << ");";
	if (sqlite3_exec(db, s.str().c_str(), NULL, 0, &zErrMsg) != SQLITE_OK)
	{
		re = false;
		err << zErrMsg;
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
	Question* currQuestion;
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
	string s = argv[3];
	s = s.substr(0, s.find('.')) + s.substr(s.find('.')+1, s.find('.')+1);
	sstr << Helper::getPaddedNumber(atoi(argv[0]), 4) << Helper::getPaddedNumber(atoi(argv[1]), 6) << Helper::getPaddedNumber(atoi(argv[2]), 6) << Helper::getPaddedNumber(atoi(s.c_str()), 4);
	scores.push_back(sstr.str());

	return 0;
}
