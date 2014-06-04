/*
 * User.cpp
 *
 *  Created on: May 23, 2014
 *      Author: user
 */

#include "User.h"

User::User(string username, string password, TCPSocket* socket) {
	this->username = username;
	this->password = password;
	this->socket = socket;
	this->connected = true;
	this->inSession=false;

}

User::~User() {

}

void User::disconnect(void) {
	if(connected){
		this->connected = false;
	}
	if(socket!=NULL){
		this->socket->cclose();
	}
}
void User::connect(void) {
	connected = true;
}
void User::closeSession(){
	inSession=false;
}

string User::getIPandPort(){
	string str= this->socket->fromAddr();
	str.append(" ");
	str.append(this->udpPort);
	return str;
}
void User::setUdpPort(string port){
	this->udpPort = port;
}
string User::getUdpPort(){
	return this->udpPort;
}
