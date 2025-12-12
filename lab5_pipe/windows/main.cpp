#include <windows.h>
#include <iostream>
#include <cstring>

int main() {
    SECURITY_ATTRIBUTES sa = {sizeof(sa), NULL, TRUE};
    
    HANDLE hRead1, hWrite1;  // сюда будут помещены хендлы первого пайпа (при его создании)
    HANDLE hRead2, hWrite2;  
    HANDLE hRead3, hWrite3;  
    HANDLE hRead4, hWrite4;  

    // Родитель → [hWrite1] → PIPE1 → [hRead1] → M.exe → [hWrite2] → PIPE2 → [hRead2] → A.exe
    // → [hWrite3] → PIPE3 → [hRead3] → P.exe → [hWrite4] → PIPE4 → [hRead4] → S.exe → Консоль
    
    if (!CreatePipe(&hRead1, &hWrite1, &sa, 0) ||
        !CreatePipe(&hRead2, &hWrite2, &sa, 0) ||
        !CreatePipe(&hRead3, &hWrite3, &sa, 0) ||
        !CreatePipe(&hRead4, &hWrite4, &sa, 0)) {
        std::cerr << "failed to create pipes" << std::endl;
        return 1;
    }

    STARTUPINFOA siM = {sizeof(siM)};
    PROCESS_INFORMATION piM;
    siM.dwFlags = STARTF_USESTDHANDLES; // устанавливаем свои потоки ввода/вывода, а не дефолтные консольные
    siM.hStdInput = hRead1;
    siM.hStdOutput = hWrite2;
    siM.hStdError = GetStdHandle(STD_ERROR_HANDLE); // устанавливаем поток вывода явно, так как дефолтный уже не учитывается
    
    char cmdM[] = "M.exe";
    if (!CreateProcessA(NULL, cmdM, NULL, NULL, TRUE, 0, NULL, NULL, &siM, &piM)) {
        std::cerr << "failed to create M process" << std::endl;
        return 1;
    }

    CloseHandle(hRead1);   
    CloseHandle(hWrite2);  
    
    STARTUPINFOA siA = {sizeof(siA)};
    PROCESS_INFORMATION piA;
    siA.dwFlags = STARTF_USESTDHANDLES;
    siA.hStdInput = hRead2;
    siA.hStdOutput = hWrite3;
    siA.hStdError = GetStdHandle(STD_ERROR_HANDLE);
    
    char cmdA[] = "A.exe";
    if (!CreateProcessA(NULL, cmdA, NULL, NULL, TRUE, 0, NULL, NULL, &siA, &piA)) {
        std::cerr << "failed to create A process" << std::endl;
        return 1;
    }

    CloseHandle(hRead2);
    CloseHandle(hWrite3);
    
    STARTUPINFOA siP = {sizeof(siP)};
    PROCESS_INFORMATION piP;
    siP.dwFlags = STARTF_USESTDHANDLES;
    siP.hStdInput = hRead3;
    siP.hStdOutput = hWrite4;
    siP.hStdError = GetStdHandle(STD_ERROR_HANDLE);
    
    char cmdP[] = "P.exe";
    if (!CreateProcessA(NULL, cmdP, NULL, NULL, TRUE, 0, NULL, NULL, &siP, &piP)) {
        std::cerr << "failed to create P process" << std::endl;
        return 1;
    }
   
    CloseHandle(hRead3);
    CloseHandle(hWrite4);
    
    STARTUPINFOA siS = {sizeof(siS)};
    PROCESS_INFORMATION piS;
    siS.dwFlags = STARTF_USESTDHANDLES;
    siS.hStdInput = hRead4;
    siS.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
    siS.hStdError = GetStdHandle(STD_ERROR_HANDLE);
    
    char cmdS[] = "S.exe";
    if (!CreateProcessA(NULL, cmdS, NULL, NULL, TRUE, 0, NULL, NULL, &siS, &piS)) {
        std::cerr << "failed to create S process" << std::endl;
        return 1;
    }
 
    CloseHandle(hRead4);

    const char* data = "1 2 3 4 5\n"; 
    DWORD bytesWritten;
    WriteFile(hWrite1, data, strlen(data), &bytesWritten, NULL); // записывает на вход первому пайпу наши данные
    CloseHandle(hWrite1); // Закрываем записывающий конец, чтобы сигнализировать конец данных
    
    HANDLE hProcesses[4] = {piM.hProcess, piA.hProcess, piP.hProcess, piS.hProcess};
    WaitForMultipleObjects(4, hProcesses, TRUE, INFINITE); // Ждем завершения всех процессов
    
    for (int i = 0; i < 4; i++) {
        CloseHandle(hProcesses[i]);
        CloseHandle((i == 0) ? piM.hThread : (i == 1) ? piA.hThread : 
                    (i == 2) ? piP.hThread : piS.hThread);
    }
    
    return 0;
}