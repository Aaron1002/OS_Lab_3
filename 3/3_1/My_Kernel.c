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
    struct task_struct *task = current; // access current thread info
    struct task_struct *thread;
    int len = 0;    // the location to place the next data 

    // read thread info into buffer
    for_each_thread(current, thread){
        if (current->pid == thread->pid){   // do not print the main thread
            continue;
        }
        // (obj buffer location, max buffer size)
        len += snprintf(buf + len, BUFSIZE, "PID: %d, TID: %d, Prio: %d, State: %d\n",
                        current->pid, thread->pid, thread->prio, thread->__state);
    }

    // copy data from kernel space to user space
    if (*offset >= len) {   
        return 0;
    }
    copy_to_user(ubuf, buf, len);   // (pass proc_buf to ubuf in user mode)

    *offset = strlen(buf); // update offset
    return strlen(buf);

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