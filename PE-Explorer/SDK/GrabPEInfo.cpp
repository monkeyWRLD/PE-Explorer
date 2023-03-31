#include "GrabPEInfo.h"


OPENFILENAME ofn;       // common dialog box structure
LPWSTR szFile[260];       // buffer for file name
HWND hwnd;              // owner window
HANDLE hFile;              // file handle

HANDLE SDK::PE::GetFileContent()
{

    // Initialize OPENFILENAME
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hwnd;
    ofn.lpstrFile = (LPWSTR)szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = L"All\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    // Display the Open dialog box. 

    if (GetOpenFileName(&ofn) == TRUE)
        hFile = CreateFile(ofn.lpstrFile,
            GENERIC_READ,
            0,
            nullptr,
            OPEN_EXISTING,
            0,
            nullptr);

    
    if (hFile == INVALID_HANDLE_VALUE)
    {
        printf("[-] An error occured when trying to open the PE file !");
        CloseHandle(hFile);
        return nullptr;
    }

    const DWORD dFileSize = GetFileSize(hFile, nullptr);
    if (dFileSize == INVALID_FILE_SIZE)
    {
        printf("[-] An error occured when trying to get the PE file size !");
        CloseHandle(hFile);
        return nullptr;
    }

    const HANDLE hFileContent = HeapAlloc(GetProcessHeap(), 0, dFileSize);
    if (hFileContent == INVALID_HANDLE_VALUE)
    {
        printf("[-] An error occured when trying to allocate memory for the PE file content !");
        CloseHandle(hFile);
        CloseHandle(hFileContent);
        return nullptr;
    }

    const BOOL bFileRead = ReadFile(hFile, hFileContent, dFileSize, nullptr, nullptr);
    if (!bFileRead)
    {
        printf("[-] An error occured when trying to read the PE file content !");
        CloseHandle(hFile);
        if (hFileContent != nullptr)
            CloseHandle(hFileContent);

        return nullptr;
    }

    CloseHandle(hFile);
    return hFileContent;
}

int SDK::PE::Main()
{
    char* lpFilePath;


    const HANDLE hFileContent = SDK::PE::GetFileContent();
    if (hFileContent == INVALID_HANDLE_VALUE)
    {
        if (hFileContent != nullptr)
            CloseHandle(hFileContent);

        return -1;
    }

    const auto pImageDOSHeader = (PIMAGE_DOS_HEADER)hFileContent;
    if (pImageDOSHeader == nullptr)
    {
        if (hFileContent != nullptr)
        {
            HeapFree(hFileContent, 0, nullptr);
            CloseHandle(hFileContent);
        }
        return -1;
    }

    const auto pImageNTHeader = (PIMAGE_NT_HEADERS)((BYTE*)pImageDOSHeader + pImageDOSHeader->e_lfanew);
    if (pImageNTHeader == nullptr)
    {
        if (hFileContent != nullptr)
        {
            HeapFree(hFileContent, 0, nullptr);
            CloseHandle(hFileContent);
        }
        return -1;
    }

    const auto pImageFileHeader = (PIMAGE_FILE_HEADER)&pImageNTHeader->FileHeader;
    if (pImageFileHeader == nullptr)
    {
        if (hFileContent != nullptr)
        {
            HeapFree(hFileContent, 0, nullptr);
            CloseHandle(hFileContent);
        }
        return -1;
    }
    const auto pImageOptionalHeader = (PIMAGE_OPTIONAL_HEADER)&pImageNTHeader->OptionalHeader;
    if (pImageOptionalHeader == nullptr)
    {
        if (hFileContent != nullptr)
        {
            HeapFree(hFileContent, 0, nullptr);
            CloseHandle(hFileContent);
        }
        return -1;
    }
    const auto pImageSectionHeader = (PIMAGE_SECTION_HEADER)((BYTE*)pImageNTHeader + sizeof(IMAGE_NT_HEADERS) + (pImageFileHeader->NumberOfSections - 1) * sizeof(IMAGE_SECTION_HEADER));
    if (pImageSectionHeader == nullptr)
    {
        if (hFileContent != nullptr)
        {
            HeapFree(hFileContent, 0, nullptr);
            CloseHandle(hFileContent);
        }
        return -1;
    }

   

    if (hFileContent != nullptr)
        HeapFree(hFileContent, 0, nullptr);

    return 1;
}
