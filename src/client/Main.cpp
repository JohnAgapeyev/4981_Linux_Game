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
int32_t myid;


void sigHandler(int sig);
int inline getRandTime();


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

    NetworkManager netMan = NetworkManager::instance();
    // start Networkmanager!
    netMan.run(argv[1], argv[2]);


}

void sigHandler(int sig)
{
    if (sig == SIGINT)
    {
        connected = false;
        running = false;
    }
    raise(SIGKILL);
}

// mimc player movement
int inline getRandTime()
{
  mt19937_64 eng{random_device{}()};
  uniform_int_distribution<> dist{RAN_MIN, RAN_MAX};
  return (dist(eng));
}
