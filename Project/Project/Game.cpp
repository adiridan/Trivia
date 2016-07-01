#include "Game.h"

Game::Game(const vector<User*>& players, int questionsNo, DataBase& db) : _db(db), _currentTurnAnswers(0), _question_no(questionsNo), _currQuestionIndex(0)
{
	if (insertGameToDB())
	{
		initQuestionsFromDB();
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
		msg << SERVER_SEND_QUESTION << Helper::getPaddedNumber(que.length(), 3) << que << Helper::getPaddedNumber(ans[0].length(), 3) << ans[0] << Helper::getPaddedNumber(ans[1].length(), 3) << ans[1] << Helper::getPaddedNumber(ans[2].length(), 3) << ans[2] << Helper::getPaddedNumber(ans[3].length(), 3) << ans[3];
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
}

void Game::handleFinishGame()
{
	_db.updateGameStatus(_ID);

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
	bool re = true;
	if (_players.size() > 0)
	{
		if (_currentTurnAnswers == _players.size())
		{
			if ((++_currQuestionIndex) < _question_no)
				sendQuestionToAllUsers();
			else
			{
				re = false;
				handleFinishGame();
			}
		}
	}
	else
	{ 
		re = false;
		handleFinishGame();
	}
	return re;
}

bool Game::handleAnswerFromUser(User* user, int answerNo, int time)
{
	bool isCorrect = false;
	stringstream msg;
	
	_currentTurnAnswers++;
	msg << SERVER_ANSWER;
	
	///בודק אם התשובה נכונה
	if (--answerNo == _questions[_currQuestionIndex]->getCorrectAnswerIndex())
	{
		_results[user->getUsername()] = _results[user->getUsername()] + 1;
		isCorrect = true;
	}

	                                                                                               
	try
	{																							///מוחזרת מחרוזת ריקה, אחרת מוחזרת התשובה שבחר השחקן answerNo = 4 אם
		_db.addAnswerToPlayer(_ID, user->getUsername(), _questions[_currQuestionIndex]->getID(), (answerNo == 4 ? "" : _questions[_currQuestionIndex]->getAnswers()[answerNo]), isCorrect, time);
	}
	catch (exception e)
	{
		cout << e.what() << endl;
	}

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

int Game::getID()
{
	return _ID;
}

bool Game::insertGameToDB()
{
	_ID = _db.insertNewGame();
	return _ID != -1;
}

void Game::initQuestionsFromDB()
{
	_questions = _db.initQuestions(_question_no);
}