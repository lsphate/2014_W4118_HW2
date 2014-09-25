#ifndef _LINUX_PRINFO_H
#define _LINUX_PRINFO_H

#include <linux/types.h>

struct prinfo {
	pid_t parent_pid;		/* process id of parent */
	pid_t pid;			/* process id */
	pid_t first_child_pid;  	/* pid of youngest child */
	pid_t next_sibling_pid;  	/* pid of older sibling */
	long state;			/* current state of process */
	long uid;			/* user id of process owner */
	char comm[64];			/* name of program executed */
};

#endif

#ifndef _LINUX_PRINFO_LIST_H
#define _LINUX_PRINFO_LIST_H

#include <linux/types.h>
#include <linux/list.h>

struct prinfo_list {
        struct list_head dfs_order;           /* process id of parent */
//        struct list_head original_task;                  /* process id */
	struct prinfo prinfo;
};

#endif

