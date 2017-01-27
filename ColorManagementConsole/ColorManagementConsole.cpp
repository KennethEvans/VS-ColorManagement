// ColorManagementConsole.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <Windows.h>
#include <iostream>
#include <Icm.h>
#include <strsafe.h>

using namespace std;

// Prototypes
void getDisplays();
void getProfile(TCHAR* szDisplayDeviceName, TCHAR* szPrefix);
TCHAR* getErrorMsg();
void displayLastError(LPTSTR lpszFunction);

TCHAR* getErrorMsg() {
	TCHAR buf[256];
	FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), buf, 256, NULL);
	return buf;
}

void displayLastError(LPTSTR lpszFunction) {
	// Retrieve the system error message for the last-error code

	LPVOID lpMsgBuf;
	LPVOID lpDisplayBuf;
	DWORD dw = GetLastError();

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dw,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf,
		0, NULL);

	// Display the error message
	lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT,
		(lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszFunction) + 40) * sizeof(TCHAR));
	StringCchPrintf((LPTSTR)lpDisplayBuf,
		LocalSize(lpDisplayBuf) / sizeof(TCHAR),
		_T("%s:\nError %d: %s"),
		lpszFunction, dw, lpMsgBuf);
	MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK);

	LocalFree(lpMsgBuf);
	LocalFree(lpDisplayBuf);
}

void getDisplays() {
	WCHAR szPath[MAX_PATH];
	DISPLAY_DEVICE dd;
	dd.cb = sizeof(dd);
	int devNum = 0;
	wcout <<  "Displays" << endl;
	while (EnumDisplayDevices(NULL, devNum, &dd, 0)) {
		wcout << "Device " << devNum << endl;
		wcout
			<< "    DeviceName=" << dd.DeviceName << endl
			<< "    DeviceString=" << dd.DeviceString << endl
			<< "    DeviceID=" << dd.DeviceID << endl
			<< "    DeviceKey=" << dd.DeviceKey << endl
			<< "    StateFlags=" << dd.StateFlags
			<< " " << ((dd.StateFlags & DISPLAY_DEVICE_ACTIVE) ? "Active" : "")
			<< " " << ((dd.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE) ? "Primary" : "")
			<< endl;
		getProfile(dd.DeviceName, _T("    "));
		devNum++;
	}
}


void getProfile(TCHAR* szDisplayDeviceName, TCHAR* szPrefix) {
	WCHAR szPath[MAX_PATH];
	DISPLAY_DEVICE dd;
	dd.cb = sizeof(dd);
	wcout << szPrefix << "Color Profile for " << szDisplayDeviceName << endl;
	if (EnumDisplayDevices(szDisplayDeviceName, 0, &dd, EDD_GET_DEVICE_INTERFACE_NAME)) {
		wcout
			<< szPrefix << "    DeviceName=" << dd.DeviceName << endl
			<< szPrefix << "    DeviceString=" << dd.DeviceString << endl
			<< szPrefix << "    DeviceID=" << dd.DeviceID << endl
			<< szPrefix << "    DeviceKey=" << dd.DeviceKey << endl
			<< szPrefix << "    StateFlags=" << dd.StateFlags
			<< szPrefix << " " << ((dd.StateFlags & DISPLAY_DEVICE_ACTIVE) ? "Active" : "")
			<< szPrefix << " " << ((dd.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE) ? "Primary" : "")
			<< endl;
		if (WcsGetDefaultColorProfile(WCS_PROFILE_MANAGEMENT_SCOPE_CURRENT_USER,
			dd.DeviceKey,  // Use DeviceKey for EDD_GET_DEVICE_INTERFACE_NAME, not Device.Name
			CPT_ICC,
			CPST_PERCEPTUAL,
			1,  // dwProfileID -- doesn't seem to matter what value you use here
			MAX_PATH * sizeof(WCHAR),
			szPath)) {
			wcout << szPrefix << "    Profile: " << szPath << endl;
#if 0
			PROFILE profile;
			profile.cbDataSize = (DWORD)(wcslen(szPath) + 1) * sizeof(WCHAR);
			profile.dwType = PROFILE_FILENAME;
			profile.pProfileData = (PVOID)szPath;

			HPROFILE hProfile = OpenColorProfile(&profile,
				PROFILE_READ, FILE_SHARE_READ, OPEN_EXISTING);

			// now do something with the profile
#endif
		} else {
			wcout << szPrefix << _T("    Profile not found") << endl;
		}
	} else {
		wcout << szPrefix << _T("    Device not found") << endl;
	}
}

int main() {
	TCHAR* szDisplayDeviceName = _T("\\\\.\\DISPLAY2");

#if 1
	getDisplays();
#else
	getProfile(szDisplayDeviceName, _T(""));
#endif

	return 0;
}

