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

#define dothrow

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
			throw 1;
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

class TCPSocket
{
public:
	TCPSocket()
	{
		CreateSock();
	}
	~TCPSocket()
	{
		closesocket(sock);
	}
	bool SetSocketBlockingEnabled(bool blocking)
	{
		if (sock < 0) return false;

#ifdef WIN32
		unsigned long mode = blocking ? 0 : 1;
		return (ioctlsocket(sock, FIONBIO, &mode) == 0) ? true : false;
#else
		int flags = fcntl(sock, F_GETFL, 0);
		if (flags < 0) return false;
		flags = blocking ? (flags&~O_NONBLOCK) : (flags | O_NONBLOCK);
		return (fcntl(sock, F_SETFL, flags) == 0) ? true : false;
#endif
	}

	void CreateSock()
	{
		sock = socket(AF_INET, SOCK_STREAM, 0);
		if (sock == INVALID_SOCKET)
		{
			sys_error(WSAGetLastError(), "Error opening socket");
			throw 1;
			return;
		}
	}

	void ConnectTo(const std::string& address, unsigned short port)
	{
		sockaddr_in add;
		add.sin_family = AF_INET;

		inet_pton(AF_INET, address.c_str(), &(add.sin_addr));

		add.sin_port = htons(port);

		int ret = connect(sock, (SOCKADDR*)&add, sizeof(add));

		if (ret < 0)
		{
			int err = WSAGetLastError();

			sys_error(err, "connectto failed");
			throw 1;
		}

	}

	void Send(const char*buffer, int len, int othersock=0, int flags = 0)
	{
		int ret = 0;

		if (othersock)
			ret = send(othersock, buffer, len, flags);
		else
			ret = send(sock, buffer, len, flags);

		if (ret < 0)
		{
			int err = WSAGetLastError();
			if (err == 10035)
				return;

			sys_error(err, "send failed");
			throw 1;
			return;
		}
	}

	void ListenTo(int port)
	{
		sockaddr_in add;
		add.sin_family = AF_INET;
		add.sin_addr.s_addr = htonl(INADDR_ANY);
		add.sin_port = htons(port);

		int ret = bind(sock, reinterpret_cast<SOCKADDR *>(&add), sizeof(add));
		if (ret < 0)
		{
			int err = WSAGetLastError();
			if (err == 10035)
				return;

			sys_error(err, "bind failed");
			throw 1;
			return;
		}

		ret = listen(sock, 10000);

		if (ret < 0)
		{
			int err = WSAGetLastError();
			if (err == 10035)
				return;

			sys_error(err, "listen failed");
			throw 1;
			return;
		}
	}

	int AcceptClient()
	{
		int size = sizeof(struct sockaddr_in);
		int ret = accept(sock, (struct sockaddr*)&otherip, &size);

		if (ret < 0)
		{
			int err = WSAGetLastError();
			if (err != 10035)
			{
				sys_error(err, "accept failed");
				throw 1;
			}
			return 0;
		}

		othersock = ret;

		return othersock;
	}

	int Recv(char* buffer, int len, int flags = 0, int othersock=0)
	{
		memset(buffer, 0, sizeof(buffer));
		int ret = 0;

		if (othersock)
			ret = recv(othersock, buffer, len, flags);
		else
			ret = recv(sock, buffer, len, flags);

		if (ret < 0)
		{
			int err = WSAGetLastError();
			if (err != 10035 )
			{
				sys_error(err, "recv failed");

				throw 1;
			}
		}

		return ret;
	}

	int sock;
	int othersock;

	struct sockaddr_in otherip;
};

class UDPSocket
{
public:
	UDPSocket()
	{
		CreateSock();
	}
	~UDPSocket()
	{
		closesocket(sock);
	}
	bool SetSocketBlockingEnabled(bool blocking)
	{
		if (sock < 0) return false;

#ifdef WIN32
		unsigned long mode = blocking ? 0 : 1;
		return (ioctlsocket(sock, FIONBIO, &mode) == 0) ? true : false;
#else
		int flags = fcntl(sock, F_GETFL, 0);
		if (flags < 0) return false;
		flags = blocking ? (flags&~O_NONBLOCK) : (flags | O_NONBLOCK);
		return (fcntl(sock, F_SETFL, flags) == 0) ? true : false;
#endif
	}

	void CreateSock()
	{
		sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		if (sock == INVALID_SOCKET)
		{
			sys_error(WSAGetLastError(), "Error opening socket");
			throw 1;
			return;
		}
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
			int err = WSAGetLastError();
			if (err == 10035 )
				return;

			sys_error(err, "sendto failed");
			throw 1;
			return;
		}
	}

	sockaddr_in RecvFrom(char* buffer, int len, int flags = 0, int*thesize = 0)
	{
		memset(buffer, 0, sizeof(buffer));
		sockaddr_in from;
		int size = sizeof(from);
		int ret = recvfrom(sock, buffer, len, flags, reinterpret_cast<SOCKADDR *>(&from), &size);
		if (ret < 0)
		{
			int err = WSAGetLastError();
			if (err != 10035)
			{
				sys_error(err, "recvfrom failed");

				throw 1;
			}
		}

		// make the buffer zero terminated
		buffer[ret] = 0;
		if (thesize)
			*thesize = ret;

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
			int err = WSAGetLastError();
			if (err == 10035)
				return;

			sys_error(err, "bind failed");
			throw 1;
			return;
		}

	}

	int sock;
};