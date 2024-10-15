#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define MAX_NUM 280

void findPrimes() {
    int numbers[MAX_NUM - 1];
    int i, j, n = 0;

    for (i = 2; i <= MAX_NUM; i++) {
        numbers[n++] = i;
    }

    for (i = 0; i < n; i++) {
        if (numbers[i] != 0) {
            printf("prime %d\n", numbers[i]);
            
            for (j = i + 1; j < n; j++) {
                if (numbers[j] % numbers[i] == 0) {
                    numbers[j] = 0;
                }
            }
        }
    }
}

int main(int argc, char *argv[]) {
    findPrimes();
    exit(0);
}