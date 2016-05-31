#pragma once
#include <string>

#define DIGITS "0123456789"
#define UPPER_CASE "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
#define LOWER_CASE "abcdefghijklmnopqrstuvwxyz"

using namespace std;
class Validator
{
public:
	static bool isPasswordValid(string password);
	static bool isUsernameValid(string username);
};