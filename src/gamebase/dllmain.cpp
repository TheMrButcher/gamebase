#include "stdafx.h"

BOOL APIENTRY DllMain(HMODULE, DWORD reasonForCall, LPVOID)
{
	switch (reasonForCall) {
	    case DLL_PROCESS_ATTACH:
	    case DLL_THREAD_ATTACH:
	    case DLL_THREAD_DETACH:
	    case DLL_PROCESS_DETACH:
		    break;
	}
	return TRUE;
}
