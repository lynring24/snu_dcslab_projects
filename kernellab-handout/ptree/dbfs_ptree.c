#include <linux/debugfs.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/uaccess.h>

#define MAX 1000

MODULE_LICENSE("GPL");

static struct dentry *dir, *inputdir, *ptreedir;
static struct task_struct *curr;

char output[MAX];
ssize_t length_total = 0;


static ssize_t write_pid_to_input(struct file *fp, 
		const char __user *user_buffer, 
		size_t length, 
		loff_t *position)
{
	pid_t input_pid;
	char dummy[MAX];
	ssize_t length_dummy = 0; 

	// initialize 
	length_total = 0;
	
	copy_from_user(&dummy,user_buffer,sizeof(user_buffer));
	printk ("user id %s\n",dummy);
	sscanf(dummy, "%u", &input_pid);
	printk ("start pid : %d \n", input_pid);
	// Find task_struct using input_pid. Hint: pid_task
	curr = pid_task(find_get_pid(input_pid), PIDTYPE_PID); 
	// checkup message

	//invalid input
	if(!curr)
		return -EINVAL;

	// Tracing process tree from input_pid to init(1) process
	// Make Output Format string: process_command (process_id)
	while( curr->pid != 0 ) {

		memset(dummy, 0, MAX);
		length_dummy = snprintf(dummy, MAX, "%s (%d)\n", curr->comm, curr->pid);
		snprintf(dummy+length_dummy, MAX-length_dummy, output);

		if(length_dummy >= 0)
			length_total += length_dummy;
		strcpy(output, dummy);
		curr = curr->parent;
	}

	return length;
}


static ssize_t read_pid_of_output(struct file *fp, 
		char __user *user_buffer,
		size_t length,
		loff_t *position)
{
	ssize_t length_copied;

	length_copied = simple_read_from_buffer(user_buffer, length, position, output, length_total);
	return length_copied;
}


static const struct file_operations dbfs_fwrite = {
	.write = write_pid_to_input,
};

static const struct file_operations dbfs_fread = {
	.read = read_pid_of_output,
};

static int __init dbfs_module_init(void)
{
	// Implement init module code

	dir = debugfs_create_dir("ptree", NULL);

	if (!dir) {
		printk("Cannot create ptree dir\n");
		return -1;
	}

	// name, mode, parent, data, fops
	inputdir = debugfs_create_file("input", 00777, dir, NULL, &dbfs_fwrite);
	if (!inputdir) {
		pr_err("Cannot create inputdir file\n");
		return -1;
	}

	ptreedir = debugfs_create_file("ptree", 00777, dir, NULL, &dbfs_fread);
	if (!ptreedir) {
		pr_err("Cannot create ptreedir file\n");
		return -1;
	}

	printk("dbfs_ptree module initialize done\n");

	return 0;
}


static void __exit dbfs_module_exit(void)
{
	// Implement exit module code
	debugfs_remove_recursive(dir);	
	printk("dbfs_ptree module exit\n");
}


module_init(dbfs_module_init);
module_exit(dbfs_module_exit);
