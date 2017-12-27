#define WIN32_LEAN_AND_MEAN
// Set startup function to standart ISO C++ int main()
#ifdef _MSC_VER
#pragma comment(linker, "/subsystem:windows /ENTRY:mainCRTStartup")
#endif
// Libraries
#pragma comment(lib, "SciEngine.lib")

// Includes
#include <windows.h>
//#include <gl/GL.h> // Dont actually need this? wow
#include <SciGame.h>

// Potentially changeable constants
const ScreenRes screen_res(400, 400);
const int       scr_bpp        = 32;                    // Bits per pixel
const bool      init_full_scr  = false;                 // Initial fullness of screen
const bool      fs_cursor      = false;                 // Show cursor in fullscreen mode
LPCSTR          game_title     = "Archer";
LPCSTR          win_class_name = "OpenGL Window Class"; // Name for a custom window class

// WIN API global variables
HDC       hdc_prime;     // Primal device context handler
HGLRC     hrc_prime;     // Primal rendering context handler
HWND      hwnd_prime;    // Primal window handler
BOOL      is_full_scr;   // Global state for screen fullness
BOOL      is_active;     // Global state of window minimization
HINSTANCE hinstance;     // Process instance handler

// Functions declarations
LRESULT CALLBACK winProc(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param);
BOOL    spawnGame(ScreenRes scr);
BOOL    spawnWindow(LPCSTR title, ScreenRes scr, int bits, bool fullscr);
BOOL    eliminateWindow();
void    resize(ScreenRes new_scr);
void    onKeyboard(UINT i_char, bool key_down);
int     errorBox(LPCSTR msg);

// ISO C++ entry function - where the adventure begins...
int main(int argc, char** argv)
{
	MSG  msg;          // Message
	BOOL abandon_ship; // Flag to know whanever game is ready to exit

	// Spawn The Window
	BOOL win = spawnWindow(game_title, screen_res, scr_bpp, init_full_scr);
	// Spawn The Game
	BOOL gm = spawnGame(screen_res);
	
	if(win & gm)
	{
		abandon_ship = FALSE; // No need to abandon ship, window & game is spawned
	}
	else
	{
		abandon_ship = TRUE; // Window or game failed to spawn, no point to start the game
	}

	// Behold, The Main Game Loop!
	while(!abandon_ship)
	{
		// Peek at OS' messages queue and dispatch to the winProc function
		if(PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE))
		{
			if(msg.message == WM_QUIT)
			{
				abandon_ship = TRUE; // QUIT message recived, time to abandon ship!
			}
			else
			{
				TranslateMessage(&msg); // Translate key messages from uint virtual codes to char
				DispatchMessage(&msg);  // Send message to winProc function
			}
		}

		// Pause the game if window is minimized
		if(is_active)
		{
			// The Game does all the Magic here
			SciGame::game->renderer->ClearBuffer();

			SciGame::game->Render();

			SwapBuffers(hdc_prime); // Make back buffer of Device Context a front buffer
		}
	}

	SciGame::Destroy(); // Eliminate game instance
	eliminateWindow();

	return (int)msg.wParam;
}
// End of main()

// Window class' Message Queue handler
LRESULT CALLBACK winProc(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param)
{
	switch(message)
	{
	case WM_ACTIVATE:
		if(!HIWORD(w_param))
		{
			is_active = TRUE;
		}
		else
		{
			is_active = FALSE;
		}
		break;

	case WM_CLOSE:
		PostQuitMessage(0);
		break;

	case WM_SIZE:
	{
		ScreenRes new_screen(LOWORD(l_param), HIWORD(l_param));
		// use resize func of game
		break;
	}

	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		// handle keys!
		break;

	case WM_KEYUP:
	case WM_SYSKEYUP:
		// handle keys!
		break;

	// Mouse control goes here...

	default:
		return DefWindowProc(hwnd, message, w_param, l_param);
	}

	return 0;
}
// End of winProc()

// Create and initialize The Game
BOOL spawnGame(ScreenRes scr)
{
	// Attempt to create game instance
	if(!SciGame::Create())
	{
		errorBox("Epic fail... Can't create game instance!");
		return FALSE; // Exit from function, return error
	}

	// Initialize game to current screen resolution
	if(!SciGame::game->Initialize(scr))
	{
		errorBox("No way game can ge initialized with given resolution...");
		return FALSE; // Exit from function, return error
	}

	return TRUE; // Function finished successfully
}
// End of spawnGame()

// Window setupper and spawner
BOOL spawnWindow(LPCSTR title, ScreenRes scr, int bits, bool fullscr)
{
	is_full_scr = fullscr;               // Set game's global fullscreen status
	hinstance   = GetModuleHandle(NULL); // Get process instance handler

	WNDCLASSEX wc; // Custom window class

	// Initiate Window Class
	wc.cbSize        = sizeof(WNDCLASSEX);                 // Size of window struct
	wc.style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC; // Window addtional styles
	wc.lpfnWndProc   = winProc;                            // Pointer to msg. handling func.
	wc.cbClsExtra    = 0;                                  // Extra bytes to allocate after struct
	wc.cbWndExtra    = 0;                                  // Extra bytes to allocate after window
	wc.hInstance     = hinstance;                          // Handle to game's process instance
	wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);    // CHANGE ICON IN FUTURE!
	wc.hIconSm       = LoadIcon(NULL, IDI_WINLOGO);        // CHANGE ICON IN FUTURE!
	wc.hCursor       = LoadCursor(NULL, IDC_ARROW);        // POTENTIALLY CHANGE CURSOR!
	wc.hbrBackground = NULL;                               // No need for background
	wc.lpszMenuName  = NULL;                               // No need for menu
	wc.lpszClassName = win_class_name;                     // Window class name
	// End of Window Class initiation

	// Register custom Window Class
	if(!RegisterClassEx(&wc))
	{
		errorBox("Sorry, can't register Window Class...");
		return FALSE; // Exit from function, return error
	}

	// Change Display Driver settings to fullscreen
	if(is_full_scr)
	{
		// Create new mode for Display Driver
		DEVMODE screen_set;
		// Set all Device Mode structure bits (members) to NULL (most of it are used only for printers)
		memset(&screen_set, NULL, sizeof(screen_set));
		// Initialize Display Driver related members
		screen_set.dmSize       = sizeof(screen_set);
		screen_set.dmPelsWidth  = scr.w;              // Screen width
		screen_set.dmPelsHeight = scr.h;              // Screen height
		screen_set.dmBitsPerPel = bits;               // Bits per pixel (pel = picture element)
		screen_set.dmFields     = DM_PELSWIDTH | DM_PELSHEIGHT | DM_BITSPERPEL;

		// Attempt to change Display Driver mode
		if(ChangeDisplaySettings(&screen_set, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
		{
			if(MessageBox(NULL, "Sorry, your GPU failed to set this resolution\n"
								"and/or BPP. Try windowed mode, shell we?", "Error",
								MB_YESNO | MB_ICONERROR) == IDYES)
			{
				is_full_scr = FALSE; // Disable fulscreen mode
			}
			else
			{
				return FALSE; // Exit from function, return error
			}
		}
	}
	// End of Display Driver manipulations

	// Select window style, depending on fullness of screen mode
	DWORD ex_style, style; // Extended Style, Generic Style
	int   init_x, init_y;  // Initial window on-screen position

	if(is_full_scr)
	{
		init_x   = init_y = 0;
		ex_style = WS_EX_APPWINDOW; // Force taskbar item to appear
		style    = WS_POPUP;        // Minimal window with no title/buttons/border
		ShowCursor(fs_cursor);      // Disable/enable cursor in fullscreen mode
	}
	else
	{
		init_x   = init_y = 300;
		ex_style = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE; // Sharp window edge
		style    = WS_OVERLAPPEDWINDOW;                // Generic default window
	}

	// Construct window box so that top-left corner is (0,0) - OpenGL style
	RECT win_box;
	win_box.left   = 0;
	win_box.top    = 0;
	win_box.right  = (LONG)scr.w;
	win_box.bottom = (LONG)scr.h;

	// Adjust window box to suit particular set of styles
	AdjustWindowRectEx(&win_box, style, FALSE, ex_style);

	// At last, The Window, shall be created!
	hwnd_prime = CreateWindowEx(ex_style,                     // Extended style
								win_class_name,               // Window class name
								title,                        // Game name
								style | WS_CLIPCHILDREN |     // Generic styles
								WS_CLIPSIBLINGS,              // CLIP required for OpenGL
								init_x, init_y,               // Initial window position
								win_box.right - win_box.left, // Window width
								win_box.bottom - win_box.top, // Window height
								NULL,                         // No need for parent handler
								NULL,                         // No need for menu handler
								hinstance,                    // Game Process Instance
								NULL);                        // No need for extra parameters
	// End of Window spawning ritual

	// Check if Window was spawned successfully
	if(!hwnd_prime)
	{
		errorBox("Window spawning was failed!");
		return FALSE; // Exit from function, return error
	}

	// Request a Primal Device Context for Primal Window to control the GDI
	if(!(hdc_prime = GetDC(hwnd_prime)))
	{
		errorBox("Device Context denied!");
		return FALSE; // Exit from function, return error
	}

	// Devise suitable Pixel Format for the game
	PIXELFORMATDESCRIPTOR pfd = {
		sizeof(PIXELFORMATDESCRIPTOR), // Size of the struct
		1,                             // Version (always 1)
		PFD_DRAW_TO_WINDOW |           // Enable drawing into window
		PFD_SUPPORT_OPENGL |           // OpenGL support!
		PFD_DOUBLEBUFFER,              // OpenGL will need double buf.
		PFD_TYPE_RGBA,                 // Standart color mode
		(BYTE)bits,                    // 32 bits per pixel mode
		0, 0, 0, 0, 0, 0,              // No color bits, no palette
		0,                             // No alpha buffer (soft-renderer)
		0,                             // No shift bit
		0,                             // No accumulation buffer
		0, 0, 0, 0,                    // No accumulation bits
		0,                             // No Z-buffer (2D game)
		0,                             // No stencil buffer (2D game)
		0,                             // No auxiliary buffer (deprecated)
		0,                             // No layer type (deprecated)
		0,                             // No overlay and underlay planes
		0, 0, 0 };                     // No layer masks (deprecated)
	// End of Pixel Format Descriptor initiation

	// Try to match custom Pixel Format to one of the system defined formats
	int pixel_format; // Code of the system defined pix. form. will go here
	if(!(pixel_format = ChoosePixelFormat(hdc_prime, &pfd)))
	{
		errorBox("There is no suitable system defined Pixel Format, sorry...");
		return FALSE; // Exit from function, return error
	}

	// Use retrived Pixel Format code to set it in OpenGL client window's Device context
	if(!SetPixelFormat(hdc_prime, pixel_format, &pfd))
	{
		errorBox("No way to set specified Pixel Format, sorry...");
		return FALSE; // Exit from function, return error
	}

	// Finally, OpenGL rendering context can be initiated
	if(!(hrc_prime = wglCreateContext(hdc_prime)))
	{
		errorBox("Epic fail while initiating OpenGL rendering context!");
		return FALSE; // Exit from function, return error
	}

	// Transfer control over window/screen (DC) to OpenGL and GPU drivers (RC)!
	if(!wglMakeCurrent(hdc_prime, hrc_prime))
	{
		errorBox("Critial failure, OpenGL rendering context can't take control!");
		return FALSE; // Exit from function, return error
	}

	// Prioritize window by getting it in forground and setting keyboard focus
	ShowWindow(hwnd_prime, SW_SHOW);
	SetForegroundWindow(hwnd_prime);
	SetFocus(hwnd_prime);

	return TRUE; // Function finished successfully
}
// End of spawnWindow()

// Annihilates Device Context, Rendering Context and The Window, restores windowed display
BOOL eliminateWindow()
{
	// Deactivate fullscreen mode, if one is active
	if(is_full_scr)
	{
		ChangeDisplaySettings(NULL, NULL); // Return to default Display Driver mode
		ShowCursor(TRUE);
	}

	// If Rendering Context is imprisoned by DC, release it
	if(hrc_prime)
	{
		// Let loose the Rendering Context
		if(!wglMakeCurrent(NULL, NULL))
		{
			errorBox("Ooops, Rendering Context is under arrest!");
		}

		// Eliminate the Rendering Context
		if(!wglDeleteContext(hrc_prime))
		{
			errorBox("Rendering context is immortal, can't destroy it!");
		}

		hrc_prime = NULL;
	}

	// Let loose the Device Context, if it exist
	if(hdc_prime && !ReleaseDC(hwnd_prime, hdc_prime))
	{
		errorBox("Not allowed, Device Context is under arrest.");
		hdc_prime = NULL;
	}

	// Finally, eliminate window, if it is valid
	if(hwnd_prime && !DestroyWindow(hwnd_prime))
	{
		errorBox("Aleart! The Window is indestructible!");
		hwnd_prime = NULL;
	}

	// Remove custom Window Class from the register
	if(!UnregisterClass(win_class_name, hinstance))
	{
		errorBox("Window Class refuses to be unregistered...");
		hinstance = NULL;
	}

	return TRUE; // Function finished successfully
}
// End of eliminateWindow()

// Display simple Error Massage Box
int errorBox(LPCSTR msg)
{
	return MessageBox(NULL, msg, "Error", MB_OK | MB_ICONERROR);
}
// End of errorBox()