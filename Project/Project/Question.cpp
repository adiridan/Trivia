#include "Question.h"

Question::Question(int id, string question, string correctAnswer, string answer2, string answer3, string answer4) : _id(id), _question(question)
{
	vector<string> v;
	v.push_back(correctAnswer);
	v.push_back(answer2);
	v.push_back(answer3);
	v.push_back(answer4);
	random_shuffle(v.begin(), v.end());
	for (unsigned int i = 0;i < v.size();i++)
	{
		_answers[i] = v[i];
		if (_answers[i] == correctAnswer)
			_correctAnswerIndex = i;
	}
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
