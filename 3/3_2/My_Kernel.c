#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/printk.h>
#include <linux/proc_fs.h>
#include <asm/current.h>

#define procfs_name "Mythread_info"
#define BUFSIZE  1024
char buf[BUFSIZE]; //kernel buffer

static ssize_t Mywrite(struct file *fileptr, const char __user *ubuf, size_t buffer_len, loff_t *offset){
    /*Your code here*/

    if (buffer_len >= BUFSIZE) {    // for '\0'
        buffer_len = BUFSIZE - 1; 
    }

    if (copy_from_user(buf, ubuf, buffer_len)){     // write data from /proc buffer into kernel buffer
        return -EFAULT; 
    }
 
    buf[buffer_len] = '\0'; 

    *offset += buffer_len;  // update the offset

    pr_info("procfile write %s\n", buf);    // write info into kernel


    return buffer_len; 

    /****************/
}


static ssize_t Myread(struct file *fileptr, char __user *ubuf, size_t buffer_len, loff_t *offset){
    /*Your code here*/
    
    snprintf(buf + strlen(buf), BUFSIZE - strlen(buf), 
        "\nPID: %d, TID: %d, time: %llu\n", 
            current->tgid, current->pid, current->utime/100/1000);

    int len = sizeof(buf);

    ssize_t ret = len; 

    if (*offset >= len || copy_to_user(ubuf, buf, len)) {
        pr_info("copy_to_user failed\n"); 
        
        ret = 0; 
    } 
    else { 
        pr_info("procfile read %s\n", fileptr->f_path.dentry->d_name.name); 

        *offset += len; 
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
