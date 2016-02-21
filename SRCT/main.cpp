
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN

#pragma comment(lib, "tier0.lib")
#pragma comment(lib, "tier1.lib")
#pragma comment(lib, "vstdlib.lib")
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "network.h"

#include "Color.h"
#include "convar.h"
#include "dbg.h"
#include "interfaces.h"
#include "uglyserverpluginshit.h"


void(*Msg)(const char *, ...);

SpewOutputFunc_t spOldOutputFunc;

IVEngineServer	*g_pEngine = NULL;
ICvar *g_pCVar = NULL;
ConVar *cvclientrecport = 0;
ConVar *cvserverport = 0;
ConVar *cvpassword = 0;

UDPSocket GlobalSocket;

#define gMaxClients 1000

const char* clientaddress[gMaxClients];

int g_serverport = 0;
int g_clientport = 0;

int sys_error( int error, char* msg )
{
	Msg("Sockets error[%i]: %s\n", error, msg);
	return 0;
}


bool bPaused = false;

void SendDataToClients( const char *data, int size )
{
	if (bPaused)
		return;

	for (int i=1;i<gMaxClients;i++)
	{
		if ( !clientaddress[i] ) continue;
		if ( strlen(clientaddress[i]) < 1 ) continue;

		GlobalSocket.SendTo(clientaddress[i], g_clientport, data, size);

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

	tosend[1] = (unsigned char)spewType; //1byte
	tosend[2] = (unsigned char)r; //1byte
	tosend[3] = (unsigned char)g; //1byte
	tosend[4] = (unsigned char)b; //1byte

	strcpy(tosend + 5, msg);

	//<type><r><g><b><msg>
	//eg 0255255hi
	//eg2 000hi

	SendDataToClients(tosend, 5+strlen(msg));

	return retval;
}

int GetClient( const char *ip )
{
	
	for (int i=1;i<gMaxClients;i++)
	{
		if ( clientaddress[i] && !strcmp(clientaddress[i], ip)  )
		{
			return i;
		}
	}

	return 0;
}

int TNWReceiveThread( )
{

    try
	{

		char buffer[65506];

        while (1)
        {
			Sleep(100);

			if (bPaused)
				continue;

			memset(buffer, 0, sizeof(buffer));

			int thesize = 0;


			sockaddr_in recvfrom = GlobalSocket.RecvFrom(buffer, 65505, 0, &thesize);
			

			if (thesize == -1)
				continue;

			if (thesize > 66505)
				continue;

			if ( recvfrom.sin_family != AF_INET )
				continue;

			int i_ipAddr = recvfrom.sin_addr.s_addr;

			char *pcharIP = inet_ntoa(recvfrom.sin_addr);

			char charIP[255];
			strcpy_s(charIP, pcharIP);

			int clientnum = GetClient(charIP);
			
			if (!clientnum)
			{
				if ( strcmp(buffer, cvpassword->GetString()) != 0 )
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
					GlobalSocket.SendTo(charIP, g_clientport, "con_accepted\n", sizeof("con_accepted\n"));

					break;
				}
	
			}else{

				if (strcmp(buffer, cvpassword->GetString()) == 0)
				{
					Msg("Reconnected client %s\n", charIP);
					GlobalSocket.SendTo(charIP, g_clientport, "con_accepted\n", sizeof("con_accepted\n"));
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



void OnPortChanged()
{
	bPaused = true;
	
	if (g_serverport != cvserverport->GetInt())
	{
		Msg("[SRCT] Updated server port: %i\n", cvserverport->GetInt());
		g_serverport = cvserverport->GetInt();
		GlobalSocket.~UDPSocket();

		try
		{
			GlobalSocket.CreateSock();
			GlobalSocket.SetSocketBlockingEnabled(false);
			GlobalSocket.Bind(g_serverport);
		}
		catch (...)
		{

		}

	
	}
	
	bPaused = false;



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

	void*vstdlibmod = 0;
	void*enginemod = 0;
	while (!vstdlibmod || !enginemod)
	{
		enginemod = GetModuleHandleA("engine");
		vstdlibmod = GetModuleHandleA("vstdlib");

		Sleep(10);
	}
	
	CreateInterfaceFn VstdFactory = (CreateInterfaceFn)GetProcAddress((HMODULE)vstdlibmod, "CreateInterface");
	CreateInterfaceFn EngineFactory = (CreateInterfaceFn)GetProcAddress((HMODULE)enginemod, "CreateInterface");

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

	Msg = (void(*)(const char *, ...))GetProcAddress(GetModuleHandleA("tier0.dll"), "Msg");

	g_pCVar			= ( ICvar* )VstdFactory( "VEngineCvar004", NULL );
	g_pEngine		= (IVEngineServer*)EngineFactory( "VEngineServer021", NULL);

	memset(clientaddress, 0, sizeof(clientaddress) );


	cvclientrecport = new ConVar("srct_clientrecport", "37015", FCVAR_ARCHIVE, "The receiver port for SRCT clients");
	cvserverport = new ConVar("srct_recport", "47015", FCVAR_ARCHIVE, "The receiver port for the SRCT server");
	cvpassword = new ConVar("srct_password", "changeme", FCVAR_SERVER_CANNOT_QUERY|FCVAR_DONTRECORD, "The SRCT password");
	
	g_pCVar->RegisterConCommand(cvclientrecport);
	g_pCVar->RegisterConCommand(cvserverport);
	g_pCVar->RegisterConCommand(cvpassword);
	


	Sleep(100);
	g_serverport = cvserverport->GetInt();
	g_clientport = cvclientrecport->GetInt();
	cvserverport->m_fnChangeCallback = &OnPortChanged;

	try
	{

		SockSession Session;

		GlobalSocket.SetSocketBlockingEnabled(false);
		GlobalSocket.Bind(g_serverport);
	}

	catch (...)
    {
       
		return 0;
	}

	Msg("====================================================\n");
	Msg("Source Remote Control Tool by Leystryku Loaded v1.0\n");
	Msg("Server port: %i\n", g_serverport);
	Msg("Client Receive Port %i\n", g_clientport);
	Msg("====================================================\n");

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
