/*
 * Room.cpp
 *
 *  Created on: May 24, 2014
 *      Author: user
 */

#include "Room.h"

using namespace std;

Room::Room(string creator,string Roomname) {
	this->roomCreator = creator;
	this->roomName = Roomname;
}
Room::~Room() {
}
vector<User*> *Room::getusers(void){
	return &users;
}

bool Room::deleteRoom(string username){
	if(username==this->roomCreator){
		if(users.size()>0){
		vector<User *>::iterator it = users.begin();
		for(;it!=users.end();++it){
			(*it)->disconnect();
		}
		users.clear();
		}
		return true;
	}
	return false;
}
string Room::getRoomCreator(void){
	return roomCreator;
}

void Room::connect(User *user){
	users.push_back(user);
	user->inSession=true;
}
/**
 * updates the entire room
 */
string Room::getiplist(){
	string str="";

	if(users.size()>0){
		str.append(users[0]->getIPandPort());
		for(int i=1;i<users.size();++i){
			str.append(",");
			str.append(users[i]->getIPandPort());
		}
	}
	return str;
}
bool Room::userConnected(User *user){
	for(int i = 0; i<users.size();++i){
		if(user == users[i]){
			return true;
		}
	}
	return false;
}
bool Room::removeUser(User *user){
	bool exist=false;
	vector<User*>::iterator it = users.begin();
	for(; it !=users.end();++it){
		if(*it == user){
			exist = true;
			break;
		}
	}
	if(exist){
		users.erase(it);
	}
	return exist;
}
string Room::getConnectedUsers(){
	string str="";
	int size = users.size();
	if(size==0){
		return "there's no users in this room!";
	}
	for(int i=0;i<size;i++){
		str.append("[");
		str.append(users[i]->username);
		str.append("]");
	}
	return str;
}
