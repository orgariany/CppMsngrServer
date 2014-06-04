/*
 * TCPMessengerProtocol.h
 *
 *  Created on: Feb 13, 2013
 *      Author: efi
 */

#ifndef TCPMESSENGERPROTOCOL_H_
#define TCPMESSENGERPROTOCOL_H_

/**
 * TCP Messenger protocol:
 * all messages are of the format [Command 4 byte int]
 * and optionally data follows in the format [Data length 4 byte int][ Data ]
 */
#define MSNGR_PORT 3346
#define CLIENT_MSNGR_PORT 3347

#define CLOSE_SESSION_WITH_PEER 	1
#define OPEN_SESSION_WITH_PEER 		2
#define EXIT						3
#define SEND_MSG_TO_PEER			4
#define OPEN_SESSION				5
#define SESSION_REFUSED				6
#define SESSION_ESTABLISHED			7
#define PRINT_USER_LIST				8
#define PRINT_CONNECTED_USERS		9
#define PRINT_ALL_ROOMS				10
#define PRINT_USERS_IN_ROOM			11
#define CLIENT_STATUS				12  // 1 = connected 0 = disconnected
#define CREATE_NEW_ROOM				13	//expect to receive a new room name
#define CLOSE_ROOM					14	//expect to receive an existing room name
#define UPDATE_IP_LIST				15
#define SESSION_ESTABLISHED_WITH_SERVER 16

#define TEST_PEER_NAME "test"
#define SESSION_REFUSED_MSG "Connection to peer refused, peer might be busy or disconnected, try again later"



#endif /* TCPMESSENGERPROTOCOL_H_ */
