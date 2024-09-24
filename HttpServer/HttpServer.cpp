// HttpServer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>

// Need to link to gaddrinfo from this lib
#pragma comment(lib, "Ws2_32.lib")


int main()
{

	// Need to initialize Winsock before calling any networking functions (specific to Windows)
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		printf("WSAStartup failed.\n");
		return 1;
	}


	std::cout << "Starting Http Server...\n";

	int status;
	struct addrinfo hints;
	struct addrinfo* servinfo; // will point to the results

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;        // unspecified, either IPV4 or IPV6
	hints.ai_socktype = SOCK_STREAM;    // TCP stream sockets
	hints.ai_flags = AI_PASSIVE;        // fill in the IP for me (host it's running on)

	if ((status = getaddrinfo(NULL, "3490", &hints, &servinfo)) != 0)
	{
		printf("getaddrinfo error: %s\n", (const char*)gai_strerror(status));
		exit(1);
	}


	// get the socket descriptor using the servinfo struct that was filled from getaddrinfo
	int sock_fd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
	if (sock_fd == INVALID_SOCKET)
	{
		printf("Socket creation failed with error: %d\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}

	// bind that socket to a port and IP on local machine located in ai_addr
	// allows for Listening to incoming connections
	if (bind(sock_fd, servinfo->ai_addr, servinfo->ai_addrlen) == SOCKET_ERROR)
	{
		printf("Binding failed with error: %d\n", WSAGetLastError());
		closesocket(sock_fd);
		WSACleanup();
		return 1;
	}

	// listen to incoming traffic as a server
	int backlog = 10;	// total number of connections allowed in the queue
	if (listen(sock_fd, backlog) == SOCKET_ERROR)
	{
		printf("Listen failed with error: %d\n", WSAGetLastError());
		closesocket(sock_fd);
		WSACleanup();
		return 1;
	}

	// accept incoming connection
	struct sockaddr_storage their_addr;
	socklen_t addr_size = sizeof their_addr;
	int new_fd = accept(sock_fd, (struct sockaddr*)&their_addr, &addr_size);
	if (new_fd == INVALID_SOCKET)
	{
		printf("Accepting connection failed with error: %d\n", WSAGetLastError());
		closesocket(sock_fd);
		WSACleanup();
		return 1;
	}

	std::cout << "Cleaning up\n";

	freeaddrinfo(servinfo);      // free the linked list
	closesocket(sock_fd);
	closesocket(new_fd);
	WSACleanup();
	return 0;
}

