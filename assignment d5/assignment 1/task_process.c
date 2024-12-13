#include<linux/module.h>
#include<linux/sched/task.h>
#include<linux/slab.h>
#include<linux/list.h>
int __init task_init(void)
{
    struct task_struct *trav;
    list_for_each_entry(trav,&current->tasks,tasks)
    {
          pr_info("%s: name = %s pid=%d\n", THIS_MODULE->name,  trav->comm,trav->pid);
    }
    return 0;
}

static void __exit task_exit(void) {
    pr_info("%s: desd_exit() called.\n", THIS_MODULE->name);
    
    pr_info("%s: current process pid=%d, cmd=%s\n", THIS_MODULE->name, current->pid, current->comm);
    
}

module_init(task_init);
module_exit(task_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("process list");
MODULE_AUTHOR("siddhu");
