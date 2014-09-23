#include <linux/linkage.h>
#include <linux/kernel.h>
#include <linux/prinfo.h>
#include <linux/syscalls.h>
#include <linux/sched.h>
#include <linux/list.h>

asmlinkage int sys_ptree(struct prinfo *buf, int *nr)
{
	
	read_lock(&tasklist_lock); 
	
	struct task_struct *p = &init_task;

	/*printk("PARENT PROCESS: ");
	struct task_struct *initTask;
	initTask = list_entry(p->children.next, struct task_struct, sibling);
	printk("%s\n", initTask->comm);
	*/
	struct list_head *i;
	/*list_for_each(i, &p->children) {
		struct task_struct *currentTask;
		currentTask = list_entry(i, struct task_struct, sibling);
		printk("%s\n", currentTask->comm);
	}*/
	for_each_process(p) {
		struct task_struct *x;
		pid_t first_childPID = 0;
		pid_t next_siblingPID = 0;
		if(p->children.prev != &p->children) {
			x = list_entry(p->children.prev, struct task_struct, sibling);
			first_childPID = x->pid;
		}
		if(p->sibling.next != &p->sibling && p->sibling.next != &p->parent->children){
			x = list_entry(p->sibling.next, struct task_struct, sibling);
			next_siblingPID = x->pid;
		}
		
		x = list_entry(p->sibling.next, struct task_struct, sibling);
		printk("%s,%d,%ld,%d,%d,%d\n", p->comm, p->pid, p->state, p->parent->pid, first_childPID, next_siblingPID);
	}
	read_unlock(&tasklist_lock);
	printk("Congrats, your new system call has been called successfully");
	return 0;
}
