#pragma once

#ifdef _WIN32
#include <WinSock2.h>
#include <WS2tcpip.h>
#pragma comment (lib, "Ws2_32.lib")
#else

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
void closesocket(int socket) { close(socket); }
#endif

#include <string>
#include <iostream>


extern int sys_error(int error, char *errormsg);

class SockSession
{
public:
	SockSession()
	{
#ifdef _WIN32
		int ret = WSAStartup(MAKEWORD(2, 2), &data);
		if (ret != 0)
		{
			sys_error(WSAGetLastError(), "WSAStartup Failed");
#ifdef dothrow
			throw;
#endif
			return;
		}
#endif
	}
	~SockSession()
	{
#ifdef _WIN32
		WSACleanup();
#endif
	}
private:
#ifdef _WIN32
	WSAData data;
#endif

};

class UDPSocket
{
public:
	UDPSocket()
	{
		sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		if (sock == INVALID_SOCKET)
		{
			sys_error(WSAGetLastError(), "Error opening socket");
#ifdef dothrow
			throw;
#endif
			return;
		}
	}
	~UDPSocket()
	{
		closesocket(sock);
	}

	void SendTo(const std::string& address, unsigned short port, const char* buffer, int len, int flags = 0)
	{
		sockaddr_in add;
		add.sin_family = AF_INET;

		inet_pton(AF_INET, address.c_str(), &(add.sin_addr));

		add.sin_port = htons(port);
		int ret = sendto(sock, buffer, len, flags, reinterpret_cast<SOCKADDR *>(&add), sizeof(add));
		if (ret < 0)
		{
			sys_error(WSAGetLastError(), "sendto failed");
#ifdef dothrow
			throw;
#endif
			return;
		}
	}
	void SendTo(sockaddr_in& address, const char* buffer, int len, int flags = 0)
	{
		int ret = sendto(sock, buffer, len, flags, reinterpret_cast<SOCKADDR *>(&address), sizeof(address));
		if (ret < 0)
		{
			sys_error(WSAGetLastError(), "sendto failed");
#ifdef dothrow
			throw;
#endif
			return;
		}
	}
	sockaddr_in RecvFrom(char* buffer, int len, int flags = 0)
	{
		sockaddr_in from;
		int size = sizeof(from);
		int ret = recvfrom(sock, buffer, len, flags, reinterpret_cast<SOCKADDR *>(&from), &size);
		if (ret < 0)
		{
			sys_error(WSAGetLastError(), "recvfrom failed");
#ifdef dothrow
			throw;
#endif
		}

		// make the buffer zero terminated
		buffer[ret] = 0;
		return from;
	}
	void Bind(unsigned short port)
	{
		sockaddr_in add;
		add.sin_family = AF_INET;
		add.sin_addr.s_addr = htonl(INADDR_ANY);
		add.sin_port = htons(port);

		int ret = bind(sock, reinterpret_cast<SOCKADDR *>(&add), sizeof(add));
		if (ret < 0)
		{
			sys_error(WSAGetLastError(), "bind failed");
#ifdef dothrow
			throw;
#endif
			return;
		}

	}

	int sock;
};