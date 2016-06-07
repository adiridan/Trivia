#pragma once
#include "sqlit3.h"
#include <vector>
#include "Question.h"

class DataBase
{
	DataBase();
	~DataBase();
	bool isUserExist(string username);
};