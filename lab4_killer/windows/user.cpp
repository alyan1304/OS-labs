#include <windows.h>
#include <iostream>
#include <tlhelp32.h>
#include <cstring>
#include <vector>
#include <sstream>
#include <string>
#include <cstdlib>


// 2. Приложение «User»
// Должно определить переменную окружения PROC_TO_KILL 
// (значение задать прямо в коде), 
// запустить процесс Killer со всеми возможными вариантами параметров
//  и продемонстрировать его работу (Проверка, что в системе до выполнения Killer был процесс с определённым именем\id,
//  а после его нет). 
// Функция для проверки существования процесса по имени

DWORD getProcessIdByName(const char* processName) {
    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(PROCESSENTRY32);

    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    
    if (snapshot == INVALID_HANDLE_VALUE) {
        return 0; 
    }

    DWORD pid = 0;

    if (Process32First(snapshot, &entry)) {
        do {
            #ifdef UNICODE
            char exeFile[MAX_PATH];
            WideCharToMultiByte(CP_ACP, 0, entry.szExeFile, -1, exeFile, MAX_PATH, NULL, NULL);
            #else
            const char* exeFile = entry.szExeFile;
            #endif

            if (_stricmp(exeFile, processName) == 0) {
                pid = entry.th32ProcessID;
                break;
            }
        } while (Process32Next(snapshot, &entry));
    }
    CloseHandle(snapshot);
    return pid;
}

bool isProcessRunning(DWORD pid) {
    if (pid == 0) return false;
    HANDLE handle = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, pid);
    if (handle != NULL) {
        DWORD exitCode;
        if (GetExitCodeProcess(handle, &exitCode)) {
            CloseHandle(handle);
            return (exitCode == STILL_ACTIVE);
        }
        CloseHandle(handle);
    }
    return false;
}

bool isProcessRunning(const char* processName) {
    DWORD id = getProcessIdByName(processName);
    bool result = isProcessRunning(id);
    return result;
}

bool runKiller(DWORD pid, const char* processName) {
    std::string command = "killer.exe " + std::to_string(pid) + " " + processName;
    
    STARTUPINFOA si = { sizeof(si) };
    PROCESS_INFORMATION pi;
    
    if (CreateProcessA(
        NULL,                      
        const_cast<char*>(command.c_str()), 
        NULL,                   
        NULL,                         
        FALSE,                        
        0,                           
        NULL,                   
        NULL,                           
        &si,                           
        &pi                             
    )) {

        WaitForSingleObject(pi.hProcess, INFINITE);
        
        DWORD exitCode;
        GetExitCodeProcess(pi.hProcess, &exitCode);
        
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        
        return (exitCode == 0);
    }
    
    return false;
}

void checkProcesses(DWORD pid, const char* name) {
    char buffer[256];
    if (GetEnvironmentVariableA("PROC_TO_KILL", buffer, sizeof(buffer)) > 0) {
        std::stringstream ss(buffer);
        std::string processNameInVar;
        while (std::getline(ss, processNameInVar, ',')) {
            if (!processNameInVar.empty()) {

            std::cout << "from PROC_TO_KILL: " << processNameInVar << (isProcessRunning(processNameInVar.c_str()) ? " RUNNING" : " NOT RUNNING") << std::endl;
            }   
        }
    } else {
        std::cout << "Error: could not get PROC_TO_KILL environment variable value";
    }


    if(name != nullptr) {
        std::cout << name << (isProcessRunning(name) ? " RUNNING" : " NOT RUNNING") << std::endl;
    }
    
    if (pid != 0) {
        std::cout << "process with PID " << pid << (isProcessRunning(pid) ? " RUNNING" : " NOT RUNNING") << std::endl;
    }
}

void testKillingProcesses(DWORD pid, const char* processName) {
    std::cout << "--------------------" << std::endl;
    checkProcesses(pid, processName);
    std::cout << "Killing processes...";
    if (runKiller(pid, processName)) {
        std::cout << "Killer executed successfully" << std::endl;
        Sleep(1000);
        checkProcesses(pid, processName);
    } else {
        std::cout << "Killer failed" << std::endl;
    }
}

void launchProcess(const char* processName) {
    std::string command = "start " + std::string(processName);
    system(command.c_str());
    std::cout << "Launched: " << processName << std::endl;
    Sleep(1000);
}

   


int main() {
    const char* processesToKill = "chrome.exe,Telegram.exe";
    SetEnvironmentVariableA("PROC_TO_KILL", processesToKill);

    launchProcess("notepad.exe");
    launchProcess("taskmgr.exe");
    launchProcess("mspaint.exe");


    DWORD pid1 = getProcessIdByName("notepad.exe");
    DWORD pid2 = getProcessIdByName("taskmgr.exe");
    
    testKillingProcesses(pid1, "calc.exe");

    testKillingProcesses(pid2, "mspaint.exe");

    return 0;
}
