#pragma once

#include "stdafx.h"

#include <winsock2.h>
#include <ws2tcpip.h>
#include <Windows.h>

#include <thread>
using std::thread;

class Networking {
public:
	unique_ptr<Networking> StartNetworking();
	unique_ptr<Networking> DummyNetworking();
	Networking(Networking&&) = default;
	~Networking() = default;

private:
	Networking();
	Networking(const Networking&) = delete;
	Networking operator=(const Networking&) = delete;

	static const char DEFAULT_PORT[];

	bool CreateListenerSocket();
	bool ValidListenerSocket();

	void Listen();

	thread listener_thread_;
	thread client_reader_thread_;

	SOCKET listener_socket_ = INVALID_SOCKET;
};
