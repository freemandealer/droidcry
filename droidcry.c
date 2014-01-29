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
	printk(KERN_ALERT "OOOOOOOO: My own lookup\n");
	return lower_iops->lookup(dir,dentry,nd);
}

static int droidcry_mkdir(struct inode *dir, struct dentry *dentry, umode_t mode)
{
	printk(KERN_ALERT "OOOOOOOO: My own mkdir\n");
	return lower_iops->mkdir(dir, dentry, mode);
}

struct inode_operations droidcry_iops = {
	.lookup = droidcry_lookup,
	.mkdir = droidcry_mkdir,
};

void droidcry_copy_inode_operations(void)
{
	droidcry_iops.create 	= lower_iops->create;
	//droidcry_iops.lookup	= lower_iops->lookup;
	droidcry_iops.link		= lower_iops->link;
	droidcry_iops.unlink	= lower_iops->unlink;
	droidcry_iops.symlink	= lower_iops->symlink;
	//droidcry_iops.mkdir	= lower_iops->mkdir;
	droidcry_iops.rmdir		= lower_iops->rmdir;
	droidcry_iops.mknod		= lower_iops->mknod;
	droidcry_iops.rename	= lower_iops->rename;
	droidcry_iops.setattr	= lower_iops->setattr;
	droidcry_iops.get_acl	= lower_iops->get_acl;
	droidcry_iops.fiemap    = lower_iops->fiemap;
}

static int droidcry_init(void)
{
	const char * pathname = "/mnt/asec";
	struct path secret_path;
	struct list_head *p; // 用于迭代
	struct dentry *d;
		
	printk(KERN_ALERT "OOOOOOOO: droidcry initializing ...\n");
	
	kern_path(pathname, LOOKUP_FOLLOW, &secret_path);
	printk(KERN_ALERT "OOOOOOOO: secret directory -- %s\n",secret_path.dentry->d_iname);
	/* 为加密目录保存之前的iops,然后替换之 */
	lower_iops = secret_path.dentry->d_inode->i_op;
	droidcry_copy_inode_operations();
	secret_path.dentry->d_inode->i_op = &droidcry_iops;
	/* 替换子目录的指针,本应该递归替换的，这里简单替换直接子目录 TODO*/
	/*
	printk(KERN_ALERT "OOOOOOOO: current child dentry in memory:\n");
	p = &secret_path.dentry->d_subdirs;
	while ( p != NULL ) {
		printk(KERN_ALERT "OOOOOOOO\n");
		d = container_of(p, struct dentry , d_u.d_child);
		d->d_inode->i_op = &droidcry_iops;
		printk(KERN_ALERT "OOOOOOOO: %s\n", d->d_iname);
		p = p->next;
	}*/
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


