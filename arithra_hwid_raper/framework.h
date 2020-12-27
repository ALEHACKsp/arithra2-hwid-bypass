#pragma once
#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files
#include <windows.h>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <MinHook.h>
#include <thread>
#include <TlHelp32.h>
#include <regex>
#pragma comment(lib, "libMinHook.lib")