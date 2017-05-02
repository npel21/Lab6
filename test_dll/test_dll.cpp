// test_dll.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"

extern "C" void __declspec (dllexport) foo()
{
	MessageBoxA(0,"Hello World2!","Hello Title2",1);
}