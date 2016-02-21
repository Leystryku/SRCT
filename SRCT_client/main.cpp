
#define WIN32_LEAN_AND_MEAN

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <ColorDlg.h>
#include "network.h"

#include "rapidjson/document.h"

#pragma comment (lib, "Ws2_32.lib")

using namespace rapidjson;


char *ip = "";
unsigned short port = 0;
char *password = "";
char cmdbuffer[998];
bool is_connected = false;

static void error_exit(char *errorMessage)
{

	fprintf(stderr, "error - %s: %d\n", errorMessage, WSAGetLastError());

	system("pause");
	exit(EXIT_FAILURE);

}

DWORD WINAPI TNetworkRecThread( )
{

    try
    {
		UDPSocket RecSocket;
		RecSocket.Bind( port+1324 );
        char buffer[4000];
		memset(buffer, 0, sizeof(buffer));

		while( 1 )
		{
			//printf("run");
			memset(buffer, 0, sizeof(buffer));

			//printf("recvfrom\n");
			RecSocket.RecvFrom(buffer, 4000);
			//printf("donerecvvfrom\n");
			
			if ( strcmp(buffer, "Connection Accepted\n") == 0 )
			{
				char title[255];

				sprintf_s(title, "SRCT - %s:%u - Connected", ip,port );

				SetConsoleTitleA(title);
				is_connected = true;

				continue;
			}

			StringStream s(buffer);
			Document d;
			d.ParseStream(s);
		
			const char *message = d[1].GetString();
			const char *group = d["2"].GetString();
			int level = d["3"].GetInt();
			int color_r = d["4"].GetInt();
			int color_g = d["5"].GetInt();
			int color_b = d["6"].GetInt();
			int color_a = d["7"].GetInt();


			printf(message);

		}

    }
    catch (const std::system_error& ex)
    {
        std::cout << ex.what();
		printf("%i", ex.code());
		system("Pause");
    }

	return 0;
}

DWORD WINAPI TNetworkSendThread( )
{

    try
    {
		UDPSocket SendSocket;
		SendSocket.Bind( port + 2324 );
		SendSocket.SendTo(ip, port, password, strlen(password));

		int shouldping = 0;

		while( 1 )
		{
			Sleep(100);

			if ( !is_connected )
			{
				SendSocket.SendTo(ip, port, password, strlen(password));
				continue;
			}

			if ( cmdbuffer && strlen(cmdbuffer) > 0 )
			{
				SendSocket.SendTo(ip, port, cmdbuffer, strlen(cmdbuffer));
				memset(cmdbuffer, 0, sizeof(cmdbuffer));
				continue;
			}

		}

	}
    catch (const std::system_error& ex)
    {
        std::cout << ex.what();
		printf("%i", ex.code());
		system("Pause");
    }

	return 0;
}


int main( int argc, char *argv[] )
{

	SetConsoleTitleA( "SRCT - Not connected" );

	//if (argc < 3)
	//	error_exit("usage: serverip serverport key\n");


	argv[1] = "127.0.0.1";
	argv[2] = "27015";	
	argv[3] = "electricpenis";

	printf("%s:%s - %s\n", argv[1], argv[2], argv[3]);

	ip = argv[1];
	port = atoi(argv[2])+14115;
	password = argv[3];

	char title[255];

	sprintf_s(title, "SRCT - %s:%u - Not connected", ip,port );

	SetConsoleTitleA( title );

    WSASession Session;

	CreateThread(0,0,(LPTHREAD_START_ROUTINE)TNetworkRecThread,0,0,0);
	CreateThread(0,0,(LPTHREAD_START_ROUTINE)TNetworkSendThread,0,0,0);

	char buffer[998];

	while (true)
	{

		std::cin.getline( buffer, 998 );

		if ( GetAsyncKeyState(VK_RETURN)&1 )
		{
			memset(cmdbuffer, 0, sizeof(cmdbuffer));
			strcpy_s(cmdbuffer, buffer);
		}
	}

	return 0;
}
