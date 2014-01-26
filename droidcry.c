/**
 * 
 *
 *
 */
 
#include <linux/backing-dev.h>
#include <linux/dcache.h>
#include <linux/file.h>
#include <linux/module.h>
#include <linux/namei.h>
#include <linux/skbuff.h>
#include <linux/crypto.h>
#include <linux/mount.h>
#include <linux/pagemap.h>
#include <linux/key.h>
#include <linux/parser.h>
#include <linux/fs_stack.h>
#include <linux/slab.h>
#include <linux/magic.h>
#include <keys/user-type.h>
#include <keys/encrypted-type.h>
#include <linux/fs.h>
#include <linux/scatterlist.h>
#include <linux/hash.h>
#include <linux/nsproxy.h>
#include <linux/sched.h>
#include <linux/kernel.h> 
#include <linux/init.h>
#include <linux/fs_struct.h>


static int droidcry_init(void)
{
	struct task_struct *pcurrent;
	struct super_block *sb;
	struct inode *inode;
	
	printk(KERN_ALERT "droidcry initializing ...\n");
	pcurrent = get_current();
	sb = pcurrent->fs->pwd.dentry->d_sb;
	printk(KERN_ALERT "OOOOOOOO: current filesystem type: %s\n", sb->s_type->name);
	printk(KERN_ALERT "OOOOOOOO: get address space operations table\n");
	inode = container_of(sb->s_inodes.next, struct inode, i_sb_list);
	

	
	return 0; 
}

static void droidcry_exit(void)
{
	printk(KERN_ALERT "Uninstalling droidcry ...\n");
}

module_init(droidcry_init);
module_exit(droidcry_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Freeman Zhang");
MODULE_DESCRIPTION("An simple transparent en/decrypt module for android");


