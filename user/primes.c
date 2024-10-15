#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define MAX 280

void primes(int read_pipe) {
    int p[2];
    int num;
    if (read(read_pipe, &num, sizeof(num)) <= 0) {
        close(read_pipe); 
        return;
    }
    printf("prime %d\n", num);
    if (pipe(p) < 0) {
        printf("pipe creation failed\n");
        exit(1);
    }
    int pid = fork();
    if (pid < 0) {
        printf("fork failed\n");
        exit(1);
    }
    if (pid == 0) {
        close(p[1]);  
        close(read_pipe);  
        primes(p[0]); 
        close(p[0]);  
    } else {
        close(p[0]);  
        int tmp;
        while (read(read_pipe, &tmp, 4) > 0) {
            if (tmp % num != 0) {
                write(p[1], &tmp, 4);
            }
        }

        close(p[1]);
        close(read_pipe); 
        wait(0); 
    }
}

int main(int argc, char *argv[]) {
    int p[2];
    pipe(p);
    int pid = fork();
    if (pid == 0) {
        close(p[1]);  
        primes(p[0]); 
        close(p[0]);  
    } else {
        close(p[0]); 
        for (int i = 2; i <= 280; i++) {
            write(p[1], &i, 4);  
        }
        close(p[1]);  
        wait(0);  
    }
    exit(0);
}