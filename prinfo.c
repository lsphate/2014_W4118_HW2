#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>
int main(int argc, char **argv) {
	int x;
	x = syscall(223);
	if (x == 0) printf("success\n");
	else perror("error");
	return 0;
}
