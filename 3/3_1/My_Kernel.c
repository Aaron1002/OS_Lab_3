#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/printk.h>
#include <linux/proc_fs.h>
#include <asm/current.h>

#define procfs_name "Mythread_info"
#define BUFSIZE  1024
char buf[BUFSIZE];

static ssize_t Mywrite(struct file *fileptr, const char __user *ubuf, size_t buffer_len, loff_t *offset){
    /* Do nothing */
	return 0;
}


static ssize_t Myread(struct file *fileptr, char __user *ubuf, size_t buffer_len, loff_t *offset){
    // (info that kernel passed, kernel write info to buffer, max read size user requested, offset)
    /*Your code here*/

    struct task_struct *thread; // PCB

    int len = sizeof(buf); 

    for_each_thread(current, thread){
        if (current->pid == thread->pid){   // ignore the main thread
            continue;
        }
        snprintf(buf + strlen(buf), BUFSIZE - strlen(buf), "PID: %d, TID: %d, Prio: %d, State: %d\n",
                    current->pid, thread->pid, thread->prio, thread->__state);
        /* 
        snprintf() will override the previous data, so add the new data 
        at (buf + strlen(buf)) (the location of the last data end)
        
        snprintf(add data at which address, MAX size of the data)
        */
    }

    ssize_t ret = len; 

    if (*offset >= len || copy_to_user(ubuf, buf, len)) {   // error dealing
        // copy_to_user(): write the data in kernel buffer into user_buffer
        
        pr_info("copy_to_user failed\n");   // print info in kernel (can be looked at kernel log)

        ret = 0; 
    } 
    else { 
        pr_info("procfile read %s\n", fileptr->f_path.dentry->d_name.name); 

        *offset += len; // update the offset
    } 

    return ret; 
    
    /****************/
}

static struct proc_ops Myops = {
    .proc_read = Myread,
    .proc_write = Mywrite,
};

static int My_Kernel_Init(void){
    proc_create(procfs_name, 0644, NULL, &Myops);   
    pr_info("My kernel says Hi");
    return 0;
}

static void My_Kernel_Exit(void){
    pr_info("My kernel says GOODBYE");
}

module_init(My_Kernel_Init);
module_exit(My_Kernel_Exit);

MODULE_LICENSE("GPL");