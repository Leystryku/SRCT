#define WIN32_LEAN_AND_MEAN

typedef signed char int8;
typedef unsigned char uint8;

#include "Form1.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <msclr/marshal.h> 
#include <msclr\marshal_cppstd.h> 

#include "../SRCT/network.h"


#pragma comment (lib, "user32.lib")

unsigned short clientport = 0;
unsigned short sendtoport = 0;

const char *ip = "";

const char *password = "";

char cmdbuffer[998];

bool bIsConnected = false;
bool bReviveMe = true;

bool bFilter_generic = false;
bool bFilter_warnings = false;
bool bFilter_asserts = false;
bool bFilter_errors = false;
bool bFilter_logs = false;
bool bFilter_count = false;

bool bBuiltMenu = false;

char args[10][255];

namespace MainForm
{
	gcroot<SRCT_cl::Form1 ^> form1;
};




int sys_error(int error, char *errormsg)
{
	
	char formatted[255];
	sprintf_s(formatted, "Socket error [%i]: %s\n", error, errormsg);
	MainForm::form1->AddConsoleText(255,255,255,255,formatted);

	return 0;
}

int TNetworkRecThread( )
{

	try{

	
	UDPSocket Socket;

	Socket.Bind(clientport);
	Socket.SetSocketBlockingEnabled(false);

	char buffer[65506];
	char message[65502];

	while (1)
	{
		Sleep(1);
		memset(message, 0, sizeof(message));
		memset(buffer, 0, sizeof(buffer));
		
		
		int thesize = 0;
		Socket.RecvFrom(buffer, 65505, 0, &thesize);

		if (!bIsConnected)
		{

			static int waits = 0;

	
			if (!waits)
			{
				Socket.SendTo(ip, clientport, password, strlen(password));
				Socket.SendTo(ip, sendtoport, password, strlen(password));
				waits = 30;
				continue;
			}else{
				waits--;
			}

		} else{

			if (cmdbuffer && strlen(cmdbuffer) > 0)
			{
				Socket.SendTo(ip, sendtoport, cmdbuffer, strlen(cmdbuffer));
				memset(cmdbuffer, 0, sizeof(cmdbuffer));
				continue;
			}

		}

		if (thesize == -1)
			continue;

	

		if (strncmp(buffer, "con_accepted\n", 25) == 0)
		{
			bIsConnected = true;

			char title[255];

			sprintf_s(title, "SRCT - %s:%u - Connected", ip, clientport);

			System::String^ clistr = gcnew System::String(title);

			MainForm::form1->Text::set(clistr);
			continue;
		}

		if (!bIsConnected)
			continue;

		int type = int((uint8)buffer[1]); // turn the uint8 to int
		int r = int((uint8)buffer[2]); // turn the uint8 to int
		int g = int((uint8)buffer[3]); // turn the uint8 to int
		int b = int((uint8)buffer[4]); // turn the uint8 to int

		
		strcpy(message, buffer + 5);
		message[strlen(buffer + 5) + 1] = '\0';

		if (strstr(message, "[ERROR]"))
		{
			type = 3;
		}


		if (type == 0 && bFilter_generic)
			continue;

		if (type == 1 && bFilter_warnings)
			continue;

		if (type == 2 && bFilter_asserts)
			continue;

		if (type == 3 && bFilter_errors)
			continue;

		if (type == 4 && bFilter_logs)
			continue;

		if (type == 5 && bFilter_count)
			continue;

		MainForm::form1->AddConsoleText(r, g, b, 255, message);

	}

	}

	catch (...)
	{
		char title[255];

		sprintf_s(title, "SRCT - Not connected", ip, sendtoport);

		System::String^ clistr = gcnew System::String(title);

		MainForm::form1->Text::set(clistr);

		bIsConnected = false;

		MainForm::form1->AddConsoleText(255, 255, 255, 255, "Retrying in 3 seconds...\n");
		Sleep(3000);

		bReviveMe = true;
		ExitThread(0);


	}

	return 0;
}

int __stdcall MainThread( )
{

	while (!bBuiltMenu) Sleep(500);

	//MainForm::form1->AddConsoleText(255,255,255,255, "error: usage: serverip serverport key, please restart with proper args!\n");

#ifdef _DEBUG
	clientport = 37015;
	ip = "127.0.0.1";
	sendtoport = 47015;
	password = "test123";
#else
	clientport = atoi(args[1]);
	ip = args[2];
	sendtoport = atoi(args[3]);
	password = args[4];
#endif

	char formatted[300];
	sprintf_s(formatted, "%s:%i - %s\nClient port: %i\n", ip, sendtoport, password, clientport);
	MainForm::form1->AddConsoleText(255, 255, 255, 255, formatted);

	 
	


	//for (int i = 0; i < 9999; i++)
	//	MainForm::form1->AddConsoleText(255,255,255,255, "FAGS");

	SockSession Session;



	while (1)
	{
		if (bReviveMe)
		{
			CreateThread(0, 0, (LPTHREAD_START_ROUTINE)TNetworkRecThread, 0, 0, 0);
			bReviveMe = false;
		}

		if ( GetAsyncKeyState(VK_RETURN) )
		{

			if ( bIsConnected )
				MainForm::form1->EnterInputText();

		}

		Sleep(10);
	}

	return 0;
}

int main( array<System::String ^> ^aargs)
{
#ifndef _DEBUG
	if (aargs && aargs->Length == 4)
	{
		msclr::interop::marshal_context^ context = gcnew msclr::interop::marshal_context();
		for (int i = 0; i < 4; i++)
		{
			strcpy(args[i+1], context->marshal_as<const char*>(aargs[i]));
		}


	}else{

		MessageBoxA(NULL, "Invalid number of arguments!", "error!", MB_OK);
		return 0;
	}
#endif

	MainForm::form1  = (gcnew SRCT_cl::Form1);
	MainForm::form1->CheckForIllegalCrossThreadCalls = false;

	SRCT_cl::Application::EnableVisualStyles();
	SRCT_cl::Application::Run(MainForm::form1);



	return 0;
}