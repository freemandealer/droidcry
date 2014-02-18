#include <linux/backing-dev.h>
#include <linux/dcache.h>
#include <linux/file.h>
#include <linux/module.h>
#include <linux/namei.h>
#include <linux/skbuff.h>
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
#include <linux/sched.h>
#include <linux/kernel.h> 
#include <linux/init.h>
#include <linux/fs_struct.h>
#include <linux/fdtable.h>


const struct inode_operations *lower_iops;
const struct address_space_operations *lower_aops;
const struct file_operations *lower_fops; 


/**
 * Copy or replace *address_space* operations
 * All non-copy operations must be implemented and registered.
 */
static int droidcry_readpage(struct file *file, struct page *page)
{
	printk(KERN_ALERT "droidcry: my own readpage\n");
	return lower_aops->readpage(file, page);
}

static int droidcry_readpages(struct file *file, struct address_space *mapping, 
					struct list_head *pages, unsigned nr_pages)
{
	printk(KERN_ALERT "droidcry: my own readpages\n");
	return lower_aops->readpages(file, mapping, pages, nr_pages);	
};


static int droidcry_writepage(struct page *page, struct writeback_control *wbc)
{
	printk(KERN_ALERT "droidcry: my own writepage\n");
	return lower_aops->writepage(page, wbc);
}

static int droidcry_writepages(struct address_space *mapping, 
						struct writeback_control *wbc)
{
	printk(KERN_ALERT "droidcry: my own writepages\n");
	return lower_aops->writepages(mapping, wbc);
}


static ssize_t droidcry_direct_IO(int rw, struct kiocb *iocb,
					const struct iovec *iov, loff_t offset, unsigned long nr_segs) 
{
	printk(KERN_ALERT "droidcry: my own direct_IO\n");
	return lower_aops->direct_IO(rw, iocb, iov, offset, nr_segs);
}




struct address_space_operations droidcry_aops = {
	.readpage = droidcry_readpage,
	.readpages = droidcry_readpages,
	.writepage = droidcry_writepage,
	.writepages = droidcry_writepages,
	.direct_IO = droidcry_direct_IO,
};

/**
 * make sure lower_iops is correctly pointed before invoke
 */
void droidcry_copy_address_space_operations(void)
{
	//droidcry_aops.writepage		= lower_aops->writepage;
	//droidcry_aops.readpage		= lower_aops->readpage;
	//droidcry_aops.writepages	= lower_aops->writepages;
	droidcry_aops.set_page_dirty= lower_aops->set_page_dirty;
	//droidcry_aops.readpages		= lower_aops->readpages;
	droidcry_aops.write_begin	= lower_aops->write_begin;
	droidcry_aops.write_end		= lower_aops->write_end;
	droidcry_aops.bmap			= lower_aops->bmap;
	droidcry_aops.invalidatepage= lower_aops->invalidatepage;
	droidcry_aops.releasepage	= lower_aops->releasepage;
	droidcry_aops.freepage		= lower_aops->freepage;
	//droidcry_aops.direct_IO		= lower_aops->direct_IO;
	droidcry_aops.get_xip_mem	= lower_aops->get_xip_mem;
	droidcry_aops.migratepage	= lower_aops->migratepage;
	droidcry_aops.launder_page	= lower_aops->launder_page;
	droidcry_aops.is_partially_uptodate= lower_aops->is_partially_uptodate;
	droidcry_aops.error_remove_page	= lower_aops->error_remove_page;
}



/**
 * Copy or replace *file* operations
 * All non-copy operations must be implemented and registered.
 */
static int droidcry_file_open(struct inode * inode, struct file * filp)
{
	printk(KERN_ALERT "droidcry: my own file open\n");
	return lower_fops->open(inode ,filp);
} 
 
struct file_operations droidcry_fops = { 
	.open		= droidcry_file_open,
};

void droidcry_copy_file_operations(void)
{
	droidcry_fops.llseek	= lower_fops->llseek;
	droidcry_fops.read		= lower_fops->read;
	droidcry_fops.write		= lower_fops->write;
	droidcry_fops.aio_read	= lower_fops->aio_read;
	droidcry_fops.aio_write	= lower_fops->aio_write;
	droidcry_fops.unlocked_ioctl = lower_fops->unlocked_ioctl;
#ifdef CONFIG_COMPAT
	droidcry_fops.compat_ioctl	= lower_fops->compat_ioctl;
#endif
	droidcry_fops.mmap		= lower_fops->mmap;
	//droidcry_fops.open		= lower_fops->open;
	droidcry_fops.release	= lower_fops->release;
	droidcry_fops.fsync		= lower_fops->fsync;
	droidcry_fops.splice_read	= lower_fops->splice_read;
	droidcry_fops.splice_write	= lower_fops->splice_write;
	droidcry_fops.fallocate	= lower_fops->fallocate;
}


/**
 * Copy or replace *inode* operations
 * All non-copy operations must be implemented and registered.
 */
static struct dentry *droidcry_lookup(struct inode *dir, struct dentry *dentry, 
							unsigned int flags) 
{
	struct dentry *ret_dentry;
	
	printk(KERN_ALERT "droidcry: My own lookup\n");
	ret_dentry = lower_iops->lookup(dir,dentry,flags);
	if (!ret_dentry || !(ret_dentry->d_inode)) {
		printk("droidcry: invalid dentry in lookup\n");
		goto out;
	}
	printk("droidcry: Modifying %s 's operations\n", ret_dentry->d_iname);
	/* Save and replace fops */
	if (ret_dentry->d_inode->i_fop == &droidcry_fops) 
		return ret_dentry;
	lower_fops = ret_dentry->d_inode->i_fop;	
	droidcry_copy_file_operations();
	ret_dentry->d_inode->i_fop = &droidcry_fops;
	/* Save and replace aops */
	if (ret_dentry->d_inode->i_mapping->a_ops == &droidcry_aops)
		return ret_dentry;
	lower_aops = ret_dentry->d_inode->i_mapping->a_ops;
	droidcry_copy_address_space_operations();
	ret_dentry->d_inode->i_mapping->a_ops = &droidcry_aops;
out:
	return ret_dentry;
} 

static int droidcry_create(struct inode *dir, struct dentry *dentry, umode_t mode, 
						bool b)
{
	int rc;
	
	printk(KERN_ALERT "droidcry: My own create\n");
	printk("droidcry: Modifying %s 's operations\n", dentry->d_iname);
	rc = lower_iops->create(dir, dentry, mode, b);
	/* Save and replace lower fops */
	if (dentry->d_inode->i_fop == &droidcry_fops) 
		return rc;
	lower_fops = dentry->d_inode->i_fop; 
	if (lower_fops) {
		printk("droidcry: Lower_fops is not null(0x%x)\n", lower_fops);
		droidcry_copy_file_operations();
		dentry->d_inode->i_fop = &droidcry_fops;
	} else {
		printk("droidcry: Lower_fops is null\n");
	}
	/* Save and replace lower fops */
	if (dentry->d_inode->i_mapping->a_ops == &droidcry_aops)	
		return rc;
	lower_aops = dentry->d_inode->i_mapping->a_ops;
	if (lower_aops) {
		printk("droidcry: Lower_aops is not null(0x%x) <- Is the value strange?\n", 
			lower_aops);
		droidcry_copy_address_space_operations();
		dentry->d_inode->i_mapping->a_ops = &droidcry_aops;
	} else {
		printk("droidcry: Lower_aops is null\n");
	}
	return rc;	
}

struct inode_operations droidcry_iops = {
	.lookup = droidcry_lookup,
	.create = droidcry_create,
};

/**
 * make sure lower_iops is correctly pointed before invoke
 */
void droidcry_copy_inode_operations(void)
{
	//droidcry_iops.create 	= lower_iops->create;
	//droidcry_iops.lookup	= lower_iops->lookup;
	droidcry_iops.link		= lower_iops->link;
	droidcry_iops.unlink	= lower_iops->unlink;
	droidcry_iops.symlink	= lower_iops->symlink;
	droidcry_iops.mkdir		= lower_iops->mkdir;
	droidcry_iops.rmdir		= lower_iops->rmdir;
	droidcry_iops.mknod		= lower_iops->mknod;
	droidcry_iops.rename	= lower_iops->rename;
	droidcry_iops.setattr	= lower_iops->setattr;
	droidcry_iops.get_acl	= lower_iops->get_acl;
	droidcry_iops.fiemap    = lower_iops->fiemap;
}

static char *target;
module_param(target,charp,0644);
 
static int droidcry_init(void)
{
	const char * pathname = target;
	struct path secret_path;
		
	printk("Droidcry initializing ...\n");
	
	kern_path(pathname, LOOKUP_FOLLOW, &secret_path);
	/* Save iops of target directory and replace it */
	lower_iops = secret_path.dentry->d_inode->i_op;
	droidcry_copy_inode_operations();
	/* TODO: check if any file in the directory is in use */
	
	secret_path.dentry->d_inode->i_op = &droidcry_iops;
	/* TODO: modify the mkdir() to support subdirectory */
	
	return 0; 
}



static void droidcry_exit(void)
{
	printk(KERN_ALERT "Uninstalling droidcry ...\n");
	//TODO: destruction
}

module_init(droidcry_init);
module_exit(droidcry_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Freeman Zhang");
MODULE_DESCRIPTION("An simple transparent en/decrypt module for practice");
