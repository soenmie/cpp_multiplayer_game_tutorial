#define PLATFORM_WINDOWS  1
#define PLATFORM_MAC      2
#define PLATFORM_UNIX     3

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(_WIN32)

#define PLATFORM PLATFORM_WINDOWS

#elif defined(__APPLE__)

#define PLATFORM PLATFORM_MAC

#else

#define PLATFORM PLATFORM_UNIX

#endif

#if PLATFORM == PLATFORM_WINDOWS

#include <winsock2.h>

#elif PLATFORM == PLATFORM_MAC || PLATFORM == PLATFORM_UNIX

#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <arpa/inet.h>

#endif

#if PLATFORM == PLATFORM_WINDOWS

#pragma comment(lib,"ws2_32.lib")

#endif

#define PORTNUM 12354
#define BUFMAX 1024

#if PLATFORM == PLATFORM_MAC || PLATFORM == PLATFORM_UNIX
	void macLinuxEchoLoop(int, struct sockaddr*, socklen_t);
	void macLinuxEchoServer();
#endif

#if PLATFORM == PLATFORM_WINDOWS
	void windowsEchoLoop(SOCKET, sockaddr*, size_t);
	int windowsEchoServer();
#endif

int main(int argc, char** argv)
{
	#if PLATFORM == PLATFORM_WINDOWS

		windowsEchoServer();

	#else

		macLinuxEchoServer();

	#endif

	puts("Press any key to continue");
	getc(stdin);

	return EXIT_SUCCESS;
}

#if PLATFORM == PLATFORM_MAC || PLATFORM == PLATFORM_UNIX

void macLinuxEchoServer()
{
	int socketFileDescriptor;
	struct sockaddr_in serverAddr;
	struct sockaddr_in clientAddr;

	socketFileDescriptor = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	bzero(&serverAddr, sizeof(serverAddr));

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serverAddr.sin_port = htons(PORTNUM);

	bind(socketFileDescriptor, (struct sockaddr*)& serverAddr, sizeof(serverAddr));

	macLinuxEchoLoop(socketFileDescriptor, (struct sockaddr*)& clientAddr, sizeof(clientAddr));
}

void macLinuxEchoLoop(int sockFd, struct sockaddr* cliaddr, socklen_t clilen)
{
	int bytesRead;
	socklen_t len;
	char msg[BUFMAX] = {0};

	printf("Waiting for datagrams on 127.0.0.1:%d\n", PORTNUM);

	for(;;)
	{
		len = clilen;
		bzero(&msg, sizeof(msg));

		bytesRead = recvfrom(sockFd, msg, BUFMAX, 0, cliaddr, &len);

		printf("Got message: %s\n", msg);

		sendto(sockFd, msg, bytesRead, 0, cliaddr, len);
	}
}

#endif

#if PLATFORM == PLATFORM_WINDOWS

int windowsEchoServer()
{
	SOCKET sock;
	sockaddr_in serverAddr;
	sockaddr_in clientAddr;
	WSADATA wsaDat;

	int wsaError = WSAStartup( MAKEWORD(2,2), &wsaDat );

	if(!wsaError)
	{
		sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

		ZeroMemory(&serverAddr, sizeof(serverAddr));

		serverAddr.sin_family = AF_INET;
		serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
		serverAddr.sin_port = htons(PORTNUM);

		bind(sock, (struct sockaddr*)& serverAddr, sizeof(serverAddr));

		windowsEchoLoop(sock, (struct sockaddr*)& clientAddr, sizeof(clientAddr));
	}
	else
	{
		return EXIT_FAILURE;
	}
}

void windowsEchoLoop(SOCKET sock, sockaddr* cliaddr, size_t clilen)
{
	int bytesRead;
	int len;
	char msg[BUFMAX] = {0};

	printf("Waiting for datagrams on 127.0.0.1:%d\n", PORTNUM);

	for(;;)
	{
		len = clilen;
		ZeroMemory(&msg, sizeof(msg));

		bytesRead = recvfrom(sock, msg, BUFMAX, 0, cliaddr, &len);

		printf("Got message: %s\n", msg);

		sendto(sock, msg, bytesRead, 0, cliaddr, len);
	}

}

#endif

