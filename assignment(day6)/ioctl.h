#ifndef __IOCTL_H
#define __IOCTL_H

#include<linux/ioctl.h>

typedef struct devinfo
{
    short size;
    short len;
    short avail;
}devinfo_t;

typedef struct info{
    char out[10];
}info_t;

#define FIFO_CLEAR          _IO('x',1)
#define FIFO_GETINFO        _IOR('x',2,devinfo_t)
#define FIFO_START_TIMER    _IO('x',3)
#define FIFO_STOP_TIMER    _IO('x',4)

#endif