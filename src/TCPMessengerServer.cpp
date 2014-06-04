#include "TCPMessengerServer.h"
#include <netinet/in.h>
#include <iostream>
#include <iterator>
#include <string>
#include <utility>

//#include "TCPMessengerProtocol.h"

/***********************   PeersRequestsDispatcher implementation ******************************/
/**
 * The dispatcher server reads incoming commands from open peers and performs the required operations
 */
PeersRequestsDispatcher::PeersRequestsDispatcher(TCPMessengerServer* mesgr) {
	messenger = mesgr;
}

void PeersRequestsDispatcher::run() {
	cout << "dispatcher started" << endl;
	string msg;
	bool exist;
	while (messenger->running) {
		MultipleTCPSocketsListener msp;
		msp.addSockets(messenger->getPeersVec());
		TCPSocket* readyPeer = msp.listenToSocket(2);
		exist = false;
		if (readyPeer == NULL) {
			continue;
		}
		int command = messenger->readCommandFromPeer(readyPeer);
		string peerName, roomname, userName;
		vector<User> us;
		vector<Room>::iterator it;
		vector<User>::iterator userIterator;
		vector<User*>::iterator iter;
		User *user = NULL;
		Room *room;
		msg = "";
		switch (command) {
		case OPEN_SESSION_WITH_PEER:
			peerName = messenger->readDataFromPeer(readyPeer);
			if ((messenger->rooms.size() > 0)
					&& ((room = messenger->findRoomByName(peerName)) != NULL)) {
				messenger->joinChatRoom(messenger->findUserBySocket(readyPeer),
						room);
			} else if ((user = messenger->findUserByUsername(peerName)) != NULL) {
				messenger->openSession(messenger->findUserBySocket(readyPeer),
						user);
			} else {
				messenger->sendCommandToPeer(readyPeer, SESSION_REFUSED);
			}
			break;
		case CLOSE_SESSION_WITH_PEER:
			user = messenger->findUserBySocket(readyPeer);
			messenger->closeSession(user);
			break;
		case SESSION_ESTABLISHED:
			peerName = messenger->readDataFromPeer(readyPeer);
			user = messenger->findUserByUsername(peerName);

			messenger->sendCommandToPeer(readyPeer, UPDATE_IP_LIST);
			messenger->sendDataToPeer(readyPeer, user->getIPandPort());

			messenger->sendCommandToPeer(user->socket, SESSION_ESTABLISHED);
			messenger->sendCommandToPeer(user->socket, UPDATE_IP_LIST);
			messenger->sendDataToPeer(user->socket,
					messenger->findUserBySocket(readyPeer)->getIPandPort());

			user->inSession = true;
			messenger->findUserBySocket(readyPeer)->inSession = true;
			messenger->createNewSession(messenger->findUserBySocket(readyPeer),
					user);

			break;
		case PRINT_USER_LIST:
			msg = messenger->getUsersName();
			messenger->sendCommandToPeer(readyPeer, PRINT_USER_LIST);
			messenger->sendDataToPeer(readyPeer, msg);
			break;
		case PRINT_CONNECTED_USERS:
			msg = messenger->getConnectedUsersName();
			messenger->sendCommandToPeer(readyPeer, PRINT_CONNECTED_USERS);
			messenger->sendDataToPeer(readyPeer, msg);
			break;
		case PRINT_ALL_ROOMS:
			msg = messenger->getAllRoomsNames();
			messenger->sendCommandToPeer(readyPeer, PRINT_ALL_ROOMS);
			messenger->sendDataToPeer(readyPeer, msg);
			break;

		case PRINT_USERS_IN_ROOM:
			roomname = messenger->readDataFromPeer(readyPeer);
			msg = messenger->getUsersInRoom(roomname);
			messenger->sendCommandToPeer(readyPeer, PRINT_USERS_IN_ROOM);
			messenger->sendDataToPeer(readyPeer, msg);
			break;
		case CLIENT_STATUS:
			userName = messenger->readDataFromPeer(readyPeer);
			msg = messenger->getClientStatus(messenger->findUserByUsername(userName));
			messenger->sendCommandToPeer(readyPeer, CLIENT_STATUS);
			messenger->sendDataToPeer(readyPeer, msg);
			break;
		case EXIT:
			cout << "peer disconnected: " << readyPeer->destIpAndPort() << endl;
			messenger->peerDisconnect(readyPeer);
			user = messenger->findUserBySocket(readyPeer);
			messenger->closeSession(user);
			user->disconnect();

			break;
		case CREATE_NEW_ROOM:
			roomname = messenger->readDataFromPeer(readyPeer);
			user = messenger->findUserBySocket(readyPeer);
			room = messenger->findRoomByName(roomname);
			if (room == NULL) {
				room = new Room(user->username, roomname);
				messenger->rooms.push_back(*room);
				messenger->sendCommandToPeer(readyPeer, CREATE_NEW_ROOM);
				messenger->sendDataToPeer(readyPeer,
						"Room: " + roomname + " was created!");
			} else {
				messenger->sendCommandToPeer(readyPeer, CREATE_NEW_ROOM);
				messenger->sendDataToPeer(readyPeer,
						"Room: " + roomname + " already exists!");
			}
			break;
		case CLOSE_ROOM:
			roomname = messenger->readDataFromPeer(readyPeer);
			user = messenger->findUserBySocket(readyPeer);
			it = messenger->rooms.begin();
			for (; it != messenger->rooms.end(); ++it) {
				if (it->roomName == roomname) {
					exist = true;
					break;
				}
			}
			if (exist
					&& messenger->findRoomByName(roomname)->deleteRoom(
							user->username)) {
				messenger->rooms.erase(it);
				messenger->sendCommandToPeer(readyPeer, CLOSE_ROOM);
				messenger->sendDataToPeer(readyPeer,
						"Room: " + roomname + " was deleted!");
			} else if (exist) {
				messenger->sendDataToPeer(readyPeer,
						"You're not it's creator!");
			} else {
				messenger->sendCommandToPeer(readyPeer, CLOSE_ROOM);
				messenger->sendDataToPeer(readyPeer, "Room name not found!");
			}
			break;
		default:
			messenger->sendDataToPeer(readyPeer,
					"Unknown command please try again");
			break;
		}
	}
	cout << "dispatcher ended" << endl;
}

/***********************   TCPMessengerServer implementation ******************************/
TCPMessengerServer::TCPMessengerServer() {
	tcpServerSocket = new TCPSocket(MSNGR_PORT);
	running = false;
	dispatcher = NULL;
}

void TCPMessengerServer::run() {
	running = true;
	dispatcher = new PeersRequestsDispatcher(this);
	dispatcher->start();
	while (running) {
		TCPSocket* peerSocket = tcpServerSocket->listenAndAccept();
		if (peerSocket != NULL) {
			this->sendDataToPeer(peerSocket,
					"Welcome to OandO Messenger\nPlease enter username:");
			string username = this->readDataFromPeer(peerSocket);
			string password;
			User *user = findUserByUsername(username);
			if (user != NULL) {
				this->sendDataToPeer(peerSocket, "password:");
				password = this->readDataFromPeer(peerSocket);
				if (password == user->password) {
					cout << "new connection established: "
							<< peerSocket->destIpAndPort() << endl;
					openedPeers[peerSocket->destIpAndPort()] = peerSocket;
					this->sendCommandToPeer(peerSocket,
					SESSION_ESTABLISHED_WITH_SERVER);
					user->setUdpPort(readDataFromPeer(peerSocket));
					user->connect();
					if (user->socket != NULL) {
						user->socket->cclose();
					}
					user->socket = peerSocket;
				} else {
					this->sendCommandToPeer(peerSocket, SESSION_REFUSED);
					peerSocket->cclose();
				}
			} else {
				this->sendDataToPeer(peerSocket,
						"Please enter password for username entered:");
				password = this->readDataFromPeer(peerSocket);
				user = new User(username, password, peerSocket);
				this->sendCommandToPeer(peerSocket,
						SESSION_ESTABLISHED_WITH_SERVER);
				user->setUdpPort(readDataFromPeer(peerSocket));
				users.push_back(*user);
				cout << "new connection established: "
						<< peerSocket->destIpAndPort() << endl;
				openedPeers[peerSocket->destIpAndPort()] = peerSocket;
			}
		}
	}
}

void TCPMessengerServer::listPeers() {
	tOpenedPeers::iterator iter = openedPeers.begin();
	tOpenedPeers::iterator endIter = openedPeers.end();
	for (; iter != endIter; iter++) {
		cout << (*iter).second->destIpAndPort() << endl;
	}
}

void TCPMessengerServer::close() {
	cout << "closing server" << endl;
	running = false;
	tcpServerSocket->cclose();

	if (users.size() > 0) {
		userIterator = users.begin();
		for (; userIterator != users.end(); ++userIterator) {
			this->sendCommandToPeer(userIterator->socket, EXIT);
		}
	}

	tOpenedPeers::iterator iter = openedPeers.begin();
	tOpenedPeers::iterator endIter = openedPeers.end();
	for (; iter != endIter; iter++) {
		((*iter).second)->cclose();
	}
	dispatcher->waitForThread();
	iter = openedPeers.begin();
	endIter = openedPeers.end();
	for (; iter != endIter; iter++) {
		delete (*iter).second;
	}
	cout << "server closed" << endl;
}

vector<TCPSocket*> TCPMessengerServer::getPeersVec() {
	vector<TCPSocket*> vec;
	tOpenedPeers::iterator iter = openedPeers.begin();
	tOpenedPeers::iterator endIter = openedPeers.end();
	for (; iter != endIter; iter++) {
		vec.push_back((*iter).second);
	}
	return vec;
}

TCPSocket* TCPMessengerServer::getAvailablePeerByName(string peerName) {
	tOpenedPeers::iterator iter = openedPeers.find(peerName);
	tOpenedPeers::iterator endIter = openedPeers.end();
	if (iter == endIter) {
		return NULL;
	}
	return (*iter).second;
}

void TCPMessengerServer::peerDisconnect(TCPSocket* peer) {
	openedPeers.erase(peer->destIpAndPort());
	busyPeers.erase(peer->destIpAndPort());
}

void TCPMessengerServer::markPeerAsUnavailable(TCPSocket* peer) {
	openedPeers.erase(peer->destIpAndPort());
	busyPeers[peer->destIpAndPort()] = peer;
}

void TCPMessengerServer::markPeerAsAvailable(TCPSocket* peer) {
	busyPeers.erase(peer->destIpAndPort());
	openedPeers[peer->destIpAndPort()] = peer;
}

int TCPMessengerServer::readCommandFromPeer(TCPSocket* peer) {
	int command = 0;
	int rt = peer->recv((char*) &command, 4);
	if (rt < 1)
		return rt;
	command = ntohl(command);
	return command;
}

string TCPMessengerServer::readDataFromPeer(TCPSocket* peer) {
	string msg;
	char buff[1500];
	int msgLen;
	peer->recv((char*) &msgLen, 4);
	msgLen = ntohl(msgLen);
	int totalrc = 0;
	int rc;
	while (totalrc < msgLen) {
		rc = peer->recv((char*) &buff[totalrc], msgLen - totalrc);
		if (rc > 0) {
			totalrc += rc;
		} else {
			break;
		}
	}
	if (rc > 0 && totalrc == msgLen) {
		buff[msgLen] = 0;
		msg = buff;
	} else {
		peer->cclose();
	}
	return msg;
}

void TCPMessengerServer::sendCommandToPeer(TCPSocket* peer, int command) {
	command = htonl(command);
	peer->send((char*) &command, 4);
}

void TCPMessengerServer::sendDataToPeer(TCPSocket* peer, string msg) {
	int msgLen = msg.length();
	msgLen = htonl(msgLen);
	peer->send((char*) &msgLen, 4);
	peer->send(msg.data(), msg.length());
}
vector<User> TCPMessengerServer::getUsers() {
	return this->users;
}
string TCPMessengerServer::getConnectedUsersName() {
	string msg;
	User *availbleUsers[users.size()];
	int i = 0;
	if (users.size() > 0) {
		for (userIterator = users.begin(); userIterator != users.end();
				++userIterator) {
			if (userIterator->connected) {
				availbleUsers[i] = &(*userIterator);
				i++;
			}
		}
	}
	if (i == 0) {
		return "There is no connected Users";
	}
	for (int j = 0; j < i; j++) {
		msg.append("[" + availbleUsers[j]->username + "]");
	}
	return msg;
}
User *TCPMessengerServer::findUserByUsername(string username) {
	vector<User>::iterator it = users.begin();
	for (; it != users.end(); it++) {
		if (it->username == username) {
			return &(*it);
		}
	}
	return NULL;
}
User *TCPMessengerServer::findUserBySocket(TCPSocket* socket) {
	vector<User>::iterator it = users.begin();
	for (; it != users.end(); it++) {
		if (it->socket == socket) {
			return &(*it);
		}
	}
	return NULL;
}
Room *TCPMessengerServer::findRoomByName(string name) {
	if (rooms.size() > 0) {
		vector<Room>::iterator it = rooms.begin();
		for (; it != rooms.end(); ++it) {
			if (it->roomName == name) {
				return &(*it);
			}
		}
	}
	return NULL;
}
string TCPMessengerServer::getAllRoomsNames() {
	string msg = "";
	if (this->rooms.size() == 0) {
		return "No rooms available";
	}
	roomIterator = this->rooms.begin();
	for (; roomIterator != rooms.end(); roomIterator++) {
		msg += "[" + roomIterator->roomName + "]";
	}
	return msg;
}
string TCPMessengerServer::getUsersName() {
	string msg = "";
	if (users.size() == 0) {
		return "No Users found";
	}
	vector<User>::iterator userIter = this->users.begin();
	if (this->users.size() > 0) {
		msg += "[" + userIter->username + "]";
		userIter += 1;
		for (; userIter != this->users.end(); ++userIter) {
			msg += ",[" + userIter->username + "]";
		}
	}
	return msg;
}
string TCPMessengerServer::getUsersInRoom(string roomname) {
	string msg = "";
	Room* r = this->findRoomByName(roomname);
	if (r == NULL) {
		return "Room Not Found!";
	}
	msg = r->getConnectedUsers();
	return msg;
}
void TCPMessengerServer::deleteSessions(User *user) {
	bool userInRoom = false;
	for (int i = 0; i < rooms.size(); i++) {
		if (rooms[i].userConnected(user)) {
			userInRoom = true;
		}
	}
	if (!userInRoom) {
		for (int i = 0; i < sessions.size(); i++) {
			if (sessions[i].inThisSession(user)) {

				if (user == sessions[i].getUser1()) {

					sendCommandToPeer(sessions[i].getUser2()->socket,
					CLOSE_SESSION_WITH_PEER);

				} else {

					sendCommandToPeer(sessions[i].getUser1()->socket,
					CLOSE_SESSION_WITH_PEER);

				}
				sessionIterator = sessions.begin() + i;
				cout << "Session: " << sessionIterator->getConnection()
						<< " was deleted!" << endl;
				sessions.erase(sessionIterator);
			}
		}
	} else {
		this->deleteUserFromRoom(user);
		for (int i = 0; i < sessions.size(); ++i) {

			if (sessions[i].inThisSession(user)) {
				sessionIterator = sessions.begin() + i;
				cout << "Session: " << sessionIterator->getConnection()
						<< " was deleted!" << endl;
				sessionIterator = sessions.begin() + i;
				sessions.erase(sessionIterator);
			}
		}
	}
}
void TCPMessengerServer::openSession(User *user1, User* user2) {
	if (!user2->inSession) {
		sendCommandToPeer(user2->socket, OPEN_SESSION_WITH_PEER);
		sendDataToPeer(user2->socket, user1->username);
	} else {
		sendCommandToPeer(user1->socket, SESSION_REFUSED);
	}
}
string TCPMessengerServer::getAllSessions() {
	string msg = "";
	if (sessions.size() == 0) {
		return "No Sessions available";
	}
	sessionIterator = sessions.begin();
	for (; sessionIterator != sessions.end(); ++sessionIterator) {
		msg += sessionIterator->getConnection();
	}
	return msg;
}
string TCPMessengerServer::getClientStatus(User *user) {
	if (user == NULL) {
		return "User not found!";
	}
	if (!user->inSession) {
		return "User is available";
	}
	if (rooms.size() > 0) {
		roomIterator = rooms.begin();
		for (; roomIterator != rooms.end(); roomIterator++) {
			if (roomIterator->userConnected(user)) {
				return "User " + user->username + " is connected to: "
						+ roomIterator->roomName;
			}
		}
	}
	if (sessions.size() > 0) {
		sessionIterator = sessions.begin();
		for (; sessionIterator != sessions.end(); ++sessionIterator) {
			if (sessionIterator->inThisSession(user)) {
				if (user == sessionIterator->getUser1()) {
					return " User " + user->username + " is connected to: "
							+ sessionIterator->getUser2()->username;
				} else {
					return " User " + user->username + " is connected to: "
							+ sessionIterator->getUser1()->username;
				}
			}
		}
	}
	return NULL;
}
void TCPMessengerServer::closeSession(User* user) {
	this->deleteSessions(user);
	this->deleteUserFromRoom(user);
	user->closeSession();
}
void TCPMessengerServer::deleteUserFromRoom(User *user) {
	for (int i = 0; i < rooms.size(); ++i) {
		if (rooms[i].userConnected(user)) {
			rooms[i].removeUser(user);
			vector<User *> *usrs = rooms[i].getusers();
			for (int j = 0; j < usrs->size(); j++) {
				string msg = rooms[i].getiplist();
				sendCommandToPeer((*usrs)[j]->socket, UPDATE_IP_LIST);
				sendDataToPeer((*usrs)[j]->socket, msg);
			}
			break;
		}
	}
}
void TCPMessengerServer::joinChatRoom(User *user, Room* room) {
	sendCommandToPeer(user->socket, SESSION_ESTABLISHED);
	room->connect(user);
	string ipList = room->getiplist();
	vector<User *> *users = room->getusers();
	for (int i = 0; i < users->size(); i++) {
		sendCommandToPeer((*users)[i]->socket, UPDATE_IP_LIST);
		sendDataToPeer((*users)[i]->socket, ipList);
		if (user != (*users)[i]) {
			createNewSession(user, (*users)[i]);
		}
	}
}
void TCPMessengerServer::createNewSession(User *user1, User *user2) {
	sessions.push_back(Session(user1, user2));
}
