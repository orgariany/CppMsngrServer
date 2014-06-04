//============================================================================
// Name        : TCPMessengerServer
// Author      : Eliav Menachi
// Version     :
// Copyright   :
// Description :
//============================================================================

#include <iostream>
#include "TCPMessengerServer.h"
#include "User.h"

using namespace std;

void printInstructions(){
	cout<<"lu - lists all users"<<endl;
	cout<<"lcu - list all connected users"<<endl;
	cout<<"ls - list all sessions (two clients communicating)"<<endl;
	cout<<"lr - list all rooms"<<endl;
	cout<<"lru <room name> - list all users in <room name>"<<endl;
	cout<<"x - shutdown"<<endl;
}
int main(){
	cout<<"Welcome to TCP messenger Server"<<endl;
	printInstructions();
	TCPMessengerServer msngrServer;
	msngrServer.start();
	bool loop = true;
	while(loop){
		string msg;
		string command;
		cin >> command;
		if(command == "lu"){
			cout<<"list of all users available:"<<endl;
			cout<<msngrServer.getUsersName()<<endl;
		}else if(command == "lcu"){
			cout<<"list of all connected users:"<<endl;
			cout<<msngrServer.getConnectedUsersName()<<endl;
		}else if(command == "ls"){
			cout<<"list of all sessions"<<endl;
			cout<<msngrServer.getAllSessions()<<endl;
		}else if(command == "lr"){
			cout<<"list of all rooms"<<endl;
			cout<<msngrServer.getAllRoomsNames()<<endl;
		}else if(command == "lru"){
			string roomName;
			cin>>roomName;
			cout<<"list of all users in: "<<roomName<<endl;
			cout<<msngrServer.getUsersInRoom(roomName)<<endl;
		}else if(command == "x"){
			loop = false;
		}else{
			cout<<"wrong input"<<endl;
			printInstructions();
		}
	}
	msngrServer.close();
	msngrServer.waitForThread();
	cout<<"messenger was closed"<<endl;
	return 0;
}

/** TO DO LIST:
 * Unconnect use when dc
 * Create - HeartBit.
 *
 * Bugs server:
 *
 *
 * Bugs client:
 */
