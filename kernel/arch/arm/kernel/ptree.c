#include <linux/linkage.h>
#include <linux/kernel.h>
#include <linux/prinfo.h>
#include <linux/syscalls.h>

asmlinkage int sys_ptree(struct prinfo *buf, int *nr)
{
	printk("Congrats, your new system call has been called successfully");
	return 0;
}
