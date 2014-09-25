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
	struct list_head *i;
	
	
	for_each_process(p) {
		procnum++;	
	} 

	struct prinfo_list prinlist[procnum];
	p = init_task;

	for_each_process(p) {
		struct task_struct *x;
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
		printk("%s,%d,%ld,%d,%d,%d\n", p->comm, p->pid, p->state, p->parent->pid, first_childPID, next_siblingPID);
		
		
		prinlist[index].prinfo.parent_pid = p->parent->pid;
		prinlist[index].prinfo.pid = p->pid;
		prinlist[index].prinfo.first_child_pid = first_childPID;
		prinlist[index].prinfo.next_sibling_pid = next_siblingPID;
		prinlist[index].prinfo.state = p->state;
		prinlist[index].prinfo.uid = p->uid;
		prinlist[index].prinfo.comm = p->comm;
		
		
	//	prinlist[index].original_task = p->sibling;	
		index++;
	}
		
	/////////////////////////////////////DFS HERE !!!!!!!!!! ////////////////////////////////////
	
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




	read_unlock(&tasklist_lock);
	printk("Congrats, your new system call has been called successfully");
	return 0;
}
