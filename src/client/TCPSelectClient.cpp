//brodie's select cpp test file
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <strings.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <limits.h>
#include <string.h>

#include "../packetizer.h"
#define TCP_PORT 35223	// Default port
#define STDIN 0


void parse(char *buffer, int len);
int writeTCPSocket(const char * msg, int len);
int readTCPSocket(char *buf, int len);
void TCPConnect(const char * ip_addr);

int _sockTCP = 0;

/*int main(void) {
    fd_set readSet, initSet;
    char buffer[USHRT_MAX];
    int bytesRead;

    TCPConnect("127.0.0.1");

    FD_ZERO(&initSet);
    FD_SET(STDIN, &initSet);
    FD_SET(_sockTCP, &initSet);
    for(;;) {
        readSet = initSet;
        if(select(_sockTCP + 1, &readSet, NULL, NULL, NULL) < 0) {
            perror("select");
            exit(1);
        }

        if(FD_ISSET(STDIN, &readSet)) {
            //get command
            fgets(buffer, USHRT_MAX, stdin);
            //replace new line with null
            buffer[strlen(buffer) - 1] = '\0';
            //send command
            writeTCPSocket(buffer, strlen(buffer));
        }

        if(FD_ISSET(_sockTCP, &readSet)) {
            if((bytesRead = readTCPSocket(buffer, USHRT_MAX)) == 0) {
                std::cout << "Server disconnected." << std::endl;
                exit(1);
            }
            Packetizer::parseControlMsg(buffer, bytesRead);
        }
    }
}
*/
// void parse(char *buffer, int len) {
//     //parse buffer for commands
//
//     std::cout << "Received Data: " << buffer << std::endl;
// }

int writeTCPSocket(const char * msg, int len) {
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


int readTCPSocket(char *buf, int len) {
	int res = 0;
	if ((res = read(_sockTCP, buf, len)) < 0 ) {
		perror("read");
		exit(1);
	}

	return res;
}

void TCPConnect(const char *ip_addr) {
	if (ip_addr == NULL || inet_addr(ip_addr) == 0 ) {
		std::cerr << "Missing or Incorrect IP addr."
			<< "\n IP Address must be in the form x.x.x.x"
			<< std::endl;
		exit(1);
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
}
