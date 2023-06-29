#include "stdafx.h"
#include "backend.h"

#include <sstream>
#include <climits>

#define CBT_MESSAGE_POST_INTERVAL 1000 // milliseconds

volatile bool running = false;

DWORD WINAPI GentooInstallerThread( LPVOID lpParam )
{
	HWND hWnd = (HWND) lpParam;

	DWORD currentMillis, lastMillis = 0, diffMillis;
	
	CBTProgress *progress = new CBTProgress;
	progress->bytesReceived = 0;
	progress->bytesTotal = 0;

	running = true;
	while (running) {
		progress->bytesReceived++; // TODO: вывести количество скачанных байт

		currentMillis = GetTickCount();
		if (currentMillis < lastMillis) { // защита от переполнения, сработает где-то через 48 дней. TODO: нахуя???
			diffMillis = ULONG_MAX - lastMillis + currentMillis;
		} else {
			diffMillis = currentMillis - lastMillis;
		}

		if (diffMillis >= CBT_MESSAGE_POST_INTERVAL) {
			/*
			std::wstringstream wss;
			wss << "sending message ";
			wss << progress->bytesReceived;
			wss << " ";
			wss << progress->bytesTotal;
			wss << std::endl;

			OutputDebugString(wss.str().c_str());
			*/
			SendMessageTimeoutA(
				hWnd,
				WM_CBT_VIRUS_PROGRESS,
				NULL,
				(LPARAM) progress,
				SMTO_NOTIMEOUTIFNOTHUNG,
				15, // ~frame time of 60 FPS
				NULL);
			
			lastMillis = currentMillis;
		}
	}

	delete progress;
	return 0;
}

VOID GentooInstallerStop()
{
	OutputDebugString(_T("stopping the worker thread...\r\n"));

	running = false; // TODO: возможно будет заморочка с потокобезопаснотью
}