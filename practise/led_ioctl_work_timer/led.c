#include <linux/io.h>
#include <linux/module.h>  
#include <linux/fs.h>  
#include <linux/cdev.h>  
#include <asm/uaccess.h>
#include <linux/ioctl.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/bitrev.h>



#define LED_DRIVER_NAME    "led_drv"
#define LED_CLASS_NAME     "led_class"
#define GPFCON_BASE         0x56000050
#define GPFDAT_BASE         0x56000054
#define LED_WORKQUEUE_NAME  "led_wordqueue"
#define OPEN 1
#define CLOSE 0

//static void do_led_work(void);
dev_t led_devno;
atomic_t wq_run_times;
static struct cdev  led_cdev;
static struct class *led_class;
static struct class_device *led_class_dev;

/*workqueue and timer*/
static void do_led_work(void);
static struct timer_list led_timer;
static struct workqueue_struct *led_wordqueue;
static struct work_struct led_work;
static DECLARE_WORK(led_work, do_led_work);
/*workqueue and timer*/

static volatile unsigned int *GPFCON;
static volatile unsigned int *GPFDAT;
static int led_open(void);
static int led_close(void);

/*
static void printk_bin(int num)
{
    bool temp =0;

    int total_bits = sizeof(num) * 8, i = 0;

    //array = (bool *)malloc(sizeof(bool) * total_bits);

    //memset(array, 0, sizeof(unsigned int) * 4);

    for (i=0; i < total_bits; i++ ){
         temp |= (num>>i);
           printk("%1d",temp);
        if((i%4==1)&&(i!=0))
         printk(" ");
}
}
*/

static void do_led_work(void)
{
       static int count = 0;
        atomic_inc(&wq_run_times);
        printk("====work queue run times: %u; count :%d\n", atomic_read(&wq_run_times),count);
        if(count <= 20){
        mod_timer(&led_timer, jiffies+HZ);
        count++;
        }
       if(count%2)
           led_open();
       else
           led_close();
           
}

static void led_time_function(unsigned long arg)
{

   int ret = 0;

/*int fastcall queue_work(struct workqueue_struct *wq, struct work_struct *work)*/
   ret = queue_work(led_wordqueue, &led_work);
   if (ret < 0)
      {
         printk("queue_work failed!!!");
      }

     //mod_timer(&led_timer, jiffies+HZ); 
}


static void workqueue_test(void)
{
    atomic_set(&wq_run_times, 0);

    led_wordqueue = create_singlethread_workqueue(LED_WORKQUEUE_NAME);

/**
*void fastcall init_timer(struct timer_list *timer)
*
*struct timer_list {
*	struct list_head entry;
*	unsigned long expires;
*
*	void (*function)(unsigned long);
*	unsigned long data;
*
*	struct tvec_t_base_s *base;
*#ifdef CONFIG_TIMER_STATS
*	void *start_site;
*	char start_comm[16];
*	int start_pid;
*#endif
*};
**/
    init_timer(&led_timer);

    led_timer.function = led_time_function;

    add_timer(&led_timer);

    mod_timer(&led_timer, jiffies+HZ); 


}

static void workqueue_test_exit(void)
{

  del_timer(&led_timer);
  destroy_workqueue(led_wordqueue);

}


static int led_open(void)
{
    //unsigned int led_dat;
    printk("led open entry!\n");
    
    *GPFDAT &= ~((1<<4) | (1<<5) | (1<<6));
    printk("OPEN date 0x%4x\n",*GPFDAT);
   // writel(led_dat, GPFCON);

    return 0;
}

static int led_close(void)
{
  //  unsigned int led_dat, value;
        printk("led close entry!\n");
    #if 0
    led_dat &= (0 << 4)&&(0 << 5)||(0 << 6);

    writel(led_dat, GPFCON);
#endif

    *GPFDAT |= ((1<<4) | (1<<5) | (1<<6));

     printk("close date %0x4x\n",*GPFDAT);
 
    return 0;
}

static ssize_t led_driver_open(struct file *file, char __user *buf, size_t count, loff_t *loff)
{
/**
*#define writel(val, addr) \
*  (void)((*(volatile unsigned int *) (addr)) = (val))
**/
#if 0
   unsigned int led_config;
  
    led_config = (1 << (4*2))|(1 << (5*2)) |(1 << (6*2));

    writel(led_config,GPFCON);
#endif
     //clear it first and write the date in
   *GPFCON &= ~((0x03<<(4*2))|(0x03<<(5*2))|(0x03<<(6*2)));

   printk("clear 567 %4x\n",*GPFCON);
   //config the i/o mode == output
   *GPFCON |= ((0x1<<(4*2)) | (0x1<<(5*2)) | (0x1<<(6*2)));
   
    printk("led open entry GPFCON:0x%4x!\n",*GPFCON);

    //printk_bin(*GPFCON);
 
    return 0;
}

#if 1
static ssize_t led_driver_write(struct file *file, const char __user *buf, size_t count, loff_t *loff)
{
  int val=0;
   
     copy_from_user(&val, buf, count);

   if (val==1)
    led_open(); 
   else if (val==0)
     led_close();

    return 0;
}
#endif

static int led_driver_ioctl(struct inode *node, struct file *file, unsigned int cmd, unsigned long arg)
{
    int ret;
    switch(_IOC_NR(cmd)){

          case OPEN:
                    led_open();
                    break;

          case CLOSE:
                    led_close();
                    break;
  
   
         default:
              printk("err cm:%d \n",cmd);
              ret = -1;
              break;
} 


    return 0;
}

static struct file_operations led_fops = {

    .open      =     led_driver_open,
#if 1
    .write     =     led_driver_write,
#endif
    .ioctl     =     led_driver_ioctl,
};


static int led_driver_init(void)
{
    int ret;
/**
*int alloc_chrdev_region(dev_t *dev, unsigned baseminor, unsigned count,
*			const char *name)
**/
    ret = alloc_chrdev_region(&led_devno, 0, 1, LED_DRIVER_NAME);
   
    if (ret < 0)
      {
        printk("region led_drv failed!");
      }     

/**
*void cdev_init(struct cdev *cdev, const struct file_operations *fops)
**/

    cdev_init(&led_cdev, &led_fops);

/**
*int cdev_add(struct cdev *p, dev_t dev, unsigned count)
**/   

    cdev_add(&led_cdev, led_devno, 1);  

/**
*struct class *class_create(struct module *owner, const char *name)
**/  

    led_class = class_create(THIS_MODULE, LED_CLASS_NAME);

/**
*extern struct class_device *class_device_create(struct class *cls,
*						struct class_device *parent,
*						dev_t devt,
*						struct device *device,
*						const char *fmt, ...)
*					__attribute__((format(printf,5,6)));
**/
    led_class_dev = class_device_create(led_class, NULL, led_devno, NULL, LED_DRIVER_NAME, NULL);

/**
*   ioremap (unsigned long phys_addr, unsigned long size)
**/
    GPFCON = ioremap(GPFCON_BASE , 16);
    if (GPFCON == NULL)
        return -1;

    GPFDAT = ioremap(GPFDAT_BASE , 16);
    if (GPFDAT == NULL)
        return -1;

    workqueue_test(); 
    
    return ret;
}


static void led_driver_exit(void)
{
/**
*void unregister_chrdev_region(dev_t from, unsigned count)
**/

   unregister_chrdev_region(led_devno, 1);

   class_destroy(led_class);

   class_device_unregister(led_class_dev);

   iounmap(GPFCON);
 
   iounmap(GPFDAT);

   workqueue_test_exit();
}


module_init(led_driver_init);
module_exit(led_driver_exit);
MODULE_LICENSE("GPL");
