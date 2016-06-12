#include "Validator.h"

bool Validator::isPasswordValid(string password)
{
	bool re = password.length() >= 4;
	if (re)
		re = password.find(' ') == string::npos;
	if (re)
		re = password.find_first_of(DIGITS) != string::npos;
	if (re)
		re = password.find_first_of(UPPER_CASE) != string::npos;
	if (re)
		re = password.find_first_of(LOWER_CASE) != string::npos;
	return re;
}

bool Validator::isUsernameValid(string username)
{
	bool re = username.length() > 0;
	if (re)
		re = username.find(' ') == string::npos;
	if (re)
		re = (('z' >= username[0] && username[0] >= 'a') || ('Z' >= username[0] && username[0] >= 'A'));
	return re;
}
