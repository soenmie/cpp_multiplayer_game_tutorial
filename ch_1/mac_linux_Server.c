#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAXCLIENTS 30
#define BUFFERSIZE 1024

void error(const char*);
int chatServer(void);

void error(const char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

int chatServer(void){
	int option = 1;
	int serverSocket;
	int addrlen;
	int currentSocket;
	int clientId[MAXCLIENTS] = {0};
	int activity;
	int index;
	int portnum = 12543;
	int valread;
	int clientFd;
	int tempFd;
	struct sockaddr_in serverAddr;
	char buffer[BUFFERSIZE];
	fd_set fds;
	char *greetingMessage = "Welcome to SimpleChat v1.0 \r\n";

	/* Initialize a brand new TCP socket and handle errors */
	if( (serverSocket = socket(AF_INET , SOCK_STREAM , 0)) == 0) {
		error("ERROR: socket() failed");
	}

	/*set master socket to allow multiple connections , this is just a good habit, it will work without this*/
	if( setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, (char *)&option, sizeof(option)) < 0 ) {
		error("ERROR: setsocketopt() failed");
	}

	/*type of socket created*/
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = INADDR_ANY;
	serverAddr.sin_port = htons(portnum);

	/*bind the socket to localhost port 12543*/
	if ( bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) <0 ) {
		error("ERROR: bind() failed");
	}

	/*try to specify maximum of 3 pending connections for the master socket*/
	if (listen(serverSocket, 3) < 0) {
		error("ERROR: listen() failed");
	}

	/*Event Loop*/
	addrlen = sizeof(serverAddr);
	puts("Waiting for connections...");
	for(;;) {
		FD_ZERO(&fds);
		FD_SET(serverSocket, &fds);
		for ( index = 0 ; index < MAXCLIENTS ; index++) {
			clientFd = clientId[index];
			if(clientFd > 0) {
				FD_SET( clientFd , &fds);
			}
		}

		/*Wait for activity on one of the file descriptors without any timeout limitation*/
		activity = select(FD_SETSIZE , &fds , NULL , NULL , NULL);

		if ((activity < 0) && (errno != EINTR)) {
			printf("WARN: select() error");
		}

		/*On the server socket activity handle the new client connection*/
		if (FD_ISSET(serverSocket, &fds)) {
			if ((currentSocket = accept(serverSocket, (struct sockaddr *)&serverAddr, (socklen_t*)&addrlen))<0) {
				error("ERROR: accept() failed");
			}

			printf("New connection , socket fd is %d , ip is : %s , port : %d \n" , currentSocket , inet_ntoa(serverAddr.sin_addr) , ntohs(serverAddr.sin_port));

			if( send(currentSocket, greetingMessage, strlen(greetingMessage), 0) != strlen(greetingMessage) ) {
				error("ERROR: send() failed");
			}

			puts("INFO: Client joined successfully");

			/*add new socket to array of sockets*/
			for (index = 0; index < MAXCLIENTS; index++) {
				clientFd = clientId[index];

				if (clientFd == 0) {
					clientId[index] = currentSocket;
					printf("Adding new client to the sockets list as %d\n" , index);
					index = MAXCLIENTS;
				}
			}
		}

		/*Any other activity will occur on one of the client sockets*/
		for (index = 0; index < MAXCLIENTS; index++) {
			clientFd = clientId[index];

			if (FD_ISSET( clientFd , &fds)) {
				/*Check if it was for closing , and also read the incoming message*/
				if ((valread = read( clientFd , buffer, 1024)) == 0) {
					/*Somebody disconnected , get the details and print*/
					getpeername(clientFd , (struct sockaddr*)&serverAddr , (socklen_t*)&addrlen);
					printf("Host disconnected , ip %s , port %d \n" , inet_ntoa(serverAddr.sin_addr) , ntohs(serverAddr.sin_port));

					/*Close the socket and mark as 0 in list for possible reuse*/
					close( clientFd );
					clientId[index] = 0;
					FD_CLR(clientFd, &fds);
				} else { /* Send out the new chat message*/
					buffer[valread] = '\0';
					/* Try sending the msg out to all connected clients*/
					for (index = 0; index < MAXCLIENTS; index++) {
						tempFd = clientId[index];
						if(0 != tempFd && tempFd != clientFd) {
							send( tempFd , buffer , strlen(buffer) , 0 );
						}
					}
				}
			}
		}
	}

	return EXIT_SUCCESS;
}

int main(int argc, char** argv) {
	chatServer();
	return EXIT_SUCCESS;
}
