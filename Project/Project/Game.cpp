#include "Game.h"

Game::Game(const vector<User*>& players, int questionsNo, DataBase& db) : _db(db)
{
	if (_db.insertNewGame())
	{
		_db.initQuestions(questionsNo);
	}
	else
	{
		throw exception("cant insert new game.");
	}

	int i;
	for (i = 0; i < players.size(); i++)
	{
		_players.push_back(players[i]);
	}

	for (i = 0; i < _players.size(); i++)
	{
		_results.insert(pair<string, int>(_players[i]->getUsername(), 0));
		_players[i]->setGame(this);
	}


	
}

Game::~Game()
{
	for (int i = 0; i < _questions.size(); i++)
	{
		delete _questions[i];
	}

	_questions.clear();
	_players.clear();
}

void Game::sendQuestionToAllUsers()
{
	string que = _questions[_currQuestionIndex]->getQuestion();
	string msg = "118" + Helper::getPaddedNumber(que.length(), 3) + que;
	string* ans = _questions[_currQuestionIndex]->getAnswers();
	msg += Helper::getPaddedNumber(ans[0].length(), 3) + ans[0];
	msg += Helper::getPaddedNumber(ans[1].length(), 3) + ans[1];
	msg += Helper::getPaddedNumber(ans[1].length(), 3) + ans[2];
	msg += Helper::getPaddedNumber(ans[3].length(), 3) + ans[3];

	_currentTurnAnswers = 0;

	for (int i = 0; i < _players.size(); i++)
	{
		try
		{
			_players[i]->send(msg);
		}
		catch (exception ex)
		{
			_players[i]->getRoom()->getAdmin()->send(SERVER_Problem_Send_Qustions);
		}
	}
}

void Game::handleFinishGame()
{
	_db.updateGameStatus(this/*??????????*/);

	//בניית הודעה
	char userNumber = _players.size()-1;//???????
	string msg = "121" + Helper::getPaddedNumber(1,1) + userNumber;
	int i;
	char score[5];
	for (i = 0; i < _players.size(); i++)
	{
		msg += Helper::getPaddedNumber(_players[i]->getUsername().length, 2) + _players[i]->getUsername();
		itoa(_results.at(_players[i]->getUsername()), score, 10);
		msg += Helper::getPaddedNumber(5, 2) + score;
	}
	
	//שליחת הודעה
	for (i = 0; i < _players.size(); i++)
	{
		try
		{
			_players[i]->setGame(nullptr);
			_players[i]->send(msg);
		}
		catch (exception ex)
		{
			cout << ex.what() << endl;
		}
	}
}

void Game::sendFirstQuestion()
{
	sendQuestionToAllUsers();
}

bool Game::handleNextTurn()
{
	if (_players.size() > 0)
	{
		while (_currentTurnAnswers != _players.size());

		if (_question_no != _questions.size())
		{
			_question_no++;
			sendQuestionToAllUsers();
			return true;
		}
		
	}

	handleFinishGame();
	return false;

}

bool Game::handleAnswerFromUser(User* user, int answerNo, int time)
{
	_currentTurnAnswers++;

	bool trueORfalse = false;
	string answer = "";
	string msg = "120" + Helper::getPaddedNumber(1, 1) + "0";
	
	//בודק אם התשובה נכונה
	if (answerNo == _questions[_currQuestionIndex]->getCorrectAnswerIndex())
	{
		_results.at(user->getUsername()) + 1;
		trueORfalse = true;

		msg = "120" + Helper::getPaddedNumber(1, 1) + "1";
	}

	//מכניס לאנסואר את התשובה
	if (answerNo != 5)
	{
		string* answers = _questions[_currQuestionIndex]->getAnswers();
		answer = answers[answerNo];
	}

	if (false == _db.addAnswerToPlayer(this->getID(), user->getUsername(), _currQuestionIndex/*?????*/, answer/*????*/, trueORfalse, time))
	{
		cout << "addAnswerToPlayer() failed" << endl;
	}

	user->send(msg);
	
	return handleNextTurn();//בודק אם המשחק נגמר או לא
}

bool Game::leaveGame(User* currUser)
{
	vector<User*>::iterator i;

	for ( i = _players.begin; i < _players.end; i++)
	{
		if (*i == currUser)
		{
			_players.erase(i);
		}
	}

	return handleNextTurn();
}