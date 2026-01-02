#include <iostream>
#include <windows.h>
#include <tlhelp32.h>
#include <cstring>
#include <vector>
#include <sstream>
#include <string>

const DWORD MAX_PID = 0xFFFF;
const DWORD MIN_PID = 4;
void KillProcessesFromEnvironment(); 
void killProcessByPID(const char* pid);
void KillProcessByName(const char* name);

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Wrong number of arguments (3 required)" << std::endl;
        return 1;
    }

    killProcessByPID(argv[1]);
    KillProcessByName(argv[2]);
    KillProcessesFromEnvironment();

    return 0;
}

void killProcessByPID(const char* id) {
    DWORD pid = atoi(id);
    if (pid >= MIN_PID && pid <= MAX_PID) {
        HANDLE handle = OpenProcess(PROCESS_TERMINATE, FALSE, pid);
        if (handle != NULL) {
            TerminateProcess(handle, 0);
            CloseHandle(handle);
        }
    }
}

void KillProcessByName(const char* name) {
    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(PROCESSENTRY32);

    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    
    if (snapshot == INVALID_HANDLE_VALUE) {
        return;
    }

    if (Process32First(snapshot, &entry)) {
        do {
           //Convert the process name to ANSI if we're in Unicode build
            #ifdef UNICODE
            char exeFile[MAX_PATH];
            WideCharToMultiByte(CP_ACP, 0, entry.szExeFile, -1, exeFile, MAX_PATH, NULL, NULL);
            #else
            const char* exeFile = entry.szExeFile;
            #endif

            if (_stricmp(exeFile, name) == 0) {
                HANDLE handle = OpenProcess(PROCESS_TERMINATE, FALSE, entry.th32ProcessID);
                if (handle != NULL) {
                    TerminateProcess(handle, 0);
                    CloseHandle(handle);
                }
            }
        } while (Process32Next(snapshot, &entry));
    }
    CloseHandle(snapshot);
}

void KillProcessesFromEnvironment() {
    char envBuffer[4096];

    DWORD result = GetEnvironmentVariableA("PROC_TO_KILL", envBuffer, sizeof(envBuffer));
    
    if (result == 0) {
        std::cout << "PROC_TO_KILL environment variable not found or empty" << std::endl;
        return;
    }
    
    if (result > sizeof(envBuffer)) {
        std::cerr << "Environment variable too long" << std::endl;
        return;
    }
    
    std::vector<std::string> processNames;
    std::stringstream ss(envBuffer);
    std::string name;
    
    while (std::getline(ss, name, ',')) {
        if (!name.empty()) {
            processNames.push_back(name);
        }
    }
    
    for (const std::string& procName : processNames) {
        KillProcessByName(procName.c_str());
    }
}