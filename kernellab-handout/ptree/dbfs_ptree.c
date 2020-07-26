#include <linux/debugfs.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/uaccess.h>

MODULE_LICENSE("GPL");

static struct dentry *dir, *inputdir, *ptreedir;
static struct task_struct *curr;

static struct char * traversed;

// TODO : Read input PID and trace down from the root until access to the passed argument
// debugfs_create_blob

static ssize_t write_pid_to_input(struct file *fp, 
                                const char __user *user_buffer, 
                                size_t length, 
                                loff_t *position)
{
        pid_t input_pid;

	// read input PID
        sscanf(user_buffer, "%u", &input_pid);
        // Find task_struct using input_pid. Hint: pid_task
        curr = pid_task(find_vpid(input_pid), PIDTYPE_PID);
	printk(KERN_INFO, "input pid : %d\n", input_pid);
        //curr = find_task_by_vpid(input_pid);

        // Tracing process tree from input_pid to init(1) process
	// list_for_each_entry(pos, head, member) ;
	struct list_head * tasks = &(curr->list_head);
	struct list_head * stack;
	INIT_LIST_HEAD(stack);

	for(curr, curr->pid!=1; curr=curr->parent) {
		// add current task_struct 
		list_add(curr, stack);

	}

	list_for_each(curr, stack,  ) {
		//append data
	 	curr->comm ;
		curr->pid; 
		
	}
        // Make Output Format string: process_command (process_id)

        return length;
}

static const struct file_operations dbfs_fops = {
        .write = write_pid_to_input,
};


//executed when model is inserted

static int __init dbfs_module_init(void)
{
        // Implement init module code

//#if 0
        dir = debugfs_create_dir("ptree", NULL);
        
        if (!dir) {
                printk("Cannot create ptree dir\n");
                return -1;
        }

	// file to read input 
        // inputdir = debugfs_create_file("input", , , , );
        inputdir = debugfs_create_file("input", 0644, dir, NULL, &dbfs_fops);

	// file to write output 
        // Find suitable debugfs API
        ptreedir = debugfs_create_file("ptree", 0644, dir, traversed, &dbfs_fops); // Find suitable debugfs API
//#endif	

	printk("dbfs_ptree module initialize done\n");

        return 0;
}


// executed when module is deleted 
static void __exit dbfs_module_exit(void)
{
        // Implement exit module code
	
	debugfs_remove_recursive(dir);

	printk("dbfs_ptree module exit\n");
}

module_init(dbfs_module_init);
module_exit(dbfs_module_exit);
