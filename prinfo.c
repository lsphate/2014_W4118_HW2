#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/syscall.h>
#include "prinfo.h"
#include <errno.h>
#include <string.h>

int main(int argc, char **argv)
{
	int x;
	int nr = 70;
	int layer =  1;
	struct prinfo buf[nr];

	x = syscall(223, buf, &nr);
	if (x != 0)
		printf("error: %s\n", strerror(errno));

	int i = 0;

	printf("%s,%d,%ld,%d,%d,%d,%ld\n",
buf[i].comm, buf[i].pid, buf[i].state,
buf[i].parent_pid, buf[i].first_child_pid, buf[i].next_sibling_pid, buf[i].uid);

	i++;
	while (i < nr && buf[i].pid != 0) {
		if (buf[i].parent_pid == 0)
			layer = 1;
		int j;

		for (j = 0 ; j < layer ; j++)
			printf("\t");

		printf("%s,%d,%ld,%d,%d,%d,%ld\n",
buf[i].comm, buf[i].pid, buf[i].state,
buf[i].parent_pid, buf[i].first_child_pid, buf[i].next_sibling_pid, buf[i].uid);

		if (buf[i].next_sibling_pid == 0) {
			if (buf[i].first_child_pid != 0)
				layer++;
			else
				layer--;
		} else if (buf[i].first_child_pid != 0) {
			layer++;
		}
	i++;
	}
	return 0;
}
