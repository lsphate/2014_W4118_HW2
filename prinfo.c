#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/syscall.h>
#include "prinfo.h"

/* Implement of stac k*/
typedef struct Stack {
	int capacity;
	int size;
	struct prinfo *elements;
}Stack;

Stack * createStack(int maxElements) {
	/* Create a Stack */
	Stack *S;
	S = (Stack *)malloc(sizeof(Stack));

	/* Initialise its properties */
	S->elements = (struct prinfo *)malloc(sizeof(int)*maxElements);
	S->size = 0;
	S->capacity = maxElements;

	return S;
}

void pop(Stack *S) {
	if(S->size==0) {
		printf("Stack is Empty\n");
		return;
	}
	else {
		S->size--;
	}
	return;
}

struct prinfo top(Stack *S) {
	if(S->size==0) {
		printf("Stack is Empty\n");
		exit(0);
	}
	return S->elements[S->size-1];
}

void push(Stack *S, struct prinfo element) {
	if(S->size == S->capacity) {
		printf("Stack is Full\n");
	}
	else {
		S->elements[S->size++] = element;
	}
	return;
}
/* Implement of stack */

int main(int argc, char **argv) {
	
	int x;
	int nr =10 ;
	int layer ;
	
	struct prinfo buff[ nr ];

	x = syscall(223, buff, nr);

	if (x == 0) 
		printf("success\n");
	else 
		perror("error");

	nr = x ;
	struct prinfo *buf[ nr ];
	x = syscall( 223, *buf, nr);
	


/*





DFS here





*/


	int i ;

	while (buf[i]!=NULL) {
		if ( buf[i] -> parent_pid == 0 )
                        layer = 0 ;	
		int j ;
		for ( j = 0 ; j < layer ; j++) {
       			printf("\t");
		}
       			printf("%s,%d,%ld,%d,%d,%d,%ld\n", buf[i]->comm, buf[i]->pid, buf[i]->state, buf[i]->parent_pid, buf[i]->first_child_pid,
				buf[i]->next_sibling_pid , buf[i]->uid);

		if ( buf[i] -> next_sibling_pid == 0) {
			if( buf[i]->first_child_pid !=0 )
				layer ++ ;
			else 
				layer -- ;
		}
		else if ( buf[i]->first_child_pid != 0 ) {
			layer ++ ;
			
		}
	i++;
	}	

	return 0;
}
