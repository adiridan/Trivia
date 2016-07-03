#include <iostream>
#include "TriviaServer.h"

int main()
{
	TriviaServer *server = nullptr;
	try
	{
		server = new TriviaServer();
	}
	catch (exception e)
	{
		cout << e.what() << endl;
	}
	if (server)
		server->server();
	system("pause");
	return 0;
}
