// user space application to test our device driver.
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
int main() {
    int fd, ret, newfd;
    char buf[32];

    fd = open("/dev/pchar1", O_WRONLY);
    if(fd < 0) {
        perror("open() failed");
        _exit(1);
    }
   
    ret = write(fd, buf, strlen(buf));
    if(ret < 0)
        perror("write() error");
    printf("write() returned: %d\n", ret);
    

    fd = open("/dev/pchar1", O_RDONLY);
    if(fd < 0) {
        perror("open() failed");
        _exit(1);
    }
    memset(buf, 0, sizeof(buf));
    ret = read(fd, buf, 26);
    printf("read() returned: %d --> %s\n", ret, buf);
    memset(buf, 0, sizeof(buf));
    ret = read(fd, buf, 10);
    printf("read() returned: %d --> %s\n", ret, buf);
    memset(buf, 0, sizeof(buf));
    ret = read(fd, buf, 4);
    printf("read() returned: %d --> %s\n", ret, buf);
    close(fd);

    /*
    fd = open("/dev/pchar", O_RDONLY); // new OFT entry (ref=1)
                                        // calls pchar_open()
    if(fd < 0) {
        perror("open() failed");
        _exit(1);
    }
    newfd = dup(fd);    // OFT entry (ref=2)
    close(newfd);       // OFT entry (ref=1)
    close(fd);          // OFT entry (ref=0)
                            // calls pchar_close()
                            // delete OFT entry
    */
    return 0;
}
