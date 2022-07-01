#include <windows.h>

int main()
{
	// NEVER QUOTE lpApplicationName IT SCREWS UP PARSING RESULTING IN ACESS_DENIED (ERR5)
	LPCSTR app = "C:\\Program Files\\BlueStacks\\Bluestacks.exe";
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	
	memset(&si, 0, sizeof(si));
	si.cb = sizeof(si);
	memset(&pi, 0, sizeof(pi));
	
	// Create Process ============================================================================
	BOOL res = CreateProcess(
		app,	//  [I|O]  Name of the module to be executed, that's it
		NULL,	// [IO|O]  Command line to be exectued, searches PATH, adds extention
		NULL,	//  [I|O]  Sec. Attrib. for inhereting new process by child processes
		NULL,	//  [I|O]  Sec. Attrib. for inhereting new thread by child processes
		FALSE,	//    [I]  New proc. inherits each inheritable handle
		NULL,	//    [I]  Process creation flags
		NULL,	//  [I|O]  Ptr to environment block of new process (inherit if NULL)
		NULL,	//  [I|O]  Full path to current directory for the process
		&si,	//    [I]  Ptr to STARTUPINFO struct, if dwFlags = 0, def. values used
		&pi	//    [O]  Ptr to PROCESS_INFORMATION struct with new proc identification info
	);
	// ===========================================================================================
	
	if(res)
	{
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
		Sleep(14000);
	}
	else
	{
		return -1;
	}
	
	INPUT inps[2];
	memset(&inps, 0, sizeof(inps));
	
	inps[0].type = INPUT_KEYBOARD;
	inps[0].ki.wScan = 0x57; // F11
	inps[0].ki.dwFlags = KEYEVENTF_SCANCODE;
	
	inps[1].type = INPUT_KEYBOARD;
	inps[1].ki.wScan = 0x57; // F11
	inps[1].ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
	
	SendInput(4, inps, sizeof(INPUT));
	
	return 0;
}
