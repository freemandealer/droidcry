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


struct inode_operations *lower_iops;

/**
 * 拷贝/替换　inode　的操作表内容
 * 凡是没有直接拷贝的函数指针都要重新实现,并在新的操作表中赋值
 */
static struct dentry *droidcry_lookup(struct inode *dir, struct dentry *dentry, struct nameidata *nd) 
{
	printk("OOOOOOOO: My own lookup\n");
	return lower_iops->lookup(dir,dentry,nd);
}

struct inode_operations droidcry_iops = {
	.lookup = droidcry_lookup,
};

void droidcry_copy_inode_operations()
{
	droidcry_iops.create 	= lower_iops->create;
	//droidcry_iops.lookup	= lower_iops->lookup;
	droidcry_iops.link		= lower_iops->link;
	droidcry_iops.unlink	= lower_iops->unlink;
	droidcry_iops.symlink	= lower_iops->syslink;
	droidcry_iops.mkdir		= lower_iops->mkdir;
	droidcry_iops.rmdir		= lower_iops->rmdir;
	droidcry_iops.mknod		= lower_iops->mknod;
	droidcry_iops.rename	= lower_iops->rename;
	droidcry_iops.setattr	= lower_iops->setattr;
	droidcry_iops.get_acl	= lower_iops->get_acl;
	droidcry_iops.fiemap    = lower_iops->fiemap;
}

static int droidcry_init(void)
{
	const char * pathname = "/mnt/";
	struct path path;
		
	printk(KERN_ALERT "droidcry initializing ...\n");
	
	kern_path(pathname, LOOKUP_DIRECTORY, &path);
	printk("OOOOOOOO: we got this one -- %s\n",path.dentry->d_iname);
	lower_iops = path.dentry->d_inode->i_ops;
	path.dentry->d_inode->i_ops = &droidcry_iops;
	copy_inode_operations();	
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


