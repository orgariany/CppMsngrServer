/*
 * SessionMannager.cpp
 *
 *  Created on: May 27, 2014
 *      Author: user
 */

#include "Session.h"

Session::Session(User *user1,User *user2){
	this->user1=user1;
	this->user2=user2;

}
Session::~Session() {
	// TODO Auto-generated destructor stub
}

bool Session::inThisSession(User *user){
	if(user1 == user){
		return true;
	}else if(user2 == user){
		return true;
	}else{
		return false;
	}
}
string Session::getConnection(){
	string msg="["+user1->username+":"+user2->username+"]";
	return msg;
}
User *Session::getUser1(){
	return user1;
}
User *Session::getUser2(){
	return user2;
}
