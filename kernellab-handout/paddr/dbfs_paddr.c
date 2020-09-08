#include <linux/debugfs.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/uaccess.h>
#include <asm/pgtable.h>


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
	//unsigned long page_addr = 0;
	//unsigned long page_offset = 0; 

	copy_from_user(&pckt, user_buffer, sizeof(struct packet));

	task = pid_task(find_vpid(pckt.pid),  PIDTYPE_PID);
        vaddr = pckt.vaddr;

	
	pgd_t *pgd;
	p4d_t *p4d;
	pud_t *pud;
	pmd_t *pmd;
	pte_t *pte;
	struct page *page; 

	pgd = pgd_offset(task->mm, vaddr);
	p4d = p4d_offset(pgd, vaddr);
	pud = pud_offset(p4d, vaddr);
	pmd = pmd_offset(pud, vaddr);
	pte = pte_offset_kernel(pmd, vaddr);
	printk("pte : %p\n", *pte);
	//pte = pte_offset_kernel(pmd, vaddr); //wrapper
	
	page = pte_page(*pte);
	paddr = page_to_phys(page);
	printk("paddr : %p\n",paddr);

	//	page_addr = pte_val(pte) & 0xFFFFFFFFF000;
	// 	page_offset = (vaddr & ~(0xFFFFFFFFF000));
	//	paddr = page_addr + page_offset;


	pckt.paddr = paddr;
	copy_to_user(user_buffer, &pckt,  sizeof(struct packet));
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
