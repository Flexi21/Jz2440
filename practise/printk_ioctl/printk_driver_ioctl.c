
//static struct device_class *printk_dev;
//static struct class *printk_cls;

#include <linux/module.h>  
#include <linux/fs.h>  
#include <linux/cdev.h>  
#include <asm/uaccess.h>
#include <linux/ioctl.h>
//#include <linux/printk.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/delay.h>
#include <linux/bitrev.h>
#include <asm/uaccess.h>
#include <asm/io.h>

#include <pcmcia/cs_types.h>
#include <pcmcia/cs.h>
#include <pcmcia/cistpl.h>
#include <pcmcia/cisreg.h>
#include <pcmcia/ciscode.h>
#include <pcmcia/ds.h>

#define OPEN_IOCTL

#ifdef OPEN_IOCTL
#define PRINTK_ONE 0x01
#define PRINTK_TWO 0X02
#endif


static struct cdev printk_dev;
static struct class *printk_class;
static dev_t devno;
static struct class_device *printk_dev_cls;
static unsigned int major = 250;
static unsigned int mirror = 0;


static int printk_open(struct inode *node, struct file *file)
{
   printk("hello,you are succssful to open it!!\n");
   return 0;
}

static ssize_t printk_write(struct file *file, const char __user *buffer, 
                   size_t count, loff_t *ppos){
//unsigned long copy_from_user(void *to, const void __user *from, unsigned long n)
   int val;
   printk("hei,gay write here!!\n");
   copy_from_user(&val, buffer, count);
   printk("count:%d\n",count);
   if(val == 1)
      printk("the val is one!!");
   else
      printk("the val is not one!!");
    return 0;
}

 #ifdef OPEN_IOCTL
static int printk_ioctl(struct inode *inode, struct file *file, unsigned int cmd,
		    unsigned long arg)
{
   int ret = 0;
   printk("_IOC_DIR:%.4x _IOC_TYPE(cmd),%.4x _IOC_NR(cmd):%.4x _IOC_SIZE(cmd):%.4x\n",_IOC_DIR(cmd), _IOC_TYPE(cmd),_IOC_NR(cmd),_IOC_SIZE(cmd));

   switch(_IOC_NR(cmd)){
    
           case PRINTK_ONE:
                   printk("This is ioctl for one!!\n");
                   break;
           case PRINTK_TWO:
                   printk("This is ioctl for TWO!!\n");
                   break;
           default :
                   return -1;
   }



    return ret;
}
#endif

static struct file_operations printk_fops = {
     .owner        =  THIS_MODULE,
     .open         =  printk_open,      
     .write        =  printk_write,
   #ifdef OPEN_IOCTL
     .ioctl        =  printk_ioctl,
   #endif
};


static int printk_driver_init(void)
{ 
    int err ;
    printk("printk_driver_init entry\n");

   devno = MKDEV(major,mirror);
//register_chrdev_region(dev_t first,unsigned int count,char *name) 

    err = register_chrdev_region(devno,1,"printk_dev");//get the device num
    if(err<0)
       printk("register_chrdev_region failed!!\n");
    cdev_init(&printk_dev,&printk_fops);//connect between chr_dev and fops

    err= cdev_add(&printk_dev,devno,1);
    if(err < 0)
      {
         printk("cdev_add error!!\n");
         return -1;       
      }

    printk_class = class_create(THIS_MODULE,"printk_cls");
 
    //device_create(printk_class, NULL, devno, NULL, "printk_file");

    printk_dev_cls = class_device_create(printk_class, NULL, devno, NULL, "printk_file");


    return 0;
}

static void printk_driver_exit(void)
{
   cdev_del(&printk_dev);
   unregister_chrdev_region(devno,1);
  	class_device_unregister(printk_dev_cls);
	class_destroy(printk_class);
       printk("printk_driver_init exit\n");
}


module_init(printk_driver_init);
module_exit(printk_driver_exit);
MODULE_LICENSE("GPL");
