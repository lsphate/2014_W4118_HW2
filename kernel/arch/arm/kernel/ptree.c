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
	int bufSize, procnum;

	if (copy_from_user(&bufSize, nr, sizeof(int)))
		return -EINVAL;
	printk("NR: %d\n\n", bufSize);

	prinfoBuf = kmalloc(sizeof(struct prinfo) * bufSize, GFP_KERNEL);
	if (!prinfoBuf)
		return -EFAULT;

	struct prinfo_list * prinlist;
	prinlist = kmalloc(sizeof(struct prinfo_list)* bufSize, GFP_KERNEL);
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
	//		printk("%s,%d,%ld,%d\n", y->comm, y->pid, y->state, y->parent->pid);
			
			struct task_struct *z;
                	pid_t first_childPID = 0;
               		pid_t next_siblingPID = 0;
                	if(y->children.prev != &y->children) {
                        	z = list_entry(y->children.prev, struct task_struct, sibling);
                        	first_childPID = z->pid;
                	}	       
                	if(y->sibling.next != &y->sibling && y->sibling.next != &y->parent->children) {
                        	z = list_entry(y->sibling.next, struct task_struct, sibling);
                        	next_siblingPID = z->pid;
                	}

	//		prinfoBuf[count].comm = y->comm;
			strncpy (prinfoBuf[count].comm, y->comm, 64);
			prinfoBuf[count].pid = y->pid;
			prinfoBuf[count].state = y->state;
			prinfoBuf[count].parent_pid = y->parent->pid;
			prinfoBuf[count].first_child_pid = first_childPID;
			prinfoBuf[count].next_sibling_pid = next_siblingPID;
			
			printk("#%d - %s,%d,%ld,%d,%d,%d\n", count, y->comm, y->pid, y->state, y->parent->pid, first_childPID, next_siblingPID);
			
			struct list_head *x;
			/* If it has children add them to top of stack*/
			list_for_each(x, &y->children) {
				prinlist[++count].original_task = x;
				INIT_LIST_HEAD(&(prinlist[count].dfs_order));
				list_add(&(prinlist[count].dfs_order), &(dfsStack.dfs_order));
			}
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
	
	read_unlock(&tasklist_lock);
	
	
	if( copy_to_user (buf, prinfoBuf, sizeof(struct prinfo)*bufSize))
		return -EINVAL;
	
	printk("Congrats, your new system call has been called successfully");
	return 0;
}
