#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <linux/ioctl.h>
//#include <linux/delay.h>

#define IOC_MAGIC  'b'
#if 0
#define PRINTK_ONE 0x01
#define PRINTK_TWO 0X02
#endif

//#define PRINTK_ONE    _IO(IOC_MAGIC, 1)
//#define PRINTK_TWO    _IO(IOC_MAGIC, 2)
//#define _IOC(dir,type,nr,size)
#define PRINTK_ONE _IOC(0,'c',1,0)
//#define PRINTK_TWO _IOC(0,'c',2,0)

//main()
int main(int argc, char** argv)  
{  
    int ret,fd,val = 1, test = 0x40;  
  
    fd = open("/dev/led_drv",O_RDWR);  
    if(fd<0)  
    {  
        perror("open fail \n");  
        return ;  
    }
   // write(fd, &val, 4); 
    //mdelay(1000);
   if (strcmp(argv[1], "1") == 0)
       val = 1;
   else
       val = 0;

     write(fd, &val, 4);

  // printf("test size:%d %d %d %d %d\n",sizeof(char),sizeof(unsigned char),sizeof(int),sizeof(unsigned int),sizeof(long));
   /*
    ret = ioctl(fd,PRINTK_ONE);
    if (ret) {
        printf("ioctl init:");
    }
*/
#if 0
    ret = ioctl(fd,PRINTK_TWO);
    if (ret) {
        printf("ioctl init:");
    }
#endif

  
    close(fd);

    return 0;
}  
