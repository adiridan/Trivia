#include "TriviaServer.h"

static const unsigned int IFACE=0;

TriviaServer::TriviaServer()
{
	//_db.DataBase();
	stringstream err;
	WSADATA wsa_data = {};
	try
	{
		if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0)
		{
			err << "WSAStartup error:" << WSAGetLastError();
			throw std::exception(err.str().c_str());
		}
		_socket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (_socket < 0)
		{
			err << "ERROR opening socket:" << WSAGetLastError();
			throw std::exception(err.str().c_str());
		}
		TRACE("starting");
	}
	catch (exception exc1)
	{
		cout << exc1.what() << endl;
	}
}

TriviaServer::~TriviaServer()
{
	closesocket(_socket);
	WSACleanup();
	_roomsList.clear();
	_connectedUsers.clear();
}

void TriviaServer::server()
{
	try
	{
		bindAndListen();
	}
	catch (exception e)
	{
		cout << e.what() << endl;
	}
	std::thread t1(&TriviaServer::handleRecivedMessages,this);

	t1.detach();

	while (true)
	{
		TRACE("accepting client...");
		TriviaServer::accept();
	}
}

void TriviaServer::bindAndListen()
{
	stringstream err;
	struct  sockaddr_in sa = { 0 };
	sa.sin_port = htons(PORT);
	sa.sin_family = AF_INET;
	sa.sin_addr.s_addr = IFACE;
	
	if (::bind(_socket, (struct sockaddr*)&sa, sizeof(sa)) == -1)
	{
		err << "bind error:" << WSAGetLastError();
		throw std::exception(err.str().c_str());
	}
	TRACE("bind");
	
	if (::listen(_socket, SOMAXCONN) == -1)
	{
		err << "listen error:" << WSAGetLastError();
		throw std::exception(err.str().c_str());
	}
	TRACE("listening on port %d...", PORT);
}

void TriviaServer::accept()
{
	 SOCKET AcceptSocket = ::accept(_socket, NULL, NULL);
	 
	 if (AcceptSocket == INVALID_SOCKET)
	 {
		 TRACE("error accepting client");
	 }
	 else
	 {
		 TRACE("client accepted. client's socket- %d", AcceptSocket);
		 std::thread t2(&TriviaServer::clientHandler, this, AcceptSocket);
		 t2.detach();
	 }
}

void TriviaServer::clientHandler(SOCKET client_socket)
{
	int msg_code = Helper::getMessageTypeCode(client_socket);
	try
	{
		while (msg_code != 0 || msg_code != CLIENT_EXIT)
		{
			addRecievedMessage(buildRecieveMessage(client_socket, msg_code));
			msg_code = Helper::getMessageTypeCode(client_socket);
		}

		addRecievedMessage(buildRecieveMessage(client_socket, CLIENT_EXIT));
	}
	catch (exception exc3)
	{
		cout << exc3.what() << '\n';
		addRecievedMessage(buildRecieveMessage(client_socket, CLIENT_EXIT));
	}
}

void TriviaServer::addRecievedMessage(RecievedMessage* msg)
{
	lock_guard<std::mutex> lck(_mtxRecievedMessages);
	_queRcvMessages.push(msg);
	cv.notify_all();
}

RecievedMessage* TriviaServer::buildRecieveMessage(SOCKET client_socket, int msgCode)
{
	std::vector<string> values;

	if (msgCode == CLIENT_SIGN_IN)
	{
		//userName
		values.push_back(Helper::getStringPartFromSocket(client_socket, Helper::getIntPartFromSocket(client_socket, 2)));
		//password
		values.push_back(Helper::getStringPartFromSocket(client_socket, Helper::getIntPartFromSocket(client_socket, 2)));
	}
	else if (msgCode == CLIENT_SIGN_UP)
	{
		//userName
		values.push_back(Helper::getStringPartFromSocket(client_socket, Helper::getIntPartFromSocket(client_socket, 2)));
		//password
		values.push_back(Helper::getStringPartFromSocket(client_socket, Helper::getIntPartFromSocket(client_socket, 2)));
		//email
		values.push_back(Helper::getStringPartFromSocket(client_socket, Helper::getIntPartFromSocket(client_socket, 2)));
	}
	else if (msgCode == CLIENT_USERS_IN_ROOM)
		//roomID
		values.push_back(Helper::getStringPartFromSocket(client_socket, 4));
	
	else if (msgCode == CLIENT_JOIN_EXISTING_ROOM)
		//roomID
		values.push_back(Helper::getStringPartFromSocket(client_socket, 4));
	
	else if (msgCode == CLIENT_CREAT_NEW_ROOM)
	{
		//roomName
		values.push_back(Helper::getStringPartFromSocket(client_socket, Helper::getIntPartFromSocket(client_socket, 2)));
		//playersNumber 
		values.push_back(Helper::getStringPartFromSocket(client_socket, 1));
		//questionsNumber 
		values.push_back(Helper::getStringPartFromSocket(client_socket, 2));
		//questionTimeInSec
		values.push_back(Helper::getStringPartFromSocket(client_socket, 2));
	}
	else if (msgCode == CLIENT_ANSWER)
	{
		//answerNumber 
		values.push_back(Helper::getStringPartFromSocket(client_socket, 1));
		//TimeInSeconds
		values.push_back(Helper::getStringPartFromSocket(client_socket, 2));
	}

	return (new RecievedMessage(client_socket, msgCode, values));
}
//try & catch - איפה לשים את הטרוו?
void TriviaServer::handleRecivedMessages()
{
	User* u;
	RecievedMessage* msg;
	unique_lock<mutex> lck(_mtxRecievedMessages, std::defer_lock);
	while (true)
	{
		lck.lock();
		while (_queRcvMessages.empty())
		{
			cv.wait(lck);
		}
		msg = _queRcvMessages.front();
		_queRcvMessages.pop();
		lck.unlock();

		u = getUserBySocket(msg->getSock());
		msg->setUser(u);
		TRACE("handleRecivedMessages: msg code = %d, client_socket = %d", msg->getMessageCode(), msg->getSock());

		try
		{
			if (msg->getMessageCode() == CLIENT_SIGN_IN)
			{
				handleSignin(msg);
			}
			else if (msg->getMessageCode() == CLIENT_SIGN_UP)
			{
				handleSignup(msg);
			}
			else if (msg->getMessageCode() == CLIENT_SIGN_OUT)
			{
				handleSignout(msg);
			}
			else if (msg->getMessageCode() == CLIENT_LEAVE_GAME)
			{
				//handleLeaveGame(msg);
			}
			else if (msg->getMessageCode() == CLIENT_START_GAME)
			{
				//handleStartGame(msg);
			}
			else if (msg->getMessageCode() == CLIENT_ANSWER)
			{
				//handlePlayerAnswer(msg);
			}
			else if (msg->getMessageCode() == CLIENT_CREAT_NEW_ROOM)
			{
				handleCreateRoom(msg);
			}
			else if (msg->getMessageCode() == CLIENT_CLOSE_ROOM)
			{
				handleCloseRoom(msg);
			}
			else if (msg->getMessageCode() == CLIENT_JOIN_EXISTING_ROOM)
			{
				handleJoinRoom(msg);
			}
			else if (msg->getMessageCode() == CLIENT_LEAVE_ROOM)
			{
				handleLeaveRoom(msg);
			}
			else if (msg->getMessageCode() == CLIENT_USERS_IN_ROOM)
			{
				handleGetUsersInRoom(msg);
			}
			else if (msg->getMessageCode() == CLIENT_GET_EXIST_ROOMS)
			{
				handleGetRooms(msg);
			}
			else if (msg->getMessageCode() == CLIENT_BEST_SCORE)
			{
				//handleGetBestScores(msg);
			}
			else if (msg->getMessageCode() == CLIENT_STATUS)
			{
				//handleGetPersonalStatus(msg);
			}
			else
			{
				safeDeleteUesr(msg);
			}
		}
		catch (exception ex)
		{
			safeDeleteUesr(msg);
		}
	}
}

void TriviaServer::handleSignout(RecievedMessage* msg)
{
	if (msg->getUser())
	{
		_connectedUsers.erase(msg->getUser()->getSocket());
	}
	
	handleCloseRoom(msg);
	handleLeaveRoom(msg);
	//handleLeaveGame(msg);
}

void TriviaServer::safeDeleteUesr(RecievedMessage* msg)
{
	try
	{
		handleSignout(msg);
		if (msg->getUser() && closesocket(msg->getUser()->getSocket()) == SOCKET_ERROR)
		{
			throw exception("problem");
		}
	}
	catch (exception ex)
	{
		cout << ex.what() << endl;
	}


	
}
//לשחרר זכרון????
/*void TriviaServer::handleLeaveGame(RecievedMessage* msg)
{
	if (msg->getUser()->leaveGame() == true)
	{
		//לשחרר זכרון
	}
}
//handle leave game

//לבדוק
void TriviaServer::handleStartGame(RecievedMessage* msg)
{
	vector<User*> user;
	user.push_back(msg);

	Game g = new Game(user, _db);
}
//handle start game

void TriviaServer::handlePlayerAnswer(RecievedMessage* msg)
{
	Game g = msg->getUser()->getGame();

	if (g != NULL)
	{
		if (g->handleAnswerFromUser() == FALSE)
		{
			//לשחרר את המשחק מהזכרון
		}
	}
}*/
//player answer


bool TriviaServer::handleCreateRoom(RecievedMessage* msg)
{
	User* u = msg->getUser();
	vector <string> val = msg->getValues();

	if (u != nullptr && u->creatRoom(_roomldSequence + 1, val[0], atoi(val[1].c_str()), atoi(val[2].c_str()), atoi(val[3].c_str())))
	{
		_roomldSequence++;
		_roomsList.insert(pair<int, Room*>(_roomldSequence, u->getRoom()));
		return true;
	}
	return false;
}

bool TriviaServer::handleLeaveRoom(RecievedMessage* msg)
{
	if (msg->getUser())
	{
		if (msg->getUser()->getRoom())
		{
			msg->getUser()->leavRoom();
			return true;
		}
	}
	return false;
}

bool TriviaServer::handleJoinRoom(RecievedMessage* msg)
{
	User* u = msg->getUser();
	if (u)
	{
		Room* r = getRoomById(atoi(msg->getValues()[0].c_str()));
		if (r)
		{
			u->joinRoom(r);
			return true;
		}
		else
			u->send(SERVER_JOIN_ROOM_NOT_EXIST);
	}
	return false;
}

bool TriviaServer::handleCloseRoom(RecievedMessage* msg)
{
	if (msg->getUser() && msg->getUser()->getRoom())
	{
		int id = msg->getUser()->getRoom()->getId();
		if (msg->getUser()->closeRoom() != -1)
		{
			_roomsList.erase(id);
			return true;
		}
	}
	return false;
}

void TriviaServer::handleGetUsersInRoom(RecievedMessage* msg)
{
	User* u = msg->getUser();
	Room* r = getRoomById(atoi(msg->getValues()[0].c_str()));

	if (r != nullptr)
	{
		u->send(r->getUsersListMessage());
	}
}


void TriviaServer::handleGetRooms(RecievedMessage* msg)
{
	string Smsg = SERVER_GET_EXIST_ROOMS + Helper::getPaddedNumber(_roomsList.size(), 4);
	for ( auto it = _roomsList.begin(); it != _roomsList.end(); it++)
	{
		Smsg += Helper::getPaddedNumber(it->second->getId(), 4);
		Smsg += Helper::getPaddedNumber(it->second->getName().length(), 2);
		Smsg += it->second->getName();
	}
	msg->getUser()->send(Smsg);
}




User* TriviaServer::getUserBySocket(SOCKET client_socket)
{
	auto it = _connectedUsers.find(client_socket);


	if (it == _connectedUsers.end())
	{
		return nullptr;
	}
	return it->second;
}

User* TriviaServer::getUserByName(string username)
{
	auto it = _connectedUsers.begin();
	if (it == _connectedUsers.end())
	{
		return nullptr;
	}
	if (it->second->getUsername() == username)
	{
		return it->second;
	}
	return nullptr;
}

Room* TriviaServer::getRoomById(int roomId)
{
	auto it = _roomsList.find(roomId);

	if (it == _roomsList.end())
	{
		return nullptr;
	}
	return it->second;
}


User* TriviaServer::handleSignin(RecievedMessage* msg)
{
	User* user = nullptr;
	/*
	WTF?!?!?!?!?!?!?!?!?!!?
	string username = *val.begin();
	val.pop_back();
	string password = *val.begin();
	*/

//	if (!isUserAndPassMatch(msg->getValues()[0], msg->getValues()[1])
	//	msg->getUser()->send(SERVER_SIGN_IN_WRONG_DETAILS);//protocol.h
	/*else*/
	if (getUserByName(msg->getValues()[0]) != NULL)
	{
		Helper::sendData(msg->getSock(), SERVER_SIGN_IN_ALLREADY_CONNECTAED);
	}
	else
	{
		user = new User(msg->getValues()[0], msg->getSock());
		_connectedUsers.insert(std::pair<SOCKET, User*>(msg->getSock(), user));
		user->send(SERVER_SIGN_IN_SUCCESS);
	}

	return user;
}


bool TriviaServer::handleSignup(RecievedMessage* msg)
{
	if (!Validator::isPasswordValid(msg->getValues()[1]))
	{
		Helper::sendData(msg->getSock(),SERVER_SIGN_UP_PASS_ILEGAL);
		return false;
	}
	else if (!Validator::isUsernameValid(msg->getValues()[0]))
	{
		Helper::sendData(msg->getSock(), SERVER_SIGN_UP_USERNAME_ILEGAL);
		return false;
	}
	/*else if (DataBase::isUserExists())
	{
		Helper::sendData(msg->getSock(), SERVER_SIGN_UP_USERNAME_ALLREDY_EXIST);
	}
	else
	{
		if (DataBase::addNewUser(msg->getValues()[0],msg->getValues()[1],msg->getValues()[2]))
		{
			Helper::sendData(msg->getSock(), SERVER_SIGN_UP_SUCCESS);
			return true;
		}
		else
		{
			Helper::sendData(msg->getSock(), SERVER_SIGN_UP_OTHER);
		}
	}*/
	Helper::sendData(msg->getSock(), SERVER_SIGN_UP_SUCCESS);
	return true;
}