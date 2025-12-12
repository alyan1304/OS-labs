#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <sys/wait.h>
#include <array>

int main() {
    // pipe[0] - чтение, pipe[1] - запись (аналогично HANDLE hRead/hWrite)
    int pipe1[2], pipe2[2], pipe3[2], pipe4[2]; // просто массивы целых чисел, куда будут складываться "хендлы"
    
    if (pipe(pipe1) == -1 || pipe(pipe2) == -1 || 
        pipe(pipe3) == -1 || pipe(pipe4) == -1) {
        std::cerr << "failed to create pipes" << std::endl;
        return 1;
    }

    // Родитель → [pipe1[1]] → PIPE1 → [pipe1[0]] → M.exe → [pipe2[1]] → PIPE2 → [pipe2[0]] → A.exe
    // → [pipe3[1]] → PIPE3 → [pipe3[0]] → P.exe → [pipe4[1]] → PIPE4 → [pipe4[0]] → S.exe → Консоль

    pid_t pidM = fork();
    if (pidM == 0) {
        close(pipe1[1]); //процесс M не будет использовать эти хендлы - ему они не нужны
        close(pipe2[0]);
        
        //перенаправление стандартных потоков
        dup2(pipe1[0], STDIN_FILENO);  // создается КОПИЯ pipe1[0]
        dup2(pipe2[1], STDOUT_FILENO); 
        
        close(pipe1[0]); //закрываем хендлы внутри дочернего процесса M
        close(pipe2[1]);
        close(pipe3[0]); 
        close(pipe3[1]);
        close(pipe4[0]); 
        close(pipe4[1]);
        
        execl("./M", "./M", NULL);
        
        std::cerr << "failed to execute M" << std::endl;
        exit(1);
    }
    
    close(pipe1[0]);  // больше нам вообще не понадобятся (нужны были только процессу M)
    close(pipe2[1]);   

    pid_t pidA = fork();
    if (pidA == 0) {        
        close(pipe2[1]); 
        close(pipe3[0]); 
        
        dup2(pipe2[0], STDIN_FILENO);  
        dup2(pipe3[1], STDOUT_FILENO); 
        
        close(pipe2[0]);
        close(pipe3[1]);
        close(pipe1[0]); close(pipe1[1]);
        close(pipe4[0]); close(pipe4[1]);
        
        execl("./A", "./A", NULL);
        std::cerr << "failed to execute A" << std::endl;
        exit(1);
    }
    
    close(pipe2[0]);   
    close(pipe3[1]);   

    
    pid_t pidP = fork();
    if (pidP == 0) {
        close(pipe3[1]);  
        close(pipe4[0]); 
        
        dup2(pipe3[0], STDIN_FILENO);  
        dup2(pipe4[1], STDOUT_FILENO); 
        
        close(pipe3[0]);
        close(pipe4[1]);
        close(pipe1[0]); close(pipe1[1]);
        close(pipe2[0]); close(pipe2[1]);
        
        execl("./P", "./P", NULL);
        std::cerr << "failed to execute P" << std::endl;
        exit(1);
    }
    
    close(pipe3[0]);   
    close(pipe4[1]);  


    pid_t pidS = fork();
    if (pidS == 0) {
        
        close(pipe4[1]); 
        
        dup2(pipe4[0], STDIN_FILENO);   
        // STDOUT остается стандартным (вывод в консоль)
        
        close(pipe4[0]);
        close(pipe1[0]); close(pipe1[1]);
        close(pipe2[0]); close(pipe2[1]);
        close(pipe3[0]); close(pipe3[1]);
        
        execl("./S", "./S", NULL);
        std::cerr << "failed to execute S" << std::endl;
        exit(1);
    }
    
    close(pipe4[0]);   

    const char* data = "1 2 3 4 5\n";
    write(pipe1[1], data, strlen(data));  // родительский процесс записывает на вход первому пайпу наши данные
    close(pipe1[1]);  // закрываем записывающий конец, чтобы сигнализировать конец данных

    waitpid(pidM, NULL, 0); // ждем завершения всех процессов
    waitpid(pidA, NULL, 0);
    waitpid(pidP, NULL, 0);
    waitpid(pidS, NULL, 0);

    close(pipe1[1]); 
    
    return 0;
}