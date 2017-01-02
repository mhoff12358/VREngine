#include "stdafx.h"
#include "Networking.h"
#include <winsock2.h>
#include <ws2tcpip.h>
#include <Windows.h>

#pragma comment(lib, "Ws2_32.lib")

const char Networking::DEFAULT_PORT[] = "20123";

/*unique_ptr<Networking> Networking::StartNetworking() {
	unique_ptr<Networking> net = std::make_unique<Networking>();
	return net;
}*/

bool Networking::ValidListenerSocket() {
	return listener_socket_ != INVALID_SOCKET;
}

bool Networking::CreateListenerSocket() {
	struct addrinfo *result = NULL, *ptr = NULL, hints;

	ZeroMemory(&hints, sizeof (hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	// Resolve the local address and port to be used by the server
	int iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo failed: %d\n", iResult);
		WSACleanup();
		return false;
	}

	listener_socket_ = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (!ValidListenerSocket()) {
		freeaddrinfo(result);
		return false;
	}

	iResult = bind( listener_socket_, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        printf("bind failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        closesocket(listener_socket_);
        WSACleanup();
        return 1;
    }

	return true;
}

void Networking::Listen() {
	std::cout << "Hello!" << std::endl;
	while (true) {

	}
}
