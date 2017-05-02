// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
#include <string>

// 1. Changes the message displayed on an illegal move to “Not in this game.”
// ProcessMoveRequest is loop when move is made
void handle_freecell_1() {
	// New message that will be displayed
	const size_t length = 50;
	wchar_t newIllegalMsg[length] = L"Not in this game.";

	// Get the address where the illegal move message is stored
	wchar_t* DataAddress = (wchar_t*)0x01010C04;

	// Permissions require editing for message to display
	DWORD prevPermis;
	VirtualProtect(DataAddress, length, PAGE_EXECUTE_READWRITE, &prevPermis);
	memcpy(DataAddress, newIllegalMsg, length);
	VirtualProtect(DataAddress, length, prevPermis, nullptr);
}

// 2. Sets the total won to 1000 as shown by the Game->Statistics dialog.
void handle_freecell_2() {
	// Hex 1000
	DWORD number = 0x3E8;
	HKEY key;
	
	// Open registry, set games won to 1000, close
	RegOpenKeyEx(HKEY_CURRENT_USER, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Applets\\FreeCell", 0, KEY_SET_VALUE | KEY_WOW64_64KEY, &key);
	RegSetValueEx(key, L"won", 0, REG_DWORD, (LPBYTE)&number, sizeof(DWORD));
	RegCloseKey(key);
}

// 3. The next valid move wins the game.
void handle_freecell_3() {
	// Changed cheat variable to 2 and wins game on next move
	// TODO Works for invalid moves too ... idk
	*(unsigned int *)0x1007130 = 2;
}

// 4. Freecell contains a cheat code, that, when Ctrl – Shift - F10 is pressed, a dialog is presented to
// the user to “Abort, Retry, or Ignore”.If the user clicks “Abort”, the game is automatically won.
// Your DLL should change this cheat - code sequence to Ctrl - Shift – F6.
void handle_freecell_4_5() {
	// Create accellerator table
	LPACCEL myTable = new ACCEL[6];

	// Defind table; keep unused function keys the same
	myTable[0].fVirt = FVIRTKEY;
	myTable[0].key = VK_F1;
	myTable[0].cmd = 106;

	// 5. handle_freecell_5 F2 cheat shortcut
	myTable[1].fVirt = FVIRTKEY | FSHIFT | FCONTROL;
	myTable[1].key = VK_F2;
	myTable[1].cmd = 102; //req 5

	myTable[2].fVirt = FVIRTKEY;
	myTable[2].key = VK_F3;
	myTable[2].cmd = 103;

	myTable[3].fVirt = FVIRTKEY;
	myTable[3].key = VK_F4;
	myTable[3].cmd = 105;

	myTable[4].fVirt = FVIRTKEY;
	myTable[4].key = VK_F5;
	myTable[4].cmd = 109;

	// 4. handle_freecell_4 F6 cheat shortcut
	myTable[5].fVirt = FVIRTKEY | FSHIFT | FCONTROL;
	myTable[5].key = VK_F6; // f10 menu now opens with ctrl + shift + f6
	myTable[5].cmd = 114;

	// Destroy old table; causes error upon loading program but retry then loads freecell fine
	// Does old table need destroyed?
	HINSTANCE newHinst;
	HACCEL oldHaccel = LoadAccelerators(newHinst, L"FREEMENU");
	DestroyAcceleratorTable(oldHaccel);

	// Create new accel table based on entries above
	HACCEL haccel = CreateAcceleratorTable(myTable, 6);
	// Load accel table
	HACCEL newHaccel = LoadAccelerators(newHinst, MAKEINTRESOURCE(haccel));

	// From TranslateAccelerator MSDN
	MSG msg;
	BOOL bRet;
	while ((bRet = GetMessage(&msg, (HWND)NULL, 0, 0)) != 0)
	{
		if (bRet == -1)
		{
			printf("Error getting message.\n");
		}
		else
		{
			// Check for accelerator keystrokes
			// Spy++ does not come installed with free version of Visual Studio
			// Could use it to get the class name of Freecell to get the hwnd like below
			// HWND freecell_hWnd = FindWindow("ClassNameHere", NULL);
			// GetActiveWindow can work if 
			if (!TranslateAccelerator(GetActiveWindow(), haccel, &msg))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
	}
}


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		//MessageBoxA(0,"Before freecell injection!","Before Freecell Injection",1);
		handle_freecell_1();
		handle_freecell_2();
		handle_freecell_3();
		handle_freecell_4_5();
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}