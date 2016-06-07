#include "Question.h"

Question::Question(int id, string question, string correctAnswer, string answer2, string answer3, string answer4): _id(id), _question(question)
{
	srand(time(NULL));
	_correctAnswerIndex = rand() % 4;
	_answers[_correctAnswerIndex] = correctAnswer;
	_answers[(_correctAnswerIndex + 1) % 4] = answer3;
	_answers[(_correctAnswerIndex + 2) % 4] = answer2;
	_answers[(_correctAnswerIndex + 3) % 4] = answer4;
}

string Question::getQuestion()
{
	return _question;
}

string * Question::getAnswers()
{
	return _answers;
}

int Question::getCorrectAnswerIndex()
{
	return _correctAnswerIndex;
}

int Question::getID()
{
	return _id;
}
