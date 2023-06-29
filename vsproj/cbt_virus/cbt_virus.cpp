// cbt_virus.cpp: ���������� ����� ����� ��� ����������.
//

#include "stdafx.h"
#include "cbt_virus.h"
#include "backend.h"

#include <sstream>

#define MAX_LOADSTRING 100
#define MAX_LOADSTRING_LONGMESSAGE 1024

// ���������� ����������:
HINSTANCE hInst;								// ������� ���������
TCHAR szTitle[MAX_LOADSTRING];					// ����� ������ ���������
TCHAR szWindowClass[MAX_LOADSTRING];			// ��� ������ �������� ����

HANDLE cbt_hWorkerThread = NULL;				// ������ �� �����, ������� ����� �� ����

// ��������� ���������� �������, ���������� � ���� ������ ����:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int, HWND&);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
VOID				StopApplication();

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	MSG msg;
	HWND hWnd = NULL;

	// ������������� ���������� �����
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_CBT_VIRUS, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	{
		TCHAR szDisclaimer[MAX_LOADSTRING_LONGMESSAGE];
		int mbReturn;
		int ok = 0;

		LoadString(hInstance, IDS_CBT_DISCLAIMER, szDisclaimer, MAX_LOADSTRING_LONGMESSAGE);

		do {
			mbReturn = MessageBox(NULL, szDisclaimer, NULL, MB_OKCANCEL | MB_ICONWARNING | MB_SYSTEMMODAL | MB_DEFBUTTON2);
			switch (mbReturn) {
			case IDOK:
				ok++;
				break;
			case IDCANCEL:
			default:
				return FALSE;
			}
		} while (ok < 3);
	}

	if (!IsUserAnAdmin())
	{
		TCHAR szRequireAdmin[MAX_LOADSTRING_LONGMESSAGE];
		LoadString(hInstance, IDS_CBT_REQUIRE_ADMIN, szRequireAdmin, MAX_LOADSTRING_LONGMESSAGE);

		MessageBox(NULL, szRequireAdmin, NULL, MB_OK | MB_ICONERROR | MB_SYSTEMMODAL);

		return FALSE;
	}

	// ��������� ������������� ����������:
	if (!InitInstance (hInstance, nCmdShow, hWnd))
	{
		return FALSE;
	}

	DWORD threadId;
	cbt_hWorkerThread = CreateThread(
		NULL,
		0,
		GentooInstallerThread,
		hWnd,
		0,
		&threadId);
	if (cbt_hWorkerThread == NULL)
	{
		return FALSE;
	}

	// ���� ��������� ���������:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		//if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		//{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
		//}
	}

	StopApplication();

	return (int) msg.wParam;
}



//
//  �������: MyRegisterClass()
//
//  ����������: ������������ ����� ����.
//
//  �����������:
//
//    ��� ������� � �� ������������� ���������� ������ � ������, ���� �����, ����� ������ ���
//    ��� ��������� � ��������� Win32, �� �������� ������� RegisterClassEx'
//    ������� ���� ��������� � Windows 95. ����� ���� ������� ����� ��� ����,
//    ����� ���������� �������� "������������" ������ ������ � ���������� �����
//    � ����.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CBT_VIRUS));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= NULL; //MAKEINTRESOURCE(IDC_CBT_VIRUS);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= NULL; //LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   �������: InitInstance(HINSTANCE, int)
//
//   ����������: ��������� ��������� ���������� � ������� ������� ����.
//
//   �����������:
//
//        � ������ ������� ���������� ���������� ����������� � ���������� ����������, � �����
//        ��������� � ��������� �� ����� ������� ���� ���������.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow, HWND &hWnd)
{
   hInst = hInstance; // ��������� ���������� ���������� � ���������� ����������

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  �������: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  ����������:  ������������ ��������� � ������� ����.
//
//  WM_COMMAND	- ��������� ���� ����������
//  WM_PAINT	-��������� ������� ����
//  WM_DESTROY	 - ������ ��������� � ������ � ���������.
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// ��������� ����� � ����:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		/*case IDM_EXIT:
			DestroyWindow(hWnd);
			break;*/
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: �������� ����� ��� ���������...
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		GentooInstallerStop();
		PostQuitMessage(0);
		break;


	case WM_CBT_VIRUS_ERROR:
		break;
	case WM_CBT_VIRUS_PROGRESS:
		{
			CBTProgress *progress = (CBTProgress *) lParam;
			std::wstringstream wss;
			wss << "receiving message ";
			wss << progress->bytesReceived;
			wss << " ";
			wss << progress->bytesTotal;
			wss << std::endl;

			OutputDebugString(wss.str().c_str());
		}
		break;
	case WM_CBT_VIRUS_SUCCESS:
		break;


	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// ���������� ��������� ��� ���� "� ���������".
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

VOID StopApplication()
{
	if (cbt_hWorkerThread) {
		OutputDebugString(_T("Waiting for the worker thread to exit...\r\n"));
		WaitForSingleObject(cbt_hWorkerThread, INFINITE);
		CloseHandle(cbt_hWorkerThread);
	}
}