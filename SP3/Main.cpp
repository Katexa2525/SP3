#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <iostream>
#include <vector>
#include <tlhelp32.h>

void StaticImport(DWORD pid);
void DynamicImport(DWORD pid);
void Injection(DWORD pid);


const char FUNCTION_NAME[] = "ReplaceStringInMemory";

#define REPLACING_STR "Kitti"
#define DLL_PATH "InjectionDll.dll"
#define SOURCE_STRING "Katya"

#pragma comment(lib, "c:\\Users\\Катя\\Desktop\\5 сем\\СП3\\OSaSP_Lab3_Injection\\OSaSP_Lab3_Injection\\x64\\Debug\\InjectionDll.lib")

extern "C" _declspec(dllimport) void __stdcall ReplaceStringInMemory(DWORD PID, const char* srcString, const char* destString);
typedef int(__cdecl* TReplaceMemoryFunc)(DWORD PID, const char*, const char*);


struct InjectParams {
    DWORD pid;
    char srcStr[10];
    char OutStr[10];
};


int main()
{
  DWORD pid = GetCurrentProcessId();
  char data1[] = SOURCE_STRING;
  DWORD injectionPID = 0;
  printf("+--------------------------------------+--------------------------+--------------------------+\n");
  printf("|          Process Information         |          Value           |      Source String       |\n");
  printf("+--------------------------------------+--------------------------+--------------------------+\n");
  printf("| %-36s | %-24lu | %-24s |\n", "Process ID", pid, "");  // Initial line
  printf("|--------------------------------------|--------------------------|--------------------------|\n");

  printf("| %-36s | %-24s | %-24s |\n", "Static Import", "", "");  // Initial line
  StaticImport(pid);
  printf("| %-36s | %-24s | %-24s |\n", "Changed String: ", data1, SOURCE_STRING);

  printf("|--------------------------------------|--------------------------|--------------------------|\n");

  printf("| %-36s | %-24s | %-24s |\n", "Dynamic Import", "", "");  // Initial line
  DynamicImport(pid);
  printf("| %-36s | %-24s | %-24s |\n", "Changed String: ", data1, REPLACING_STR);

  printf("|--------------------------------------|--------------------------|--------------------------|\n");

  printf("| %-36s | %-24s | %-24s |\n", "Enter Injection Process ID", "", "");  // Initial line
  printf("|--------------------------------------|--------------------------|--------------------------|\n");
  scanf("%lu", &injectionPID);

  Injection(injectionPID);

  return 0;
}

void StaticImport(DWORD pid)
{
  ReplaceStringInMemory(pid, SOURCE_STRING, REPLACING_STR);
}

void DynamicImport(DWORD pid)
{
  HMODULE hDll = LoadLibraryA(DLL_PATH);
  if (hDll == NULL)
    return;

  TReplaceMemoryFunc func = (TReplaceMemoryFunc)GetProcAddress(hDll, FUNCTION_NAME);
  func(pid, REPLACING_STR, SOURCE_STRING);
  FreeLibrary(hDll);
}


void Injection(DWORD pid) {

    HANDLE process = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);

    if (process == NULL) {
        printf("Cant't open the procces with id - ");
        //std::cout << "Cant't open the procces with id - " << pid << std::endl;
        exit(1);
    }

    #pragma warning(suppress : 6387)
    LPVOID loadLibraryPointer = (LPVOID)GetProcAddress(GetModuleHandle(L"kernel32.dll"), "LoadLibraryA");

    if (loadLibraryPointer == NULL) {
        printf("Can't get pointer on LoadLibraryA!");
        exit(-1);
    }

    LPVOID allocForStrings = VirtualAllocEx(process, NULL, 1024, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (allocForStrings == NULL) {
        printf("Can't alloc for strings!");
        //std::cout << "Can't alloc for strings!" << std::endl;
        exit(-1);
    }
    if (WriteProcessMemory(process, allocForStrings, DLL_PATH, strlen(DLL_PATH) + 1, NULL) == 0) {
        printf("Can't write dll name in string alloc!");
        //std::cout << "Can't write dll name in string alloc!" << std::endl;
        exit(-1);
    }


    LPVOID allocParamPtr = VirtualAllocEx(process, NULL, strlen(DLL_PATH) + 1, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

    if (allocParamPtr == NULL) {
      printf("Can't alloc memory in process for injecting dll!");
        //std::cout << "Can't alloc memory in process for injecting dll!" << std::endl;
        exit(-1);
    }


    if (WriteProcessMemory(process, allocParamPtr, &allocForStrings, sizeof(LPVOID), NULL) == 0) {
      printf("Can't write dll name in alloc memory!");
        //std::cout << "Can't write dll name in alloc memory!" << std::endl;
        exit(-1);
    }

    HANDLE thread = CreateRemoteThread(process, NULL, 0, (LPTHREAD_START_ROUTINE)loadLibraryPointer, allocForStrings, 0, NULL);
    if (thread == NULL) {
      printf("Can't create thread for dll injection");
        //std::cout << "Can't create thread for dll injection" << std::endl;
        exit(-1);
    }
    WaitForSingleObject(thread, INFINITE);
    CloseHandle(thread);


    #pragma warning(suppress : 6387)
    LPVOID replaceFuncPointer = (LPVOID)GetProcAddress(LoadLibraryA(DLL_PATH), "ForDllInject");

    if (replaceFuncPointer == NULL) {
      printf("Can't get address of replace string func in injected dll");
        //std::cout << "Can't get address of replace string func in injected dll" << std::endl;
        exit(-1);
    }

    int paramSize = strlen(SOURCE_STRING) + 1;

    char* buffer = (char*)calloc(paramSize, sizeof(char));
    if (buffer == NULL) {
      printf("Can't alloc memmory to buffer");
        //std::cout << "Can't alloc memmory to buffer" << std::endl;
        exit(-1);
    }
    memcpy(buffer, SOURCE_STRING, strlen(SOURCE_STRING) + 1);

    if (WriteProcessMemory(process, allocForStrings, buffer, paramSize, NULL) == 0) {
        printf("Can't write alloc string in memory");
        //std::cout << "Can't write alloc string in memory" << std::endl;
        exit(-1);
    }

    struct InjectParams params = {
    pid,
    SOURCE_STRING,
    REPLACING_STR
    };

    LPVOID allocPwnParamPtr = VirtualAllocEx(process, NULL, sizeof(params), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (allocPwnParamPtr == NULL) {
        printf("Can't allocate memory in process for replace string func");
        //std::cout << "Can't allocate memory in process for replace string func" << std::endl;
        exit(-11);
    }

    if (!WriteProcessMemory(process, allocPwnParamPtr, &params, sizeof(params), NULL)) {
        printf("Can't write function parameters to memory");
        //std::cout << "Can't write function parameters to memory" << std::endl;
        exit(-1);
    }

    thread = CreateRemoteThread(process, NULL, 0, (LPTHREAD_START_ROUTINE)replaceFuncPointer, allocPwnParamPtr, 0, NULL);

    if (thread == NULL) {
        printf("Can't create thread for replace string func");
        exit(-11);
    }


    WaitForSingleObject(thread, INFINITE);
    CloseHandle(thread);
    CloseHandle(process);

    free(buffer);
}



