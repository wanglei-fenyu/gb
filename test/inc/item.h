#pragma once
#include "item_info.h"
#include "utf8_str.h"
#include <windows.h>
#include <winnt.h>
#include <string>
 
 


#if 0
int RunCmd(HANDLE& hRead,HANDLE& hWrite,const std::string cmd,std::string args,int& code) {
	SECURITY_ATTRIBUTES sa;
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle = TRUE;

	if (!CreatePipe(&hRead, &hWrite, &sa, 0)) {
		DWORD ret = GetLastError();
		return ret ? ret : -1;
	}

	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si, sizeof(STARTUPINFO));
	ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));

	si.cb = sizeof(STARTUPINFO);
	GetStartupInfo(&si);
	si.hStdError = hWrite;
	si.hStdOutput = hWrite;
	si.wShowWindow = SW_HIDE;
	si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;

	char cmdline[200] = { 0 };
	sprintf(cmdline, "%s", cmd.c_str());
	char argsline[200] = { 0 };
	sprintf(argsline, "%s", args.c_str());
	if (!CreateProcess(NULL,cmdline, NULL, NULL, TRUE, NULL,
						NULL, NULL, &si, &pi)) {
		DWORD ret = GetLastError();
		CloseHandle(hRead);
		CloseHandle(hWrite);
		return ret ? ret : -1;
	}	

	CloseHandle(hWrite);


//	DWORD exitCode = 0;
//	GetExitCodeProcess(pi.hProcess, &exitCode);
//	code = exitCode;
//	CloseHandle(hRead);	
//	CloseHandle(pi.hThread);
//	CloseHandle(pi.hProcess);
	return 0;
}

class Item
{
public:
	Item(std::string title,std::string cmd) : item_title(title)
	{
		console.set_title(item_title.c_str());
		logtick = 0;
		RunCmd(hRead, hWrite,cmd,"",ret);
	}
	void init()
	{
		RunCmd(hRead, hWrite,"G:\\GmBase_gl\\bin\\Debug\\Base.exe","",ret);
		//RunCmd(hRead, hWrite,"D:\\XX\\master\\Copy_Echo_SDK.bat",ret);
	}

	void OnDestroy()
	{

	}

	void GetLog()
	{
		DWORD bytesRead;
		char buffer[4096] = { 0 };
		::ZeroMemory(buffer, sizeof(buffer));

		
		if (ReadFile(hRead, buffer, 4096, &bytesRead, NULL))
		{
			console.AddLog(U8(buffer).c_str());
		}

	}



	void DrowConsole()
	{
		console.Draw(("["+item_title + "]").c_str());
		if (logtick % 10 == 0)
		{
			GetLog();
			logtick = 0;
		}
		logtick++;
	}




public:
	int logtick;
	HANDLE hRead, hWrite;
	int ret;
	std::string item_title;
	std::string readbuffer;
private: 
	ItemConsole console;
	HANDLE hPipeRead;
	HANDLE hPipeWrite;
	SECURITY_ATTRIBUTES saOutPipe;


	
};

#endif
