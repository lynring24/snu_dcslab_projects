#include <linux/debugfs.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/uaccess.h>
#include <asm/pgtable.h>

#define PADDR_MASK 0x0000ffffffffffff

MODULE_LICENSE("GPL");

static struct dentry *dir, *output;
static struct task_struct *task;

struct packet 
{
	pid_t pid;
	unsigned long vaddr;
	unsigned long paddr;
};


static ssize_t read_output(struct file *fp,
		char __user *user_buffer,
		size_t length,
		loff_t *position)
{
	// Implement read file operation

	struct packet pckt;
	unsigned long vaddr;
	unsigned long paddr = 0;
	unsigned long page_addr = 0;
	unsigned long page_offset = 0; 

	copy_from_user(&pckt, user_buffer, sizeof(struct packet));

	task = pid_task(find_vpid(pckt.pid),  PIDTYPE_PID);
	vaddr = pckt.vaddr;


	pgd_t *pgd;
	p4d_t *p4d;
	pud_t *pud;
	pmd_t *pmd;
	pte_t *pte;
	struct page* page; 

	// if 5 level 
	pgd = pgd_offset(task->mm, vaddr);
	if (pgd_none(*pgd) && pgd_bad(*pgd))
		goto done;
	p4d = p4d_offset(pgd, vaddr);
	if (p4d_none(*p4d) && p4d_bad(*p4d))
		goto done;
	pud = pud_offset(p4d, vaddr);
	if (pud_none(*pud) && pud_bad(*pud))
		goto done;
	pmd = pmd_offset(pud, vaddr);

	pte = pte_offset_kernel(pmd, vaddr);
	/* Page frame physical address mechanism | offset */

	page_addr = pte_val(*pte) & PAGE_MASK;
	page_addr = page_addr & PADDR_MASK;


	pckt.paddr = page_addr;
	printk("paddr :%lx\n",pckt.paddr);
	copy_to_user(user_buffer, &pckt, length);
done:
	return length;

}

static const struct file_operations dbfs_fops = {
	// Mapping file operations with your functions
	.read = read_output,	
};

static int __init dbfs_module_init(void)
{
	// Implement init module

	dir = debugfs_create_dir("paddr", NULL);

	if (!dir) {
		printk("Cannot create paddr dir\n");
		return -1;
	}

	// Fill in the arguments below
	output = debugfs_create_file("output", 00777, dir, NULL, &dbfs_fops);

	printk("dbfs_paddr module initialize done\n");

	return 0;
}

static void __exit dbfs_module_exit(void)
{
	// Implement exit module
	debugfs_remove_recursive(dir);

	printk("dbfs_paddr module exit\n");
}

module_init(dbfs_module_init);
module_exit(dbfs_module_exit);
