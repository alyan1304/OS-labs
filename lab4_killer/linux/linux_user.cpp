#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <sstream>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <cstring>

bool isPidAlive(pid_t pid) {
    if (pid <= 0) return false;
    
    if (kill(pid, 0) == 0) {
        return true;
    }
    
    if (errno == ESRCH) {
        return false;
    }
    
    char path[64];
    snprintf(path, sizeof(path), "/proc/%d", pid);
    struct stat sb;
    return (stat(path, &sb) == 0);
}

std::vector<pid_t> getPidsByName(const std::string& name) {
    std::vector<pid_t> out;
    std::string cmd = "pgrep -x \"" + name + "\"";
    FILE* pipe = popen(cmd.c_str(), "r");
    if (!pipe) return out;
    
    char buf[128];
    while (fgets(buf, sizeof(buf), pipe)) {
        pid_t p = atoi(buf);
        if (p > 0 && isPidAlive(p)) {
            out.push_back(p);
        }
    }
    pclose(pipe);
    return out;
}

void launchProcess(const char* processName) {
    pid_t pid = fork();
    if (pid == 0) {
        execlp(processName, processName, (char*)NULL);
        _exit(127); 
    }
    std::cout << "Launched: " << processName << std::endl;
    usleep(1000000); 
}

bool runKiller(pid_t pid, const char* processName) {
    pid_t child = fork();
    if (child == 0) {
        std::string pidStr = std::to_string(pid);
        const char* pidCStr = pidStr.c_str();
        execl("./killer", "killer", pidCStr, processName, (char*)NULL);
        _exit(127);
    }
    
    int status = 0;
    waitpid(child, &status, 0);
    
    usleep(500000);
    
    return (WIFEXITED(status) && WEXITSTATUS(status) == 0);
}

bool isProcessRunningByName(const std::string& name) {
    std::string cmd = "pgrep -x \"" + name + "\" > /dev/null 2>&1";
    int result = system(cmd.c_str());
    
    if (result == -1) {
        return false;
    }
    
    return (WEXITSTATUS(result) == 0);
}

void checkProcesses(pid_t pid, const char* name) {
    const char* envValue = std::getenv("PROC_TO_KILL");
    
    if (envValue) {
        std::vector<std::string> processNames;
        std::stringstream stream(envValue);
        std::string procName;
        
        while (std::getline(stream, procName, ',')) {
            size_t start = procName.find_first_not_of(" \t");
            size_t end = procName.find_last_not_of(" \t");
            if (start != std::string::npos && end != std::string::npos) {
                procName = procName.substr(start, end - start + 1);
                if (!procName.empty()) {
                    std::cout << "from PROC_TO_KILL: " << procName << (isProcessRunningByName(procName) ? " RUNNING" : " NOT RUNNING") << std::endl;
                }
            }
        }
    } else {
        std::cout << "Error: could not get PROC_TO_KILL environment variable value" << std::endl;
    }
    
    if (name != nullptr) {
        std::cout << name << (isProcessRunningByName(name) ? " RUNNING" : " NOT RUNNING") << std::endl;
    }
    
    if (pid != 0) {
        std::cout << "process with PID " << pid << (isPidAlive(pid) ? " RUNNING" : " NOT RUNNING") << std::endl;
    }
}

void testKillingProcesses(const std::vector<pid_t>& pids, const char* processName) {
    std::cout << "--------------------" << std::endl;

    for (pid_t pid : pids) {
        checkProcesses(pid, processName);
    }
    
    std::cout << "Killing processes...";
    
    bool allKilled = true;
    for (pid_t pid : pids) {
        if (!runKiller(pid, processName)) {
            allKilled = false;
        }
    }
    
    if (allKilled) {
        std::cout << "Killer executed successfully" << std::endl;
        usleep(1000000); 
        
        for (pid_t pid : pids) {
            checkProcesses(pid, processName);
        }
    } else {
        std::cout << "Killer failed" << std::endl;
    }
}

int main() {
    const char* processesToKill = "xcalc";
    setenv("PROC_TO_KILL", processesToKill, 1);
    
    launchProcess("xclock");
    launchProcess("firefox");
    
    std::vector<pid_t> p1 = getPidsByName("xclock");
    
    testKillingProcesses(p1, "firefox");

    launchProcess("xclock");
    launchProcess("firefox");

    std::vector<pid_t> p2 = getPidsByName("firefox");

    testKillingProcesses(p2, "xclock");
    
    return 0;
}