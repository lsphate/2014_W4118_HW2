#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/syscall.h>
#include "prinfo.h"

int main(int argc, char **argv) {
	
	int x;
	int nr =10 ;
	int layer ;
	
	struct prinfo buf[nr];

	x = syscall(223, buf, &nr);

	if (x == 0) 
		printf("success\n");
	else 
		perror("error");

//	nr = x ;
//	struct prinfo *buf[ nr ];
//	x = syscall( 223, *buf, nr);
	
	printf("%s\n",buf[0].comm);	

	int i ;

	while (i<nr) {
		if ( buf[i].parent_pid == 0 )
                        layer = 0 ;	
		int j ;
		for ( j = 0 ; j < layer ; j++) {
       			printf("\t");
		}
       			printf("%s,%d,%ld,%d,%d,%d,%ld\n", buf[i].comm, buf[i].pid, buf[i].state, buf[i].parent_pid, buf[i].first_child_pid,
				buf[i].next_sibling_pid , buf[i].uid);

		if ( buf[i].next_sibling_pid == 0) {
			if( buf[i].first_child_pid !=0 )
				layer ++ ;
			else 
				layer -- ;
		}
		else if ( buf[i].first_child_pid != 0 ) {
			layer ++ ;
			
		}
	i++;
	}	

	return 0;
}
