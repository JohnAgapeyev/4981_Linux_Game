#ifndef SERVER_H
#define SERVER_H

#include <cstdarg>
#include <limits.h>

//Temp variable to represent client count
#define CLIENT_COUNT 10

#define LISTEN_PORT_UDP 35222
#define LISTEN_PORT_TCP 35223
#define TICK_RATE 20
//we dont ever, ever want to have to resize. just max it
#define IN_PACKET_SIZE USHRT_MAX
#define OUT_PACKET_SIZE USHRT_MAX

#define SYNC_IN 32 //name padded with nulls
#define NAMELEN 32 //same as above but kept seperate for clarity of purpose
#define SYNC_OUT 33 //name padded with nulls + id
#define OPT_STRING "hl:L:c:v"
#define MAX_PORT 65535
#define LISTENQ 25 //although many kernals define it as 5 usually it can support many more

typedef struct Players{
    int x,y;
    float dx,dy;
    int id;
    char username[NAMELEN];
} Player;

typedef struct Clients{
    Player *player;
    struct sockaddr_in *addr;
} Client;

extern const long long microSecPerTick;
extern char outputPacket[];
extern int outputLength;
extern int listenSocketUDP;
extern int listenSocketTCP;
extern int sendSocketUDP;
extern int sendSocketTCP;
extern Client *clients;

void initSync(int sock);

void processPacket(const char *data);
void genOutputPacket();
void sendSyncPacket(int sock);
void listenForPackets(const struct sockaddr_in servaddr);
void alarmHandler(int signo);
void startTimer();
void listenTCP(int socket, unsigned long ip, unsigned short port);
void listenUDP(int socket, unsigned long ip, unsigned short port);
int createSocket(bool useUDP, bool nonblocking);

//where each client w/ both socket and user info is stored

//off by default
extern bool verbose;
//logging for statuses when in verbose mode
void logv(const char *msg, ...);
#endif
