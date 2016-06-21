#include "Game.h"

Game::Game(const vector<User*>& players, int questionsNo, DataBase& db) : _db(db)
{
	_id = _db.insertNewGame();
	if (_id != -1)
	{
		_questions = _db.initQuestions(questionsNo);
	}
	else
	{
		throw exception("ERROR: failed inserting new game");
	}

	_players = players;

	for (int i = 0; i < _players.size(); i++)
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
	_results.clear();
}

void Game::sendQuestionToAllUsers()
{
	stringstream msg;
	string que = _questions[_currQuestionIndex]->getQuestion(), *ans = _questions[_currQuestionIndex]->getAnswers();
	_currentTurnAnswers = 0;

	
	if (que.length() > 0)
	{
		msg << SERVER_SEND_QUESTION << Helper::getPaddedNumber(que.length(), 3) << que << Helper::getPaddedNumber(ans[0].length(), 3) << ans[0] << Helper::getPaddedNumber(ans[1].length(), 3) << ans[1] << Helper::getPaddedNumber(ans[1].length(), 3) << ans[2] << Helper::getPaddedNumber(ans[3].length(), 3) << ans[3];
		for (int i = 0; i < _players.size(); i++)
		{
			try
			{
				_players[i]->send(msg.str());
			}
			catch (exception e)
			{
				cout << e.what() << endl;
			}
		}
	}
	else if(_players.size() > 0)
		///failure: sending failure message to the admin
		_players[0]->getRoom()->getUsers()[0]->send(SERVER_SEND_QUESTION + '0');
}

void Game::handleFinishGame()
{
//	_db.updateGameStatus(getID());

	///בניית הודעה
	stringstream msg;
	int i;

	msg << SERVER_END_GAME << _players.size();

	for (i = 0; i < _players.size(); i++)
		msg << Helper::getPaddedNumber(_players[i]->getUsername().length(), 2) << _players[i]->getUsername() << Helper::getPaddedNumber(_results[_players[i]->getUsername()],2);
	
	///שליחת הודעה
	for (i = 0; i < _players.size(); i++)
	{
		try
		{
			_players[i]->clearGame();
			_players[i]->send(msg.str());
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
	bool re = false;
	if (_players.size() > 0)
	{
		if (_currentTurnAnswers == _players.size())
		{
			if (_question_no != _questions.size())
			{
				_question_no++;
				sendQuestionToAllUsers();
				re = true;
			}
			else
				handleFinishGame();
		}

	}
	else
		handleFinishGame();
	return re;
}

bool Game::handleAnswerFromUser(User* user, int answerNo, int time)
{
	bool isCorrect = false;
	stringstream msg;
	
	_currentTurnAnswers++;
	msg << SERVER_ANSWER;
	
	///בודק אם התשובה נכונה
	if (answerNo == _questions[_currQuestionIndex]->getCorrectAnswerIndex())
	{
		_results[user->getUsername()] = _results[user->getUsername()] + 1;
		isCorrect = true;
	}

	                                                                                             ///מוחזרת מחרוזת ריקה, אחרת מוחזרת התשובה שבחרת השחקן answerNo = 5 אם
	if (!_db.addAnswerToPlayer(_id, user->getUsername(), _questions[_currQuestionIndex]->getID(), (answerNo == 5 ? "" : _questions[_currQuestionIndex]->getAnswers()[answerNo]), isCorrect, time))
		cout << "addAnswerToPlayer() failed" << endl;
	

	msg << (isCorrect ? 1 : 0);
	user->send(msg.str());
	
	return handleNextTurn();///בודק אם המשחק נגמר או לא
}

bool Game::leaveGame(User* currUser)
{
	vector<User*>::iterator i;
	///או ברגע שהשחקן ימצא _players הלולאה תסתיים בסוף
	for (i = _players.begin(); i < _players.end() && *i != currUser; i++);
	_players.erase(i);
	return handleNextTurn();
}