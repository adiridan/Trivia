#pragma once

#include <string>
#include <random>
#include <time.h>

using namespace std;

class Question
{
public:
	Question(int id, string question, string correctAnswer, string answer2, string answer3, string answer4);
	string getQuestion();
	string* getAnswers();
	int getCorrectAnswerIndex();
	int getID();

private:
	string _question, _answers[4];
	int _correctAnswerIndex, _id;
};