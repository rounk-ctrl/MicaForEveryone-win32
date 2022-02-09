// header.h : include file for standard system include files,
// or project specific include files
//

#pragma once

#include "targetver.h"
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files
#include <windows.h>
// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <shellapi.h>
#include <vector>
#include <dwmapi.h>
#pragma comment(lib, "dwmapi.lib")
#include <stdio.h>
#include <Uxtheme.h>
#pragma comment(lib, "Uxtheme.lib")
#include <vsstyle.h>
#pragma comment(lib, "Comctl32.lib")
