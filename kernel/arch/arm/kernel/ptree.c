#include <linux/linkage.h>
#include <linux/kernel.h>
#include <linux/prinfo.h>
#include <linux/syscalls.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/list.h>
#include <asm/uaccess.h>

SYSCALL_DEFINE2(ptree, struct prinfo *, buf, int *, nr)
{
	struct prinfo * prinfoBuf;
	int bufSize, procnum, index;

	if (copy_from_user(&bufSize, nr, sizeof(int)))
		return -EFAULT;
	printk("NR: %d\n\n", bufSize);

	prinfoBuf = kmalloc(sizeof(struct prinfo) * bufSize, GFP_KERNEL);

	if (copy_from_user(prinfoBuf, buf, sizeof(struct prinfo) * bufSize))
		return -EFAULT;
	
	read_lock(&tasklist_lock); 
	
	struct task_struct *p = &init_task;
	
	
	for_each_process(p) {
		procnum++;	
	} 

	struct prinfo_list prinlist[128];
	printk("Total processes: %d\n", procnum);
	p = &init_task;

	int count = 0;
	struct list_head *i;
	struct prinfo_list dfsStack;
	INIT_LIST_HEAD(&dfsStack.dfs_order);

	list_for_each(i, &p->children) {
		prinlist[count].original_task = i;
		INIT_LIST_HEAD(&(prinlist[count].dfs_order));
		list_add(&(prinlist[count].dfs_order), &(dfsStack.dfs_order));
		/* As long as there are children in the stack visit */
		struct list_head *top;

		top = &(dfsStack.dfs_order);
		while(top->next != &dfsStack.dfs_order) {
			struct prinfo_list *currentTask;
			currentTask = list_entry(top->next, struct prinfo_list, dfs_order);
			list_del(top->next);
			struct task_struct *y;
			y = list_entry(currentTask->original_task, struct task_struct, sibling);
			printk("%s,%d,%ld,%d\n", y->comm, y->pid, y->state, y->parent->pid);
		}
	/*	
		struct task_struct *x;
		struct prinfo_list *y;
		pid_t first_childPID = 0;
		pid_t next_siblingPID = 0;
		if(p->children.prev != &p->children) {
			x = list_entry(p->children.prev, struct task_struct, sibling);
			first_childPID = x->pid;
		}
		if(p->sibling.next != &p->sibling && p->sibling.next != &p->parent->children) {
			x = list_entry(p->sibling.next, struct task_struct, sibling);
			next_siblingPID = x->pid;
		}
		
		x = list_entry(p->sibling.next, struct task_struct, sibling);
		printk("#%d - %s,%d,%ld,%d,%d,%d\n", count, p->comm, p->pid, p->state, p->parent->pid, first_childPID, next_siblingPID);
	*/	
		count++;	
	}
/*
	list_for_each(i, prinlist->dfs_order.next) {
		struct prinfo_list *x;
		struct task_struct *y;
		x = list_entry(i, struct prinfo_list, dfs_order);
		y = list_entry(x->original_task, struct task_struct, sibling);
		printk("%s,%d,%ld,%d\n", y->comm, y->pid, y->state, y->parent->pid);
	}
*/		
	/////////////////////////////////////DFS HERE !!!!!!!!!! ////////////////////////////////////
/*	
		index = 1;
		int i;
		struct prinfo_list temp ;

		for (index ; index < procnum-1 ; index ++ ) {
			if (prinlist[index-1].prinfo.next_child_pid!=0)
				for (i = index ; i < procnum ; i++ ) {
					if (prinlist[i].prinfo.pid == prinlist[index-1].prinfo.next_child_pid )
						temp = prinlist[i];
						prinlist[i] = prinlist[index];
						prinlist[index] = temp;
				}	
			else if (prinlist[index-1].prinfo.next_sibling_pid!=0)
				for (i = index ; i < procnum ; i++ ) {
                                	if (prinlist[i].prinfo.pid == prinlist[index-1].prinfo.next_sibling_pid )
                                                temp = prinlist[i];
                                                prinlist[i] = prinlist[index];
                                                prinlist[index] = temp;
                                }
			else {
				temp = prinlist[index] ;
				for (i = index ; i < procnum ; i++) {	
					if (prinlist[index].prinfo.pid < temp)
						temp = prinlist[ index ];
				}
				prinlist[i] = prinlist[index];
				prinlist[index] = temp; 
			}
		}


*/

	read_unlock(&tasklist_lock);
	printk("Congrats, your new system call has been called successfully");
	return 0;
}
