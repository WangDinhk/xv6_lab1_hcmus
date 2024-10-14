#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#define RD 0
#define WR 1

int main(int argc, char *argv[])
{
	int pp[2];	
	int prime;
	int data;
	int pid=0;

	
	pipe(pp);
	for (int i=2; i<=35; i++)
		write(pp[WR], &i, sizeof(int));
	close(0);
	dup(pp[RD]);
	close(pp[RD]);
	close(pp[WR]);
	prime=2;
	printf("prime 2\n");

	while (read(0, &data, sizeof(int))) {
		if (data % prime != 0) {
			if (!pid) {	
				pipe(pp);
				pid=fork();
				if (pid) {	
					close(pp[RD]);
				} else {	
					close(0);
					dup(pp[RD]);
					close(pp[RD]);
					close(pp[WR]);
					prime=data;
					printf("prime %d\n", data);
				}
			}
			write(pp[WR], &data, sizeof(int));
		}
	}
	close(pp[WR]);
	wait(0);
	exit(0);
}