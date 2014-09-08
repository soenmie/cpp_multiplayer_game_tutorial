#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>

#define BUFFERSIZE 128
#define NAMESIZE 32

void error(const char*);
int chatClient(void);

void error(const char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

int chatClient(void) {
	int port = 12543;
	int sockfd;
	int bytes;
	int activity;
	int inputfd = fileno(stdin);
	struct sockaddr_in serverAddr;
	struct hostent* server;
	char writeBuffer[BUFFERSIZE] = {0};
	char readBuffer[BUFFERSIZE] = {0};
	char name[NAMESIZE] = {0};
	fd_set fds;

	printf("Input your name: ");

	// Construct chat signature
	fgets(name, sizeof(name), stdin);
	name[strlen(name) - 1] = '\0';
	strcat(name, ": ");

	// Default signature if empty
	if(0 == strncmp(name, ": ", strlen(name))) {
		bzero(name, sizeof(name));
		strcat(name, "anonymous coward: ");
	}

	puts("Type \"exit\" without quotes to leave the chat.");

	if( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0 ) {
		error("ERROR: socket() failed");
	}

	if( (server = gethostbyname("localhost")) == NULL ) {
		error("ERROR: gethostbyname() failed");
	}

	// Prepare the server address struct
	bzero((char *)& serverAddr, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	bcopy((char *)server->h_addr, (char *)& serverAddr.sin_addr.s_addr, server->h_length);
	serverAddr.sin_port = htons(port);

	if( connect(sockfd, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) < 0 ) {
		error("ERROR connect() failed");
	}

	// Alert users in the chat room
	strcat(writeBuffer, name);
	strcat(writeBuffer, "has joined the chat\n");
	if( (bytes = write(sockfd, writeBuffer, sizeof(writeBuffer))) <= 0 ) {
		error("ERROR: write() failed");
	}
	bzero(writeBuffer, sizeof(writeBuffer));

	// Event Loop
	for(;;) {
		// Clear and load the file descriptor set
		FD_ZERO(&fds);
		FD_SET(inputfd, &fds);
		FD_SET(sockfd, &fds);

		if( (activity = select(FD_SETSIZE, &fds, NULL, NULL, NULL)) < 0 ) {
			error("ERROR: select() failed");
		}

		// In the event of user input
		if(FD_ISSET(inputfd, &fds)){
			bzero(readBuffer, sizeof(readBuffer));
			if( (bytes = read(inputfd, readBuffer, sizeof(readBuffer))) <= 0 ) error("ERROR: read() failed");

			if(0 == strncmp("exit", readBuffer, 4)) break;

			strncpy(writeBuffer, name, strlen(name));
			strncat(writeBuffer, readBuffer, bytes - 1);
			if( (bytes = write(sockfd, writeBuffer, sizeof(writeBuffer))) <= 0 ) error("ERROR: write() failed");
			bzero(writeBuffer, sizeof(writeBuffer));
		}

		// In the event of a new server message
		if(FD_ISSET(sockfd, &fds)){
			bzero(readBuffer, sizeof(readBuffer));
			if( (bytes = read(sockfd, readBuffer, sizeof(readBuffer))) <= 0 ) error("ERROR: recv() failed");

			printf("%s\n", readBuffer);
		}
	}

	// Alert users in the chat room
	strcat(writeBuffer, name);
	strcat(writeBuffer, "has left the chat\n");
	if( (bytes = write(sockfd, writeBuffer, sizeof(writeBuffer))) <= 0 ) error("ERROR: write() failed");
	bzero(writeBuffer, sizeof(writeBuffer));

	close(sockfd);

	return EXIT_SUCCESS;
}

int main(int argc, char *argv[]) {
	chatClient();
    return EXIT_SUCCESS;
}
