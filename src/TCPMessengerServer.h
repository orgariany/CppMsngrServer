//============================================================================
// Name        : TCP Messenger Server
// Author      : Eliav Menachi
// Version     :
// Copyright   :
// Description : TCP Messenger application
//============================================================================
#include <strings.h>
#include <map>
#include <vector>
#include "MThread.h"
#include "TCPSocket.h"
#include "MultipleTCPSocketsListener.h"
#include "TCPMessengerProtocol.h"
#include "User.h"
#include "Room.h"
#include "Session.h"

using namespace std;

class TCPMessengerServer;

/**
 * The dispatcher server reads incoming commands from open peers and performs the required operations
 */
class PeersRequestsDispatcher: public MThread{
	TCPMessengerServer* messenger;
public:
	/**
	 * constructor that receive a reference to the parent messenger server
	 */
	PeersRequestsDispatcher(TCPMessengerServer* mesgr);

	/**
	 * The Dispatcher main loop
	 */
	void run();
};

class TCPMessengerServer: public MThread{
	friend class PeersRequestsDispatcher;

	PeersRequestsDispatcher* dispatcher;
	TCPSocket* tcpServerSocket;
	bool running;

	typedef map<string, TCPSocket*> tOpenedPeers;
	tOpenedPeers openedPeers;
	tOpenedPeers busyPeers;

	vector<User> users;
	vector<Room> rooms;
	vector<Session> sessions;

	vector<Session>::iterator sessionIterator;
	vector<User>::iterator userIterator;
	vector<User*>::iterator iter;
	vector<Room>::iterator roomIterator;

public:
	/**
	 * Construct a TCP server socket
	 */
	TCPMessengerServer();

	/**
	 * the messenger server main loop- this loop waits for incoming clients connection,
	 * once a connection is established the new connection is added to the openedPeers
	 * on which the dispatcher handle further communication
	 */
	void run();

	/**
	 * print out the list of connected clients
	 */
	void listPeers();

	/**
	 * close the server
	 */
	void close();

	/**
	 * print all users
	 */
	vector<User> getUsers();

	/**
	 * returns client status: "Connected to: xyz"
	 */
	string getClientStatus(User *user);
	/**
	 * Returns a list of connected users.
	 */
	string getConnectedUsersName();
	/**
	 * Return a list of rooms.
	 */
	string getAllRoomsNames();
	/**
	 * Returns a list all users ever connected to the server.
	 */
	string getUsersName();
	/**
	 * Returns a list of users in #roomname.
	 */
	string getUsersInRoom(string roomname);
	/**
	 * Returns a list of all sessions on the server.
	 */
	string getAllSessions();
	/**
	 * updates the #room 's user list.
	 * updates all user in room's list of joined member.
	 */
	void joinChatRoom(User *user,Room* room);
	/**
	 * Removes user from room.
	 * Updates all user in room's list of left member.
	 */
	void deleteUserFromRoom(User *user);
	/**
	 * Request session between user1 and user2
	 */
	void openSession(User *user1,User* user2);
	/**
	 * Create a session and updates server's session list.
	 */
	void createNewSession(User *user1,User *user2);
	/**
	 * Removing a session from server's list
	 * Also updates other users of disconnected user.
	 */
	void deleteSessions(User *user);

	/**
	 * Closing a Sessions of #user
	 */
	void closeSession(User* user);

	/**
	 * finding User in users by username
	 */
	User *findUserByUsername(string username);
private:
	/**
	 * returns the open peers in a vector
	 */
	vector<TCPSocket*> getPeersVec();

	/**
	 * return the open peer that matches the given name (IP:port)
	 * returns NULL if there is no match to the given name
	 */
	TCPSocket* getAvailablePeerByName(string peerName);

	/**
	 * remove and delete the given peer
	 */
	void peerDisconnect(TCPSocket* peer);

	/**
	 * move the given peer from the open to the busy peers list
	 */
	void markPeerAsUnavailable(TCPSocket* peer);

	/**
	 * move the given peer from the busy to the open peers list
	 */
	void markPeerAsAvailable(TCPSocket* peer);

	/**
	 * read command from peer
	 */
	int readCommandFromPeer(TCPSocket* peer);

	/**
	 * read data from peer
	 */
	string readDataFromPeer(TCPSocket* peer);

	/**
	 * send command to peer
	 */
	void sendCommandToPeer(TCPSocket* peer, int command);

	/**
	 * send data to peer
	 */
	void sendDataToPeer(TCPSocket* peer, string msg);



	/**
	 * finding User in users by socket
	 */
	User *findUserBySocket(TCPSocket* socket);

	/**
	 * finding Room in rooms by name
	 */
	Room *findRoomByName(string name);
};
