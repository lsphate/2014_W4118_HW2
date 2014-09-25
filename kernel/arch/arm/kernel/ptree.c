#include <linux/linkage.h>
#include <linux/kernel.h>
#include <linux/prinfo.h>
#include <linux/syscalls.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/list.h>
#include <linux/errno.h>
#include <asm/uaccess.h>

SYSCALL_DEFINE2(ptree, struct prinfo *, buf, int *, nr)
{
	struct prinfo * prinfoBuf;
	int bufSize, procnum, index;

	if (copy_from_user(&bufSize, nr, sizeof(int)))
		return -EINVAL;
	printk("NR: %d\n\n", bufSize);

	prinfoBuf = kmalloc(sizeof(struct prinfo) * bufSize, GFP_KERNEL);
	if (!prinfoBuf)
		return -EFAULT;

	struct prinfo_list prinlist[ nr ];
	prinlist = kmalloc(sizeof(struct prinfo_list) * nr, GFP_KERNEL);
	if (!prinlist)
		return -EFAULT;

	if (copy_from_user(prinfoBuf, buf, sizeof(struct prinfo) * bufSize))
		return -EINVAL;
	
	read_lock(&tasklist_lock); 
	
	struct task_struct *p = &init_task;
	
	
	for_each_process(p) {
		procnum++;	
	} 

	/* struct prinfo_list prinlist[128]; */
	printk("Total processes: %d\n", procnum);
	p = &init_task;

	int count = 0;
	struct list_head *i;
	struct prinfo_list dfsStack;
	INIT_LIST_HEAD(&dfsStack.dfs_order);

	prinlist[count].original_task = &(p->children);
	INIT_LIST_HEAD(&(prinlist[count].dfs_order));
	list_add(&(prinlist[count].dfs_order), &(dfsStack.dfs_order));	
	struct task_struct *z;
	z = list_entry(prinlist[count].original_task, struct task_struct, children);

	struct list_head *top;
	int numCopied;
	numCopied = 0;

	top = &(dfsStack.dfs_order);
	while(top->next != &dfsStack.dfs_order && numCopied < bufSize) {
		struct prinfo_list *currentTask;
		currentTask = list_entry(top->next, struct prinfo_list, dfs_order);
		list_del(top->next);
		struct task_struct *y;
		if(count != 0) 
			y = list_entry(currentTask->original_task, struct task_struct, sibling);
		else
			y = list_entry(currentTask->original_task, struct task_struct, children);
		printk("%s,%d,%ld,%d\n", y->comm, y->pid, y->state, y->parent->pid);
		numCopied++;
		struct list_head *x;
		/* If it has children add them to top of stack*/
		list_for_each(x, &y->children) {
			prinlist[++count].original_task = x;
			INIT_LIST_HEAD(&(prinlist[count].dfs_order));
			list_add(&(prinlist[count].dfs_order), &(dfsStack.dfs_order));
		}
	}
/*
	list_for_each(i, &p->children) {
		prinlist[count].original_task = i;
		INIT_LIST_HEAD(&(prinlist[count].dfs_order));
		list_add(&(prinlist[count].dfs_order), &(dfsStack.dfs_order));
		struct list_head *top;

		top = &(dfsStack.dfs_order);
		while(top->next != &dfsStack.dfs_order) {
			struct prinfo_list *currentTask;
			currentTask = list_entry(top->next, struct prinfo_list, dfs_order);
			list_del(top->next);
			struct task_struct *y;
			y = list_entry(currentTask->original_task, struct task_struct, sibling);
			printk("%s,%d,%ld,%d\n", y->comm, y->pid, y->state, y->parent->pid);
			struct list_head *x;
			list_for_each(x, &y->children) {
				prinlist[++count].original_task = x;
				INIT_LIST_HEAD(&(prinlist[count].dfs_order));
				list_add(&(prinlist[count].dfs_order), &(dfsStack.dfs_order));
			}
		}
		count++;	
	}
*/
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
	read_unlock(&tasklist_lock);
	printk("Congrats, your new system call has been called successfully");
	return 0;
}
