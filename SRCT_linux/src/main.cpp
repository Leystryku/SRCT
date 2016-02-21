#define WIN32_LEAN_AND_MEAN
#define ipV6

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <string>


#include "dbg.h"
#include "Color.h"
#include "tier1/iconvar.h"
#include "tier1/tier1.h"
#include "vstdlib/cvar.h"
#include "convar.h"
#include "network.h"
#include "eiface.h"
#include "igameevents.h"

SpewOutputFunc_t spOldOutputFunc;

IVEngineServer	*g_pEngine = NULL;

UDPSocket GlobalRecSocket;
UDPSocket GlobalSendSocket;

#define gMaxClients 100

const char* clientaddress[gMaxClients];

int g_recport = 0;
int g_sendport = 0;
int g_clientrecport = 0;

int sys_error( int error, char* msg )
{
	Msg("Sockets error[%i]: %s", error, msg);
	return 0;
}

void SendDataToClients( const char *data, int size )
{

	for (int i=1;i<gMaxClients;i++)
	{
		if ( !clientaddress[i] ) continue;
		if ( strlen(clientaddress[i]) < 1 ) continue;

		GlobalSendSocket.SendTo(clientaddress[i], g_clientrecport, data, size);
		Sleep(2);

	}

}

char tosend[65506];

SpewRetval_t spMessageHandler(SpewType_t spewType, const char *msg)
{
	if (!msg) return SPEW_CONTINUE;

	SpewRetval_t retval = spOldOutputFunc(spewType, msg);

	const Color *outputcolor = GetSpewOutputColor();
	int r, g, b, a;
	outputcolor->GetColor(r,g,b,a);

	
	memset(tosend, 0, sizeof(tosend));

	tosend[1] = uint8(spewType); //1byte
	tosend[2] = uint8(r); //1byte
	tosend[3] = uint8(g); //1byte
	tosend[4] = uint8(b); //1byte

	strcpy(tosend + 5, msg);

	//<type><r><g><b><msg>
	//eg 0255255hi
	//eg2 000hi

	SendDataToClients(tosend, 5+strlen(msg));

	return retval;
}

bool IsAlreadyConnected( const char *ip )
{
	
	for (int i=1;i<gMaxClients;i++)
	{
		if ( clientaddress[i] && !strcmp(clientaddress[i], ip)  )
		{
			return true;
		}
	}

	return false;
}

int TNWReceiveThread( )
{

    try
	{

		char buffer[65506];

        while (1)
        {
			memset(buffer, 0, sizeof(buffer));
			sockaddr_in recvfrom = GlobalRecSocket.RecvFrom(buffer, 65505);

			if ( recvfrom.sin_family != AF_INET )
			{
				//int fam = recvfrom.sin_family;
				//Msg("Family isnt AF_INET %i !", fam );
				continue;
			}

			int i_ipAddr = recvfrom.sin_addr.s_addr;

			char *pcharIP = inet_ntoa(recvfrom.sin_addr);

			char charIP[255];
			strcpy(charIP, pcharIP);

			int clientnum = 0;

			if ( !IsAlreadyConnected(charIP) )
			{
				if ( strcmp(buffer, /*g_pCVar->FindVar("rcon_password")->GetString()*/"test123") != 0 )
				{
					
					Sleep(200);
					continue;
				}

				for (int i=1;i<gMaxClients;i++)
				{
					if ( clientaddress[i] )
						continue;
					
					clientaddress[i] =  charIP;
					clientnum = i;
					Msg("Received client %s - session nr:%i\n", charIP, i );
					GlobalSendSocket.SendTo(charIP, g_clientrecport, "Connection Accepted\n", sizeof("Connection Accepted\n"));

					break;
				}
	
			}else{

				if (strcmp(buffer, "test123"/*g_pCVar->FindVar("rcon_password")->GetString()*/) == 0)
				{
					Msg("Reconnected client %s\n", charIP);
					GlobalSendSocket.SendTo(charIP, g_clientrecport, "Connection Accepted\n", sizeof("Connection Accepted\n"));
					Sleep(200);
					continue;
				}

				std::string cmd = buffer;
				cmd.append("\n");

				g_pEngine->ServerCommand( cmd.c_str() );
			}

			Sleep(200);
        }
    }
    catch (...)
    {

    }
	return 1;
}

void PreSocketDead( )
{

}

bool didexec = false;

int mainexec( )
{
	if (didexec)
		return 0;

	didexec = true;

	GetSpewOutputFunc();

	spOldOutputFunc = GetSpewOutputFunc();

	SpewOutputFunc(spMessageHandler);

	CreateInterfaceFn VstdFactory   = Sys_GetFactory("vstdlib");
	CreateInterfaceFn EngineFactory = Sys_GetFactory("engine");

	if ( !VstdFactory )
	{
		MessageBox( NULL, "no vstd factory", "k", MB_OK );
		return 0;
	}

	if ( !EngineFactory )
	{
		MessageBox( NULL, "no engine factory", "k", MB_OK );
		return 0;
	}

	g_pCVar			= ( ICvar* )VstdFactory( CVAR_INTERFACE_VERSION, NULL );
	g_pEngine		= (IVEngineServer*)EngineFactory( INTERFACEVERSION_VENGINESERVER, NULL);

	memset(clientaddress, 0, sizeof(clientaddress) );

	ConVar_Register(0);

	ConVar *cvclientrecport = new ConVar("srct_clientrecport", "37015", FCVAR_ARCHIVE, "The receiver port for SRCT clients");
	ConVar *cvrecport = new ConVar("srct_recport", "47015", FCVAR_ARCHIVE, "The receiver port for the SRCT server");
	ConVar *cvsendport = new ConVar("srct_sendport", "57015", FCVAR_ARCHIVE, "The port the SRCT server uses for sending ( just use some unused port )");


	g_sendport = cvsendport->GetInt();
	g_recport = cvrecport->GetInt();
	g_clientrecport = cvclientrecport->GetInt();

	try
	{

		SockSession Session;

		GlobalRecSocket.Bind(g_recport);
		GlobalSendSocket.Bind(g_sendport);
	}

	catch (...)
    {
       
		return 0;
	}

	Msg( "====================================================\n" );
	Msg( "Source Remote Control Tool by Leystryku Loaded v1.0\n" );
	Msg( "Send port: %i\n", g_sendport );
	Msg( "Receive port: %i\n", g_recport);
	Msg( "Client Receive Port %i\n", g_clientrecport );
	Msg( "====================================================\n");

	CreateThread( NULL, NULL, (LPTHREAD_START_ROUTINE)TNWReceiveThread, 0, 0, 0);
	
	return 0;
}


int DllMain(HMODULE hInstDLL, DWORD reason, LPVOID lpReserved)
{

	if (reason == DLL_PROCESS_ATTACH)
	{
		CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)mainexec, NULL, NULL, NULL);
		return 1;
	}

	return 1;
}


#include "uglyserverpluginshit.h"