// ColorManagementConsole.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <Windows.h>
#include <iostream>
#include <Icm.h>
#include <strsafe.h>

using namespace std;

TCHAR* getErrorMsg() {
	TCHAR buf[256];
	FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), buf, 256, NULL);
	return buf;
}

void displayLastError(LPTSTR lpszFunction)
{
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

void getDeviceNames() {
	WCHAR szPath[MAX_PATH];
	DISPLAY_DEVICE dd;
	dd.cb = sizeof(dd);
	int devNum = 0;
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
		if (dd.StateFlags & DISPLAY_DEVICE_ACTIVE) {
			WCHAR szPath[MAX_PATH];
			if (WcsGetDefaultColorProfile(WCS_PROFILE_MANAGEMENT_SCOPE_CURRENT_USER,
				dd.DeviceID,
				CPT_ICC,
				CPST_PERCEPTUAL, // Or CPST_ABSOLUTE_COLORIMETRIC,
				devNum,  // dwProfileID -- doesn't seem to matter what value you use here
				MAX_PATH * sizeof(WCHAR),
				szPath)) {

				wcout << "    Profile Name" << szPath << endl;

#if 0
				PROFILE profile;
				profile.cbDataSize = (DWORD)(wcslen(szPath) + 1) * sizeof(WCHAR);
				profile.dwType = PROFILE_FILENAME;
				profile.pProfileData = (PVOID)szPath;

				HPROFILE hProfile = OpenColorProfile(&profile,
					PROFILE_READ, FILE_SHARE_READ, OPEN_EXISTING);

				// Now do something with the profile
				wcout << "    Profile Name" << profile.pProfileData << endl;

				// Close the profile
				CloseColorProfile(hProfile);
#endif
			} else {
				cout << "WcsGetDefaultColorProfile failed" << endl;
				displayLastError(_T("WcsGetDefaultColorProfile failed"));
			}
		}
		devNum++;
	}
}


void getProfile() {
	WCHAR szPath[MAX_PATH];
	DISPLAY_DEVICE dd;
	dd.cb = sizeof(dd);
	TCHAR* szDisplayDeviceName = _T("\\.\\DISPLAY1");
	if (EnumDisplayDevices(NULL, 0, &dd, EDD_GET_DEVICE_INTERFACE_NAME)) {
		if (WcsGetDefaultColorProfile(WCS_PROFILE_MANAGEMENT_SCOPE_CURRENT_USER,
			dd.DeviceKey,
			CPT_ICC,
			CPST_PERCEPTUAL,
			1,  // dwProfileID -- doesn't seem to matter what value you use here
			MAX_PATH * sizeof(WCHAR),
			szPath)) {
			wcout << "Color Profile" << szPath << endl;
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
			cout << "WcsGetDefaultColorProfile failed" << endl;
			displayLastError(_T("WcsGetDefaultColorProfile failed"));
		}
	} else {
		cout << "EnumDisplayDevices failed" << endl;
		displayLastError(_T("EnumDisplayDevices failed"));
	}
}

int main()
{
	//getProfile();
	getDeviceNames();

	return 0;
}

