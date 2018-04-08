#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <linux/ioctl.h>

#define IOC_MAGIC  'b'
/*
#define PRINTK_ONE 0x01
#define PRINTK_TWO 0X02
*/

//#define PRINTK_ONE    _IO(IOC_MAGIC, 1)
//#define PRINTK_TWO    _IO(IOC_MAGIC, 2)
//#define _IOC(dir,type,nr,size)
#define PRINTK_ONE _IOC(0,'c',1,0)
#define PRINTK_TWO _IOC(0,'c',2,0)

main()  
{  
    int ret,fd,val = 1;  
  
    fd = open("/dev/printk_file",O_RDWR);  
    if(fd<0)  
    {  
        perror("open fail \n");  
        return ;  
    }
   if(0) 
    write(fd, &val, 4); 
   
    ret = ioctl(fd,PRINTK_ONE);
    if (ret) {
        printf("ioctl init:");
    }
    ret = ioctl(fd,PRINTK_TWO);
    if (ret) {
        printf("ioctl init:");
    }

  
    close(fd);
}  
