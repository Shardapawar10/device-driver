#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/kfifo.h>
#include <linux/moduleparam.h>
#include <linux/slab.h>
#include<linux/moduleparam.h>
#include<linux/ioctl.h>
#include"ioctl.h"

#define MAX 32
typedef struct pchardev
{
    struct kfifo mybuf;
    struct cdev cdev;
    dev_t devno;

}pchardev_t;


static int pchar_open(struct inode *p_inode, struct file * p_file)
{
    pchardev_t *dev= container_of(p_inode->i_cdev,pchardev_t,cdev);//private buff ka address deta 
    p_file->private_data=dev;//then usko private data memeber me daldiya
    pr_info("%s pchar open called for pchar\n",THIS_MODULE->name);
    return 0;

}

static int pchar_close(struct inode *p_inode, struct file *p_file)
{
     pchardev_t *dev= container_of(p_inode->i_cdev,pchardev_t,cdev);//private buff ka address deta 
    p_file->private_data=dev;//then usko private data memeber me daldiya
    pr_info("%s pchar open called for pchar\n",THIS_MODULE->name);
    return 0;
}
static ssize_t pchar_write(struct file *p_file, const char *ubuff, size_t bufsize, loff_t *fpos)
{

    pchardev_t *dev=(pchardev_t*)p_file->private_data;//then usko private data memeber me daldiya
    int ret,nbytes;
     pr_info("%s: pchar_write() called for pchar.\n", THIS_MODULE->name);
       ret = kfifo_from_user(&dev->mybuf, ubuff, bufsize, &nbytes);
    if(ret < 0) {
        pr_err("%s: kfifo_from_user() failed for pchar.\n", THIS_MODULE->name);
        return ret;
    }
    pr_info("%s: pchar_write() written %d bytes in pchar.\n", THIS_MODULE->name, nbytes);
    return nbytes;
}
static ssize_t pchar_read(struct file *p_file, char *buff, size_t buffsize, loff_t *fpos)
{
    pchardev_t *dev = (pchardev_t *)p_file->private_data;
    int ret, nbytes;
    pr_info("%s: pchar_read() called .\n", THIS_MODULE->name);
    ret = kfifo_to_user(&dev->mybuf, buff, buffsize, &nbytes);
    if(ret < 0) {
        pr_err("%s: kfifo_to_user() failed for .\n", THIS_MODULE->name);
        return ret;
    }
    pr_info("%s: pchar_read() read %d bytes in .\n", THIS_MODULE->name, nbytes);
    return nbytes;
}

long pchar_ioctl(struct file *p_file, unsigned int cmd, unsigned long param)
{
    pchardev_t *dev = (pchardev_t *)p_file->private_data;
    devinfo_t info;
    int ret;
    pr_info("%s pchar_ioctl called() -- %u\n", THIS_MODULE->name, cmd);
    switch (cmd)
    {
    case FIFO_CLEAR:
        kfifo_reset(&dev->mybuf);
        pr_info("%s: pchar_ioctl() dev buffer is cleared.\n", THIS_MODULE->name);
        return 0;
    case FIFO_GETINFO:
        info.avail = kfifo_avail(&dev->mybuf);
        info.size = kfifo_size(&dev->mybuf);
        info.len = kfifo_len(&dev->mybuf);
        ret = copy_to_user((void *)param, &info, sizeof(info));
        if (ret < 0)
        {
            pr_info("%s copy to user failed\n", THIS_MODULE->name);
        }
      return 0;


    }
    printk(KERN_ERR "%s: invalid command in pchar_ioctl().\n", THIS_MODULE->name);
    return -EINVAL;
}


int DEVCNT=4;
module_param_named(devcnt,DEVCNT,int,0600);
static dev_t devno;
static int major;
static pchardev_t *devices;

static struct class *pclass1;


static struct file_operations  fops = {
    .owner=THIS_MODULE,
    .open=pchar_open,
    .release=pchar_close,
    .write=pchar_write,
    .read=pchar_read,
    .unlocked_ioctl=pchar_ioctl
};

static int __init sid_init(void)
{
    int ret,i;
    dev_t devnum;
    static struct device *pdevice;
    pr_info("%s init_called()\n",THIS_MODULE->name);
    //allocate each devices size using kmalloc
    devices=kmalloc(DEVCNT*sizeof(pchardev_t),GFP_KERNEL);
    if(IS_ERR(devices))
    {
        pr_err("%s kmalloc failed\n",THIS_MODULE->name);
        ret=-1;
        goto kmalloc_failed;
    }
//allocate device number
ret=alloc_chrdev_region(&devno,0,DEVCNT,"pchar");// ls /proc/devices/
//allocate devno number
if(ret!=0)
{
        pr_err("%s chrdev region failed\n",THIS_MODULE->name);
        goto chrdev_failed;
}
pr_info("%s no. allocated  succesfull\n",THIS_MODULE->name);

//allocate class
major=MAJOR(devno);
pclass1=class_create("pchar_class");
if(IS_ERR(pclass1))
{
    pr_err("%s class failed\n",THIS_MODULE->name);
    ret =-1;
    goto class_failed;
}

pr_info("%s class create succesfull\n",THIS_MODULE->name);

//device create
for(i=0;i<DEVCNT;i++)
{
devnum=MKDEV(major,i);
pdevice=device_create(pclass1,NULL,devnum,NULL,"pchar%d",i);
if(IS_ERR(pdevice))
{
    pr_err("%s device create pchar%d failed\n",THIS_MODULE->name,i);
    ret =-1;
    goto device_create_failed;
}
pr_info("%s device create succesfull pchar %d \n",THIS_MODULE->name,i);
}

//cdev intialize
//har ek device ko intialize kro 
for(i=0;i<DEVCNT;i++)
{
    devnum=MKDEV(major,i);//local isliye liya kyuki agr struct vale ko use krna hai to pehle intislaize kro
    devices[i].cdev.owner=THIS_MODULE;
    cdev_init(&devices[i].cdev,&fops);
    ret=cdev_add(&devices[i].cdev,devnum,1);
    if(ret!=0)
    {
        pr_err("%s cdev add failed for pchar %d\n",THIS_MODULE->name,i);
    
        goto cdev_failed;
    }
    pr_info("%s cdev intialize succesfull pchar %d \n",THIS_MODULE->name,i);

}
//alloc fifo for each device
//ab yaha intialize krdiya cdev ko to devices[i].cdev use kiya.

for(i=0;i<DEVCNT;i++)
{
    devices[i].devno=MKDEV(major,i);
    ret=kfifo_alloc(&devices[i].mybuf,MAX,GFP_KERNEL);
    if(ret!=0)
    {
        pr_err("%s kfifo alloc  failed for pchar %d\n",THIS_MODULE->name,i);

    goto fifo_alloc_failed;
    }

    pr_info("%s: kfifo alloc succesfull for pchar %d \n",THIS_MODULE->name,i);
}

return 0;

fifo_alloc_failed:
for(i=i-1;i>=0;i--)
{
    devnum=MKDEV(major,i);//local isliye liya kyuki agr struct vale ko use krna hai to pehle intislaize kro
    cdev_del(&devices[i].cdev);
}

cdev_failed:
for(i=i-1;i>=0;i--)
{
    devnum=MKDEV(major,i);
    device_destroy(pclass1,devnum);
}

device_create_failed:
class_destroy(pclass1);//jisme class ka return le rhe vo

class_failed:
unregister_chrdev_region(devno,DEVCNT);
chrdev_failed:
    kfree(devices);
kmalloc_failed:
    return ret;
}

static void __exit sid_exit(void)
{
    int i;
    pr_info("%s exit called\n",THIS_MODULE->name);
    //kfifo free
for(i=0;i<DEVCNT;i++)
{
   kfifo_free(&devices[i].mybuf);
}
   pr_info("%s kfifo released \n",THIS_MODULE->name);
//cdev free
   for(i=0;i<DEVCNT;i++)
{
    cdev_del(&devices[i].cdev);
}
//device destroy
   for(i=0;i<DEVCNT;i++)

{
    device_destroy(pclass1,devices[i].devno);
}
 pr_info("%s device  released \n",THIS_MODULE->name);
 //class destroy
 class_destroy(pclass1);
 //deallocate number
 unregister_chrdev_region(devno,DEVCNT);
 //malloc free
 kfree(devices);
}


module_init(sid_init);
module_exit(sid_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("SIDDHU");
MODULE_DESCRIPTION("multi device");





