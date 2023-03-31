#pragma once
#include <Windows.h>
#include <winternl.h>
#include <cstdio>
#include <strsafe.h>
#include <iostream>

namespace SDK::PE {
	HANDLE GetFileContent();

	int Main();

}

