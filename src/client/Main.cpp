//TESTER FOR UDP

#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <signal.h>

#include <thread>
#include <mutex>
#include <random>
#include <chrono>
#include <map>

#include "NetworkManager.h"
#include "../UDPHeaders.h"
#include "../packetizer.h"

#define BUFFSIZE 65536
#define RAN_MIN 0
#define RAN_MAX 500 //max wait time in ms before action is produced
#define STDIN 0

static bool running = false;
static bool connected = false;
unsigned int myid;


void sigHandler(int sig);
int inline getRandTime();
void sendAction(UDPSocket &sock);
void sendMove(UDPSocket &sock);
void recvUDP(UDPSocket &sock);

using namespace std;

int main (int argc, char * argv[])
{
    // prompt usage
    if(argc < 3)
    {
      cerr << "Usage: " << argv[0] << " <<ipaddr>>" << " <<username>>\n";
      exit(1);
    }

    //set signal handling
    if (signal(SIGINT, sigHandler) == SIG_ERR)
    {
        perror("Signal() error.");
        exit(1);
    }

    // start program!
    fd_set readSet, initSet;
    NetworkManager netMan = NetworkManager::instance();
    UDPSocket udpsock;
    char buffrecv[BUFFSIZE], buffsend[BUFFSIZE], *precvBuff;
    thread udpRecvThread;
    int32_t playerid, bRead;
    int tcpsock;

    netMan.handshake(argv[1], argv[2]);
    tcpsock = netMan.getSockTCP();
    connected = true;
    udpsock = netMan.getSockUDP();

    bRead = netMan.readTCPSocket(buffrecv, BUFFSIZE);
    { // semantically separated to signify parseControlMsg
        myid = *(reinterpret_cast<const int32_t *>(buffrecv));
        buffrecv[bRead] = '\0';
        precvBuff = buffrecv + 6;
    }
    netMan.insertplayer(myid,precvBuff);
    // start UDP thread
    udpRecvThread = thread([&]{recvUDP(udpsock);});

    FD_ZERO(&initSet);
    FD_SET(STDIN, &initSet);
    FD_SET(tcpsock, &initSet);

    do {
        readSet = initSet;

        if(select(tcpsock + 1, &readSet, NULL, NULL, NULL) < 0) {
            perror("select");
            exit(1);
        }

        if(FD_ISSET(STDIN, &readSet)) {
            int bytesToSend;
            string str;
            getline(cin, str);
            bytesToSend = Packetizer::packControlMsg(buffsend, STD_BUFFSIZE, str.c_str());
            netMan.writeTCPSocket(buffsend, bytesToSend);
        }

        if(FD_ISSET(tcpsock, &readSet)) {
            if ((bRead = netMan.readTCPSocket(buffrecv, BUFFSIZE)) == 0 ) {
                perror("Connection Closed");
                connected = false;
                break;
            }

            if(!running)
            {
                { // semantically separated to signify parseControlMsg
                    playerid = *(reinterpret_cast<const int32_t *>(buffrecv));
                    buffrecv[bRead] = '\0';
                    precvBuff = buffrecv + 6;
                }
                netMan.insertplayer(myid,precvBuff);
            }
            else{ // if game is going, must be player message
                { // semantically separated to signify parseControlMsg
                    playerid = *(reinterpret_cast<const int32_t *>(buffrecv));
                    buffrecv[bRead] = '\0';
                    precvBuff = buffrecv + 6;
                }
                cout << "Msg From " << netMan.getNameFromId(playerid)
                     << ": " << precvBuff << endl;
            }
        }

    } while(connected);
    udpRecvThread.join();
}

void sigHandler(int sig)
{
    if (sig == SIGINT)
    {
        running = false;
    }
}


void recvUDP(UDPSocket &sock)
{
    char buffrecv[BUFFSIZE];
    int bRead;
    thread tSendActions, tSendMoves;
    //first udp call will block until i recv something
    bRead = sock.recvFromServ(buffrecv, BUFFSIZE);
    cout << "\nRecevied Dgram. Bytes read: " << bRead;

    running = true;
    tSendActions = thread([&]{sendAction(sock);}); //thread to send actions
    tSendMoves = thread([&]{sendMove(sock);}); //thread to send moves
    while (running)
    {
      // read a game sync and print the size read
      bRead = sock.recvFromServ(buffrecv, BUFFSIZE);
      cout << "\nRecevied Dgram. Bytes read: " << bRead;
    }
    tSendActions.join();
    tSendMoves.join();
    cout << endl;
}

// mimc player action
void sendAction(UDPSocket &sock)
{
  AttackAction action;
  int     iCounter = 1;
  double  dCounter = 1.1;

  action.actionid = 0;
  action.weaponid = 0;
  while(running)
  {

    action.id = iCounter++;
    action.xpos = dCounter;
    action.ypos = dCounter;
    action.direction = dCounter;
    dCounter += 1.1;

    //unique_lock<mutex> lock(mtx);
    sock.sendToServ(reinterpret_cast<char *>(&action), sizeof(action));
    //unique_lock<mutex> unlock(mtx);

    //mimc random actions taken with random sleep
    this_thread::sleep_for(chrono::milliseconds(getRandTime()));
  }
}

// mimc player movement
void sendMove(UDPSocket &sock)
{
  MoveAction action;
  double  dCounter = 1.1;
  int iCounter = 1;
  while(running)
  {
    action.id = iCounter++;
    action.xpos = dCounter;
    action.ypos = dCounter;
    action.direction = dCounter;
    dCounter += 1.1;

    sock.sendToServ(reinterpret_cast<char *>(&action), sizeof(action));

    //mimc random actions taken with random sleep
    this_thread::sleep_for(chrono::milliseconds(getRandTime()));
  }
}

// mimc player movement
int inline getRandTime()
{
  mt19937_64 eng{random_device{}()};
  uniform_int_distribution<> dist{RAN_MIN, RAN_MAX};
  return (dist(eng));
}
