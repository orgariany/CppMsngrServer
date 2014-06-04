/*
 * Session.h
 *
 *  Created on: May 27, 2014
 *      Author: user
 */

#ifndef SESSION_H_
#define SESSION_H_
#include  <vector>
#include "User.h"

using namespace std;

class Session {
	User *user1,*user2;
public:
	Session(User *user1,User *user2);
	virtual ~Session();
	bool inThisSession(User *user);
	string getConnection();
	User *getUser1();
	User *getUser2();
};

#endif /* SESSION_H_ */
