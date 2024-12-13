#include<linux/module.h>
#include<linux/sched/task.h>
#include<linux/slab.h>
#include<linux/list.h>
int __init task_init(void)
{
    struct module *trav;//refer book
    list_for_each_entry(trav,&THIS_MODULE->list,list)
    {
          pr_info("%s:  list name =%s\n", THIS_MODULE->name, trav->name);
    }
    return 0;
}

static void __exit task_exit(void) {
    pr_info("%s: task_exit() called.\n", THIS_MODULE->name);
    
}

module_init(task_init);
module_exit(task_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("process list");
MODULE_AUTHOR("siddhu");