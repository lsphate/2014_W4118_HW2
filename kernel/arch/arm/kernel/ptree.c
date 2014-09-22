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

	printk("PARENT PROCESS: ");
	struct task_struct *initTask;
	initTask = list_entry(p->children.next, struct task_struct, sibling);
	printk("%s\n", initTask->comm);

		read_unlock(&tasklist_lock);
	printk("Congrats, your new system call has been called successfully");
	return 0;
}
