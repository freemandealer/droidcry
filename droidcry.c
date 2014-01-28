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
#include <linux/fdtable.h>

/*
struct super_block *sb;
struct super_operations *lower_ops;
*/
/**
 * 拷贝/替换　super_block　的操作表内容
 * 凡是没有直接拷贝的函数指针都要重新实现,并在新的操作表中赋值
 */
/*
struct inode * droidcry_alloc_inode(struct super_block *sb)
{
	printk(KERN_ALERT "OOOOOOOO: alloc inode \n");
	return lower_ops->alloc_inode(sb); 
}

struct super_operations droidcry_sops = {
	.alloc_inode = droidcry_alloc_inode,
};

static void copy_super_operations()
{
	//droidcry_sops.alloc_inode = lower_ops->alloc_inode;
	droidcry_sops.destroy_inode = lower_ops->destroy_inode;
	droidcry_sops.write_inode = lower_ops->write_inode;
	droidcry_sops.dirty_inode = lower_ops->dirty_inode;
	droidcry_sops.drop_inode = lower_ops->drop_inode;
	droidcry_sops.evict_inode = lower_ops->evict_inode;
	droidcry_sops.write_super = lower_ops->write_super;
	droidcry_sops.put_super = lower_ops->put_super;
	droidcry_sops.statfs = lower_ops->statfs;
	droidcry_sops.remount_fs = lower_ops->remount_fs;
	droidcry_sops.show_options = lower_ops->show_options;
	droidcry_sops.bdev_try_to_free_page = lower_ops->bdev_try_to_free_page;
}
*/

static int droidcry_init(void)
{
	const char * pathname = "/mnt/";
	struct path path;
	struct nameidata nd;
	
	printk(KERN_ALERT "droidcry initializing ...\n");
	
	kern_path(pathname, LOOKUP_DIRECTORY, &path);
	printk("OOOOOOOO: we got this one -- %s\n",path.dentry->d_iname);
	
	
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


