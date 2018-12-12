#include <windows.h>
#include <stdio.h>
#include <stddef.h>
#include <gl/gl.h>
#include "gldefs.h"
#include "game.h"

static void err()
{
	LPSTR errmsg;

	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER, NULL, GetLastError(), LANG_SYSTEM_DEFAULT, (LPSTR)&errmsg, 0, NULL);
	MessageBox(NULL, errmsg, "Error", MB_ICONERROR);
	LocalFree(errmsg);

	exit(0);
}

LRESULT APIENTRY mainproc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
		case WM_KEYDOWN:
			if(wParam == VK_F5)
				game_reload();
			if(wParam != VK_ESCAPE)
					break;
		case WM_QUIT:
		case WM_DESTROY:
			game_die();
			PostQuitMessage(0);
		break;
	}

	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

static void gl_init(HWND hwnd)
{
	HDC h;
	HGLRC gl;
	int nformat;

	PIXELFORMATDESCRIPTOR pfd = {
		sizeof(PIXELFORMATDESCRIPTOR),
		1,
		PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
		PFD_TYPE_RGBA,
		32,
		0, 0, 0,
		0, 0, 0,
		0, 0,
		0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0
	};

	h = GetWindowDC(hwnd);
	if(!h)
		err();
	nformat = ChoosePixelFormat(h, &pfd);
	if(!nformat)
		err();
	if(!SetPixelFormat(h, nformat, &pfd))
		err();
	gl = wglCreateContext(h);
	if(!gl)
		err();
	if(!wglMakeCurrent(h, gl))
		err();

	/* Init various opengl function pointers */
	gl_init_procs();
	wglSwapIntervalEXT(1);
}


int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, int nCmdShow)
{
	WNDCLASSEX wcx;
	MSG msg;
	HWND hwnd;
	HDC hdc;

	(void)hPrevInstance;
	(void)nCmdShow;
	(void)lpCmdLine;

	wcx.cbSize        = sizeof(WNDCLASSEX);
	wcx.style         = 0;
	wcx.lpfnWndProc   = mainproc;
	wcx.cbClsExtra    = 0;
	wcx.cbWndExtra    = 0;
	wcx.hInstance     = 0;//hInstance;
	wcx.hIcon         = NULL;
	wcx.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wcx.hbrBackground = GetSysColorBrush(COLOR_3DLIGHT);
	wcx.lpszMenuName  = NULL;
	wcx.lpszClassName = "TGM3_INPUT";
	wcx.hIconSm       = NULL;

	if(!RegisterClassEx(&wcx))
		err();

	hwnd = CreateWindowEx(
		WS_EX_WINDOWEDGE, "TGM3_INPUT", "input lol",
		WS_CLIPCHILDREN | WS_SYSMENU,
		CW_USEDEFAULT, CW_USEDEFAULT, 646, 508,
		NULL, NULL, hInstance, NULL
	);

	if(!hwnd)
		err();

	gl_init(hwnd);
	game_init();
	ShowWindow(hwnd, SW_SHOW);
	hdc = GetWindowDC(hwnd);

	while(1)
	{
		if(PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			if(msg.message == WM_QUIT)
				break;

			TranslateMessage(&msg);
			DispatchMessage(&msg);
			continue;
		}

		game();

		SwapBuffers(hdc);
	}

	return msg.wParam;
}