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
	if (!buf || !nr)
		return -EINVAL;

	struct prinfo *prinfoBuf;
	int bufSize = 0;
	int procnum = 0;

	if (copy_from_user(&bufSize, nr, sizeof(int)))
		return -EINVAL;

	prinfoBuf =
kmalloc_array(bufSize, sizeof(struct prinfo), GFP_KERNEL);
	if (!prinfoBuf)
		return -EFAULT;

	struct prinfo_list *prinlist;

	prinlist =
kmalloc_array(bufSize, sizeof(struct prinfo_list), GFP_KERNEL);
	if (!prinlist)
		return -EFAULT;

	if (copy_from_user(prinfoBuf, buf, sizeof(struct prinfo) * bufSize))
		return -EINVAL;

	read_lock(&tasklist_lock);
	struct task_struct *p = &init_task;

	for_each_process(p) {
		procnum++;
	}

	p = &init_task;

	int count;
	struct list_head *i;
	struct prinfo_list dfsStack;

	count = 0;
	INIT_LIST_HEAD(&dfsStack.dfs_order);
	prinlist[count].original_task = &(p->children);
	INIT_LIST_HEAD(&(prinlist[count].dfs_order));
	list_add(&(prinlist[count].dfs_order), &(dfsStack.dfs_order));
	struct task_struct *z;

	z = list_entry(prinlist[count].original_task,
struct task_struct, children);

	struct list_head *top;
	int numCopied;

	numCopied = 0;
	top = &(dfsStack.dfs_order);
	while (top->next != &dfsStack.dfs_order && numCopied < bufSize) {
		struct prinfo_list *currentTask;

		currentTask =
list_entry(top->next, struct prinfo_list, dfs_order);
		list_del(top->next);
		struct task_struct *y;

		if (count != 0)
			y =
list_entry(currentTask->original_task, struct task_struct, sibling);
		else
			y =
list_entry(currentTask->original_task, struct task_struct, children);

		struct task_struct *z;
		pid_t first_childPID = 0;
		pid_t next_siblingPID = 0;

		if (y->children.prev != &y->children) {
			z =
list_entry(y->children.prev, struct task_struct, sibling);
			first_childPID = z->pid;
		}
		if (y->sibling.next != &y->sibling
&& y->sibling.next != &y->parent->children) {
			z =
list_entry(y->sibling.next, struct task_struct, sibling);
			next_siblingPID = z->pid;
		}

		strncpy(prinfoBuf[numCopied].comm,
y->comm, sizeof(prinfoBuf[numCopied].comm));
		prinfoBuf[numCopied].pid = y->pid;
		prinfoBuf[numCopied].state = y->state;
		prinfoBuf[numCopied].parent_pid = y->parent->pid;
		prinfoBuf[numCopied].first_child_pid = first_childPID;
		prinfoBuf[numCopied].next_sibling_pid = next_siblingPID;
		numCopied++;
		struct list_head *x;

		/* If it has children add them to top of stack*/
		list_for_each_prev(x, &y->children) {
			prinlist[++count].original_task = x;
			INIT_LIST_HEAD(&(prinlist[count].dfs_order));
			list_add(&(prinlist[count].dfs_order),
&(dfsStack.dfs_order));
		}
	}
	read_unlock(&tasklist_lock);

	if (copy_to_user(buf, prinfoBuf, sizeof(struct prinfo) * bufSize))
		return -EINVAL;

	kfree(prinfoBuf);
	kfree(prinlist);
	return procnum;
}
