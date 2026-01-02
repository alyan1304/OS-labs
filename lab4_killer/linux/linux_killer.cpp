#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <sstream>
#include <signal.h>
#include <sys/types.h>

void killProcessesFromEnvironment();  
void killProcessByPID(const char* pid);
void killProcessByName(const char* name);
std::vector<pid_t> getPidsByName(const std::string& processName);

int main(int argc, char* argv[]){
    if(argc != 3){
        std::cerr << "Wrong number of arguments!\n";
        return 1;
    }
    killProcessesFromEnvironment();
    killProcessByPID(argv[1]);
    killProcessByName(argv[2]);
    return 0;
}


void killProcessByPID(const char* pid) {
    pid_t processId = atoi(pid);
    
    if (processId <= 0) {
        std::cerr << "Invalid PID: " << pid << std::endl;
        return;
    }

    if (kill(processId, SIGTERM) == 0) {
        std::cout << "Sent SIGTERM to process " << processId << std::endl;
    } else {
        if (kill(processId, SIGKILL) == 0) {
            std::cout << "Sent SIGKILL to process " << processId << std::endl;
        } else {
            std::cerr << "Failed to kill process.";
        }
    }
}



void killProcessByName(const char* name) {
    std::vector<pid_t> pids = getPidsByName(name);
    
    if (pids.empty()) {
        std::cout << "No processes found with name: " << name << std::endl;
        return;
    }
    
    for (pid_t pid : pids) {
        std::cout << "Killing process " << pid << std::endl;
        
        if (kill(pid, SIGTERM) != 0) {
            if (kill(pid, SIGKILL) != 0) {
                std::cerr << "Failed to kill process.";
            }
        }
    }
}

std::vector<pid_t> getPidsByName(const std::string& processName) {
    std::vector<pid_t> pids;
    
    std::string command = "pgrep " + processName;
    
    FILE* pipe = popen(command.c_str(), "r");
    if (!pipe) {
        std::cerr << "Error: cannot execute pgrep" << std::endl;
        return pids;
    }
    
    char buffer[128];
    while (fgets(buffer, sizeof(buffer), pipe)) {
        pid_t pid = atoi(buffer);
        pids.push_back(pid);
    }
    
    pclose(pipe);
    
    return pids;
}

void killProcessesFromEnvironment() {
    const char* envValue = std::getenv("PROC_TO_KILL");
    
    if (!envValue) {
        std::cout << "PROC_TO_KILL environment variable not found or empty" << std::endl;
        return;
    }
    
    std::vector<std::string> processNames;
    std::stringstream stream(envValue);
    std::string name;

    while (std::getline(stream, name, ',')) {
        size_t start = name.find_first_not_of(" \t");
        size_t end = name.find_last_not_of(" \t");
        if (start != std::string::npos && end != std::string::npos) {
            processNames.push_back(name.substr(start, end - start + 1));
        }
    }

    for (const std::string& procName : processNames) {
        if (!procName.empty()) {
            std::cout << "Killing processes from environment: " << procName << std::endl;
            killProcessByName(procName.c_str());
        }
    }
}