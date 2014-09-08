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
	void error(const char*);
	void macLinuxEchoLoop(int, struct sockaddr*, socklen_t);
	void macLinuxEchoClient();
#endif

#if PLATFORM == PLATFORM_WINDOWS
	void windowsEchoLoop(SOCKET, SOCKADDR*, size_t);
	int windowsEchoClient();
#endif

int main(int argc, char** argv)
{

	#if PLATFORM == PLATFORM_WINDOWS

		windowsEchoClient();

	#else

		macLinuxEchoClient();

	#endif

	puts("Press any key to continue");
	getc(stdin);

	return EXIT_SUCCESS;
}

#if PLATFORM == PLATFORM_MAC || PLATFORM == PLATFORM_UNIX

void error(const char *msg) {
	perror(msg);
	exit(EXIT_FAILURE);
}

void macLinuxEchoClient()
{
	int socketFileDescriptor;
	struct sockaddr_in serverAddr;

	bzero(&serverAddr, sizeof(serverAddr));

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORTNUM);
	serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);

	socketFileDescriptor = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	if(socketFileDescriptor < 0)
		error("socket() failed");

	macLinuxEchoLoop(socketFileDescriptor, (struct sockaddr*)& serverAddr, sizeof(serverAddr));
}

void macLinuxEchoLoop(int sock, struct sockaddr* serverAddr, socklen_t serverlen)
{
	int bytesRead;
	int sentResult;
	char inputBuffer[BUFMAX] = {0};
	char recvBuffer[BUFMAX] = {0};

	for(;;)
	{
		printf("Type message: ");
		fgets(inputBuffer, BUFMAX, stdin);

		sentResult = sendto(sock, inputBuffer, strlen(inputBuffer), 0, serverAddr, serverlen);

		if(sentResult < 0)
			error("sendTo() failed");

		bytesRead = recvfrom(sock, recvBuffer, BUFMAX, 0, NULL, NULL);

		if(bytesRead < 0)
			error("recvfrom() failed");

		recvBuffer[bytesRead] = 0; /* NULL terminates the char array */
		printf("Server responds: %s\n", recvBuffer);
	}
}

#endif

#if PLATFORM == PLATFORM_WINDOWS

int windowsEchoClient()
{
	SOCKET sock;
	sockaddr_in serverAddr;
	WSADATA wsaDat;

	int wsaError = WSAStartup( MAKEWORD(2,2), &wsaDat );

	if(!wsaError)
	{
		sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

		if (sock == INVALID_SOCKET)
		{
			wprintf(L"socket function failed with error = %d\n", WSAGetLastError() );
			exit(-1);
		}

		ZeroMemory(&serverAddr, sizeof(serverAddr));

		serverAddr.sin_family = AF_INET;
		serverAddr.sin_port = htons(PORTNUM);
		serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

		sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

		windowsEchoLoop(sock, (SOCKADDR*)& serverAddr, sizeof(serverAddr));
	}
	else
	{
		return EXIT_FAILURE;
	}
}

void windowsEchoLoop(SOCKET sock, SOCKADDR* serverAddr, size_t serverlen)
{
	int bytesRead;
	int sendToResult;
	char inputBuffer[BUFMAX] = {0};
	char recvBuffer[BUFMAX] = {0};

	for(;;)
	{
		printf("Type message: ");
		fgets(inputBuffer, BUFMAX, stdin);

		sendToResult = sendto(sock, inputBuffer, strlen(inputBuffer), 0, serverAddr, serverlen);

		if (sendToResult == SOCKET_ERROR) {
			wprintf(L"sendto failed with error: %d\n", WSAGetLastError());
			closesocket(sock);
			WSACleanup();

			puts("Press any key to continue");
			getc(stdin);
			exit(sendToResult);
		}

		bytesRead = recvfrom(sock, recvBuffer, BUFMAX, 0, NULL, NULL);

		if (bytesRead == SOCKET_ERROR) {
			wprintf(L"recvfrom failed with error: %d\n", WSAGetLastError());
			closesocket(sock);
			WSACleanup();

			puts("Press any key to continue");
			getc(stdin);
			exit(bytesRead);
		}

		recvBuffer[bytesRead] = 0; /* NULL terminates the char array */
		printf("Server responds: %s\n", recvBuffer);
	}
}

#endif


