//TESTER FOR UDP

#include <iostream>
#include <arpa/inet.h>
#include <sys/socket.h>


#include <thread>
#include <mutex>
#include <random>
#include <chrono>

#include "UDPSocket.h"
#include "../UDPHeaders.h"

#define BUFFSIZE 6291456
#define RAN_MIN 0
#define RAN_MAX 300

static bool running = true;
std::mutex mtx;

int inline getRandTime();
void sendAction(UDPSocket &sock);
void sendMove(UDPSocket &sock);

using namespace std;

int main (int argc, char * argv[])
{
  char buff[BUFFSIZE];
  int bRead;

  if(argc < 2)
  {
    cerr << "Usage: " << argv[0] << " <ipaddr>\n";
    exit(1);
  }

  UDPSocket sock(argv[1]); // create UDP sock

  thread tSendActions([&]{sendAction(sock);}); //thread to send actions
  thread tSendMoves([&]{sendMove(sock);}); //thread to send moves

  while (running)
  {
    //unique_lock<mutex> lock(mtx);
    bRead = sock.recvFromServ(buff, BUFFSIZE);
    //unique_lock<mutex> unlock(mtx);
    cout << "\nRecevied Dgram! Bytes read: " << bRead;
  }

  cout << endl;
  tSendActions.join();
  tSendMoves.join();
}

// mimc player action
void sendAction(UDPSocket &sock)
{
  AttackAction action;
  int     iInd = 1;
  double  dInd = 1.1;

  action.actionid = 0;
  action.weaponid = 0;
  while(running)
  {
    action.id = iInd++;
    action.xpos = dInd;
    action.ypos = dInd;
    action.direction = dInd;
    dInd += 1.1;

    unique_lock<mutex> lock(mtx);
    sock.sendToServ(reinterpret_cast<char *>(&action), sizeof(action));
    unique_lock<mutex> unlock(mtx);

    //mimc random actions taken with random sleep
    this_thread::sleep_for(chrono::milliseconds(getRandTime()));
  }
}

// mimc player movement
void sendMove(UDPSocket &sock)
{
  MoveAction action;
  double  dInd = 1.1;
  int iInd = 1;
  while(running)
  {
    action.id = iInd++;
    action.xpos = dInd;
    action.ypos = dInd;
    action.direction = dInd;
    dInd += 1.1;

    unique_lock<mutex> lock(mtx);
    sock.sendToServ(reinterpret_cast<char *>(&action), sizeof(action));
    unique_lock<mutex> unlock(mtx);

    //mimc random actions taken with random sleep
    this_thread::sleep_for(chrono::milliseconds(getRandTime()));
  }
}

// mimc player movement
int inline getRandTime()
{
  mt19937_64 eng{random_device{}()};  // or seed however you want
  uniform_int_distribution<> dist{RAN_MIN, RAN_MAX};
  return (dist(eng));
}
