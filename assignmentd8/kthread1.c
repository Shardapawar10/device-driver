#include<linux/module.h>
#include<linux/kthread.h>
#include<linux/delay.h>

static int print_numbers(void *data )
{
    int i;
    for(i=1;i<=10;i++)
    {
        pr_info("%s: number running = %d\n",THIS_MODULE->name,i);    
         mdelay(1000);
    }
    return 0;
}
static struct task_struct *task;
static int __init num_init(void)
{
    pr_info("%s: init called()\n",THIS_MODULE->name);
    task =kthread_run(print_numbers,NULL,"numthread"); //numthread:which name you want 
    pr_info("%s: new thread created for thread %d()\n",THIS_MODULE->name,task->pid); 
   
    return 0;
}
static void __exit num_exit(void)
{
    pr_info("%s: exit called()\n",THIS_MODULE->name);
}

module_init(num_init);
module_exit(num_exit);

MODULE_LICENSE("GPL");
