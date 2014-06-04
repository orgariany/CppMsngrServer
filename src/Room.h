/*
 * Room.h
 *
 *  Created on: May 24, 2014
 *      Author: user
 */

#ifndef ROOM_H_
#define ROOM_H_

#include "User.h"
#include <vector>

using namespace std;

class Room {
public:
	Room(string creator,string Roomname);
	virtual ~Room();
	string getRoomCreator(void);
	string roomName;
	void connect(User *user);
	string getiplist();

	bool userConnected(User *user);
	vector<User*> *getusers(void);
	string getConnectedUsers();
	bool deleteRoom(string username);
	bool removeUser(User *user);
private:
	vector<User*> users;
	string roomCreator;
};

#endif /* ROOM_H_ */
