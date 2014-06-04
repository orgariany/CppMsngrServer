/*
 * User.h
 *
 *  Created on: May 23, 2014
 *      Author: user
 */

#ifndef USER_H_
#define USER_H_

#include <string>
#include <stdio.h>
#include "TCPSocket.h"
#include <vector>

using namespace std;

class User {
public:
	User(string username,string password,TCPSocket* socket);
	virtual ~User();
	void disconnect(void);
	void connect(void);
	void closeSession(void);
	string getIPandPort();
	string username;
	string password;
	TCPSocket* socket;
	string room;
	bool connected;
	bool inSession;
	void setUdpPort(string port);
	string getUdpPort();
private:
	string udpPort;
};

#endif /* USER_H_ */
