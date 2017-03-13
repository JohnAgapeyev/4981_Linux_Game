#include <stdio.h>
#include <iostream>
#include <string.h>
#include <string>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <thread>
#include <errno.h>
#include <sys/epoll.h>
#include <string>
#include <iterator>

#include "NetworkManager.h"
#include "../packetizer.h"

using namespace std;

NetworkManager& NetworkManager::instance() {
    static NetworkManager sInstance;
    return sInstance;
}

/*--------------------------------------------------------------------------
METHOD: run

DATE: February. 1, 2017

REVISIONS:
Version, Date and Description

DESIGNER: Brody McCrone

PROGRAMMER: Brody McCrone

INTERFACE:  void run (const char *ip, const char *username)
ip: IP of the server.
username: Client's username.

RETURNS:
void

NOTES:
Performs the first part of the client-server handshake. i.e Sends the server
the client's username, receives the integer id assigned to the client, and
then received the integer ids and usernames of other clients connecting to
the server.
--------------------------------------------------------------------------*/
void NetworkManager::handshake(const char *ip, const char *username) {

    TCPConnect(ip);
    char sendline[STD_BUFFSIZE] = {0};
    int bytesToSend;

    //packetize the username first,
    bytesToSend = Packetizer::packControlMsg(sendline, STD_BUFFSIZE, username);
    //send the username to server.
    writeTCPSocket(sendline, bytesToSend);

    //create UDPSocket
    _sockUDP = UDPSocket(ip);
    //std::shared_ptr<UDPSocket> udpSock = std::make_shared<UDPSocket>(ip);
    //std::thread t(&NetworkManager::runUDPClient, this, udpSock);
    //t.detach();
}

void NetworkManager::closeConnection()
{
    close(_sockTCP);
}

void NetworkManager::runUDPClient(std::shared_ptr<UDPSocket> udpSock) {
    char buffer[SYNC_PACKET_MAX];
    int packetSize;
    //Packetizer packetizer;
    for(;;) {
        packetSize = udpSock.get()->recvFromServ(buffer, SYNC_PACKET_MAX);
        Packetizer::parseGameSync(buffer, packetSize);
    }
}

/*--------------------------------------------------------------------------
-- FUNCTION: writeTCPSocket
--
-- DATE: FEB. 01, 2017
--
-- REVISIONS:
-- Version 1.0 - [EY] - 2016/FEB/01 - Created Function
--
-- DESIGNER: Brody Mccrone
--
-- PROGRAMMER: Eva Yu
--
-- INTERFACE: int writeTCPSocket(const char * msg, int len)
-- const char * -- the message to write to the TCP socket
-- int len -- the length of the message to write
--
-- RETURNS:
--  0 represents excution success
-- -1 represents failure
--
-- NOTES:
-- This function loops the writing to the TCP Socket
--------------------------------------------------------------------------*/
int NetworkManager::writeTCPSocket(const char * msg, int len) {
    int res = 0;
    int ttlsent = 0, bytesleft = len;
    while (ttlsent < len) {
        if ((res = send(_sockTCP, msg + ttlsent, bytesleft, 0)) < 0 ) {
            perror("TCP write error");
            return res;
        }
        ttlsent += res;
        bytesleft -= res;
    }
    return 0;
}

/*------------------------------------------------------------------------------
METHOD: readTCPSocket

DATE: February. 1, 2017

REVISIONS:
Version, Date and Description

DESIGNER: Eva Yu

PROGRAMMER: Brody McCrone

INTERFACE: int readTCPSocket (char * buf, int len)
buf: Buffer to store read result in.
len: Amount of characters to read, can be no more than buffer size.

RETURNS:
-1: error (check errno for further info)
0: connection was closed.
len: read was successful.

NOTES:
This read method reads the amount specified by the param len from the TCP
socket stored as a private member of the Client object.
--------------------------------------------------------------------------*/
int NetworkManager::readTCPSocket(char *buf, int len) {
    int res = 0;
    if ( (res = read(_sockTCP, buf, len) ) < 0 ) {
        perror("read");
        return res;
    }
    return res;
}

/*--------------------------------------------------------------------------
-- FUNCTION: TCPConnect
--
-- DATE: FEB. 01, 2017
--
-- REVISIONS:
-- Version 1.0 - [EY] - 2016/FEB/01 - Created Function
--
-- DESIGNER: Eva Yu
--
-- PROGRAMMER: Eva Yu
--
-- INTERFACE: int TCPConnect (const char *)
-- const char * The ip_address
--
-- RETURNS:
-- int representing state : -1 is failure ; 0 is success
--
-- NOTES:
-- This is afunction that connects the user with via TCP
--------------------------------------------------------------------------*/
int NetworkManager::TCPConnect(const char * ip_addr) {
    if (ip_addr == NULL || inet_addr(ip_addr) == 0 ) {
        std::cerr << "Missing or Incorrect IP addr."
        << "\n IP Address must be in the form x.x.x.x"
        << std::endl;
        return -1;
    }

    struct sockaddr_in serv_addr;
    //set server addr struct
    memset(&serv_addr, '0', sizeof(struct sockaddr_in));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(TCP_PORT);
    serv_addr.sin_addr.s_addr = inet_addr(ip_addr);

    if ((_sockTCP = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cerr << "Error opening socket" << std::endl;
        exit(1);
    }

    if((connect(_sockTCP, (struct sockaddr *)&serv_addr,
    sizeof(struct sockaddr_in))) < 0) {
        perror("Error Connecting");
        exit(1);
    }

    return 0;
}

//Ey:- Mar 13 17- inserts a player in a map of all players of the game
void NetworkManager::insertplayer(int32_t id, const char * username)
{
    string stdStrUsername(username);
    insertplayer(id,stdStrUsername);
}

//Ey:- Mar 13 17- inserts a player in a map of all players of the game
//* Overlaoded function
void NetworkManager::insertplayer(int32_t id, string & username)
{
    _players.insert( std::pair<int,string>(id,username) );
}

//Ey:- Mar 13 17- retrieves the user name given a
const std::string & NetworkManager::getNameFromId(int32_t id)
{
    return (_players.find(id)->second);
}
