#include "pch.h"
#include <dwmapi.h>
#pragma comment(lib, "dwmapi.lib")
#include "Memory.h"
#include "CheatFunction.h"
#include "Globals.h"
#include "Init.h"
#include "GUI.h"
#include "Engine.h"
std::shared_ptr<Engine> EngineInstance;
std::string ProcessName;

bool InitGame()
{
	printf("Connecting to DMA Device...\n");
	while (!TargetProcess.Connect())
	{
		printf("Retrying DMA connection in 1s...\n");
		Sleep(1000);
	}

	printf("Searching for Game Process...\n");
	bool found = false;
	while (!found)
	{
		if (TargetProcess.Init("DeadByDaylight-Win64-Shipping.exe"))
		{
			ProcessName = "DeadByDaylight-Win64-Shipping.exe";
			found = true;
		}
		else if (TargetProcess.Init("DeadByDaylight-EGS-Shipping.exe"))
		{
			ProcessName = "DeadByDaylight-EGS-Shipping.exe";
			found = true;
		}
		else
		{
			Sleep(1000); // Wait 1 second before retrying
		}
	}

	printf("Game Found: %s\n", ProcessName.c_str());
	TargetProcess.FixCr3();

	uint64_t base = TargetProcess.GetBaseAddress(ProcessName);
	uint64_t size = TargetProcess.GetBaseSize(ProcessName);

	printf("Performing Memory Dump...\n");
	// Create Dumps folder if it doesn't exist
	CreateDirectory("Dumps", NULL);
	std::string dumpPath = "Dumps\\" + ProcessName + "_Dump.exe";
	if (TargetProcess.DumpMemory(base, dumpPath))
	{
		printf("Memory Dumped successfully to: %s\n", dumpPath.c_str());
	}
	else
	{
		printf("Failed to Dump Memory.\n");
	}
	
	printf("Updating Offsets and Initializing Engine...\n");
	EngineInstance = std::make_shared<Engine>();
	EngineInstance->Cache();
	return true;
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	InputWndProc(hWnd, message, wParam, lParam);
	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
		break;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	HWND hWnd;
	WNDCLASSEX wc;
	AllocConsole();
	FILE* fDummy;
	freopen_s(&fDummy, LIT("CONIN$"), LIT("r"), stdin);
	freopen_s(&fDummy, LIT("CONOUT$"), LIT("w"), stderr);
	freopen_s(&fDummy, LIT("CONOUT$"), LIT("w"), stdout);
	printf(LIT("Debugging Window:\n"));
	
	// Wait for game initialization before creating GUI
	if (!InitGame())
	{
		return 0; // Should not happen given the loop, but safety check
	}

	ZeroMemory(&wc, sizeof(WNDCLASSEX));
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = CreateSolidBrush(RGB(0, 0, 0));
	wc.lpszClassName = "GUI Framework";
	RegisterClassEx(&wc);

	hWnd = CreateWindowEx(WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_LAYERED, wc.lpszClassName, "GUI Framework",
		WS_POPUP,
		0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), NULL, NULL, hInstance, NULL);

	if (!hWnd)
		return -1;

	SetLayeredWindowAttributes(hWnd, RGB(0, 0, 0), 255, LWA_ALPHA);
	MARGINS margins = { -1 };
	DwmExtendFrameIntoClientArea(hWnd, &margins);

	ShowWindow(hWnd, nCmdShow);

	InitD2D(hWnd);
	CreateGUI();
	MSG msg;
	SetProcessDPIAware();
	SetInput();
	bool LastMenuOpen = true;
	while (TRUE)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			if (msg.message == WM_QUIT)
				break;
		}

		if (LastMenuOpen != MenuOpen)
		{
			LastMenuOpen = MenuOpen;
			if (MenuOpen)
			{
				SetWindowLong(hWnd, GWL_EXSTYLE, WS_EX_TOPMOST | WS_EX_LAYERED);
			}
			else
			{
				SetWindowLong(hWnd, GWL_EXSTYLE, WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TRANSPARENT);
			}
		}

		RenderFrame();
	}
	CleanD2D();
	return msg.wParam;
}
