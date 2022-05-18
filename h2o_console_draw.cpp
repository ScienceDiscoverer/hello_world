#include <windows.h>
#include <cstdio>

UINT_PTR timer;

void prnt(const char* t);
void pc(char c);
void pi(long long int i);
char getch();
void tproc(HWND p1, UINT p2, UINT_PTR p3, DWORD p4);
DWORD WINAPI input_thread(LPVOID lp);

int  APIENTRY WinMain(HINSTANCE hInstance,
                      HINSTANCE hPrevInstance,
                      LPSTR lpCmdLine,
                      int nCmdShow)
{ 	
	HANDLE thread = CreateThread(NULL, 0, input_thread, NULL, 0, NULL);
	
	/* HANDLE chand = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO inf;
	inf.dwSize = 100;
	inf.bVisible = FALSE;
	SetConsoleCursorInfo(chand, &inf); */
	
	
	HWND wnd = GetConsoleWindow();
	prnt("HWND: ");
	pi((long long int)wnd);
	prnt("\n");	
	
	char c = getch();
	pc(c);
	
	FLASHWINFO flash;
	flash.cbSize = sizeof(FLASHWINFO);
	flash.hwnd = wnd;
	flash.dwFlags = FLASHW_TRAY | FLASHW_TIMER;
	//flash.dwFlags = FLASHW_TRAY | FLASHW_TIMER | 1; // Flash ALL
	flash.uCount = 0;
	flash.dwTimeout = 500;
	
	FlashWindowEx(&flash);
	
	c = getch();
	pc(c);
	
	flash.dwFlags = 0;
	FlashWindowEx(&flash);
	
	timer = SetTimer(NULL, NULL, 1000, tproc);
	
	c = getch();
	pc(c);
	
	MSG msg;
	int count = 0;
	while(GetMessage(&msg, NULL, 0, 0))
	{
		/* ++count;
		if(msg.message == WM_TIMER)
		{
			prnt("TIMERMSG!!!");
		}
		
		char txt[100];
		sprintf(txt, "c:%d msg:%d\n", (long long int)count, msg.message);
		prnt(txt);*/
		
		HANDLE chand = GetStdHandle(STD_OUTPUT_HANDLE);
		CONSOLE_CURSOR_INFO inf;
		inf.dwSize = 100;
		inf.bVisible = FALSE;
		SetConsoleCursorInfo(chand, &inf);
		
		DispatchMessage(&msg);
	}
	
	/* while(1)
	{
		FlashWindow(wnd, 1);
		Sleep(500);
		FlashWindow(wnd, 1);
		Sleep(500);
	} */
	KillTimer(NULL, timer);
	return 0;
}

void prnt(const char* t)
{
	long unsigned int wrt = 0;
	HANDLE stdo = GetStdHandle(STD_OUTPUT_HANDLE);
	WriteConsole(stdo, t, strlen(t), &wrt, NULL);
}

void pc(char c)
{
	long unsigned int wrt = 0;
	HANDLE stdo = GetStdHandle(STD_OUTPUT_HANDLE);
	WriteConsole(stdo, &c, 1, &wrt, NULL);
}

void pi(long long int i)
{
	char txt[100];
	sprintf(txt, "%d", i);
	prnt(txt);
}

char getch()
{
    DWORD mode, cc;
    HANDLE h = GetStdHandle(STD_INPUT_HANDLE);

    GetConsoleMode( h, &mode );
    SetConsoleMode( h, mode & ~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT));
    char c = 0;
    ReadConsole( h, &c, 1, &cc, NULL );
    SetConsoleMode( h, mode );
	
	//pi(mode);
	
    return c;
}

void tproc(HWND p1, UINT p2, UINT_PTR p3, DWORD p4)
{
	SetTimer(NULL, timer, 1000, tproc);
	
	static char min, sec;
	++sec;
	if(sec >= 60)
	{
		sec = 0;
		++min;
	}
	char t[6];
	sprintf(t, "%02d:%02d", min, sec);
	
	COORD xy;
	xy.X = 0;
	xy.Y = 1;
	
	HANDLE oh = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleCursorPosition(oh, xy);
	prnt(t);
	//SetConsoleCursorPosition(oh, xy);
}

DWORD WINAPI input_thread(LPVOID lp)
{
	DWORD mode, cc;
	HANDLE h = GetStdHandle(STD_INPUT_HANDLE);
	HANDLE oh = GetStdHandle(STD_OUTPUT_HANDLE);

    GetConsoleMode( h, &mode );
    SetConsoleMode( h, ENABLE_EXTENDED_FLAGS | ENABLE_MOUSE_INPUT | ENABLE_PROCESSED_INPUT);
    char c = 0;
	
	INPUT_RECORD ir[128];
	DWORD nread;
	while(1)
	{
		//ReadConsole( h, &c, 1, &cc, NULL );
		//prnt("IT FUKIN WORKZ*******");
		ReadConsoleInput(h, ir, 128, &nread);
		for(int i = 0; i < nread; ++i)
		{
			if(ir[i].EventType == MOUSE_EVENT)
			{
				const MOUSE_EVENT_RECORD* m = &ir[i].Event.MouseEvent;
				if(m->dwButtonState == 0x1)
				{
					//prnt("PRESSSEEEEEED______LMB");
					SetConsoleCursorPosition(oh, m->dwMousePosition);
					//pc('#');
					pc((unsigned char)219);
				}
				else if(m->dwButtonState == 0x2)
				{
					SetConsoleCursorPosition(oh, m->dwMousePosition);
					pc(' ');
					//prnt("PRESSSEEEEEED______RMB");
				}
			}
		}
	}
}
