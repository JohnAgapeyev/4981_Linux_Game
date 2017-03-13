#ifndef NETWORKMANAGER_HPP
#define NETWORKMANAGER_HPP

#include "UDPSocket.h"
#include <string>
#include <stdio.h>
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <limits.h>
#include <atomic>
#include <memory>
#include <map>

#define TCP_PORT 		35223
#define UDP_PORT 		35222
#define UNAME_SIZE 	    32
#define PLAYERPACK_SIZE (UNAME_SIZE + 1)
#define CHAT_BUFFSIZE 	128
#define STD_BUFFSIZE    1024
#define MAX_EVENTS 		1
#define MAX_USERS		23
#define SYNC_PACKET_MAX USHRT_MAX
#define SYNC_PACKET


//moved enums to the bottom, and commented out

class NetworkManager {
public:
    static NetworkManager& instance();

    UDPSocket& getSockUDP() {return _sockUDP;};
    void handshake(const char * ip, const char * uname);
    void closeConnection();

    // EY - Mar 12: moved from private to public
    int writeTCPSocket(const char *, int);
    int readTCPSocket(char *, int);
    // EY -Mar 12 : added for select
    int getSockTCP(){return _sockTCP;};

    // EY: Mar 12: added for player id-username mapping
    void insertplayer(int32_t id, const char * username);
    void insertplayer(int32_t id, std::string & username);
    const std::string & getNameFromId(int32_t id);

private:
    int _sockTCP;
    UDPSocket _sockUDP;
    std::map<int, std::string> _players; // maps id to user name
    NetworkManager() {};

    void runUDPClient(std::shared_ptr<UDPSocket> udpSock);
    int TCPConnect(const char *);

};

#endif

/*
//ENUMS
#ifndef PACKET_SPECIFIER_ENUMCL
#define PACKET_SPECIFIER_ENUMCL

enum class P_SPECIFIER{
	  EXIT,
		UNAME,
		CHAT
};

#endif

#ifndef GAMESTATE_ENUMCL
#define GAMESTATE_ENUMCL

enum class GAMESTATE{
		GAME_RECV,
		AI_RECV
};
#endif
*/
