#pragma once

#include "stdafx.h"

#define WM_CBT_VIRUS_ERROR		(WM_APP + 0)
#define WM_CBT_VIRUS_PROGRESS	(WM_APP + 1)
#define WM_CBT_VIRUS_SUCCESS	(WM_APP + 2)

struct CBTProgress {
	int bytesReceived;
	int bytesTotal; // -1 if the size of an image is unknown
};

DWORD WINAPI	GentooInstallerThread( LPVOID lpParam );
VOID			GentooInstallerStop();