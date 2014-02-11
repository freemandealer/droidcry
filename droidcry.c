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
struct address_space_operations *lower_aops;
struct file_operations *lower_fops;


/**
 * 拷贝/替换　address_space　的操作表内容
 * 凡是没有直接拷贝的函数指针都要重新实现,并在新的操作表中赋值
 */
static int droidcry_readpage(struct file *file, struct page *page)
{
	printk(KERN_ALERT "OOOOOOOO: my own readpage\n");
	return lower_aops->readpage(file, page);
}

static int droidcry_readpages(struct file *file, struct address_space *mapping, struct list_head *pages, unsigned nr_pages)
{
	printk(KERN_ALERT "OOOOOOOO: my own readpages\n");
	return lower_aops->readpages(file, mapping, pages, nr_pages);	
};

static sector_t droidcry_bmap(struct address_space *mapping, sector_t block)
{
	printk(KERN_ALERT "OOOOOOOO: my own bmap\n");
	return lower_aops->bmap(mapping, block);
}

static int droidcry_writepage(struct page *page, struct writeback_control *wbc)
{
	printk(KERN_ALERT "OOOOOOOO: my own writepage\n");
	return lower_aops->writepage(page, wbc);
}

static int droidcry_writepages(struct address_space *mapping, struct writeback_control *wbc)
{
	printk(KERN_ALERT "OOOOOOOO: my own writepages\n");
	return lower_aops->writepages(mapping, wbc);
}


static ssize_t droidcry_direct_IO(int rw, struct kiocb *iocb, const struct iovec *iov, loff_t offset, unsigned long nr_segs) 
{
	printk(KERN_ALERT "OOOOOOOO: my own direct_IO\n");
	return lower_aops->direct_IO(rw, iocb, iov, offset, nr_segs);
}


static int droidcry_write_begin(struct file *file, struct address_space *mapping,
			    loff_t pos, unsigned len, unsigned flags,
			    struct page **pagep, void **fsdata)
{
	printk(KERN_ALERT "OOOOOOOO: my own write_begin\n");
	return lower_aops->write_begin(file, mapping, pos, len, flags, pagep, fsdata);
}

static int droidcry_write_end(struct file *file,
				     struct address_space *mapping,
				     loff_t pos, unsigned len, unsigned copied,
				     struct page *page, void *fsdata)
{
	printk(KERN_ALERT "OOOOOOOO: my own write_end\n");
	return lower_aops->write_end(file, mapping, pos, len, copied, page, fsdata);
}

struct address_space_operations droidcry_aops = {
	.readpage = droidcry_readpage,
	.readpages = droidcry_readpages,
	.bmap = droidcry_bmap,
	.writepage = droidcry_writepage,
	.writepages = droidcry_writepages,
	.direct_IO = droidcry_direct_IO,
	.write_begin = droidcry_write_begin,
	.write_end = droidcry_write_end,
};


void droidcry_copy_address_space_operations(void)
{
	/* 使用前确定lower_aops准备就绪 */
	//droidcry_aops.writepage		= lower_aops->writepage;
	//droidcry_aops.readpage		= lower_aops->readpage;
	//droidcry_aops.writepages	= lower_aops->writepages;
	droidcry_aops.set_page_dirty= lower_aops->set_page_dirty;
	//droidcry_aops.readpages		= lower_aops->readpages;
	//droidcry_aops.write_begin	= lower_aops->write_begin;
	//droidcry_aops.write_end		= lower_aops->write_end;
	//droidcry_aops.bmap			= lower_aops->bmap;
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
 * 拷贝/替换　inode　的操作表内容
 * 凡是没有直接拷贝的函数指针都要重新实现,并在新的操作表中赋值
 */
static struct dentry *droidcry_lookup(struct inode *dir, struct dentry *dentry, struct nameidata *nd) 
{
	struct dentry *ret_dentry;
	printk(KERN_ALERT "OOOOOOOO: My own lookup\n");
	ret_dentry = lower_iops->lookup(dir,dentry,nd);
	/* 这个dentry对应的inode可能不存在内存或磁盘上 */
	if (!ret_dentry)
		goto out;
	lower_aops = ret_dentry->d_inode->i_mapping->a_ops;	
	droidcry_copy_address_space_operations();
	ret_dentry->d_inode->i_mapping->a_ops = &droidcry_aops;
out:
	return ret_dentry;
}

static int droidcry_create(struct inode *dir, struct dentry *dentry, umode_t mode, struct nameidata *nd)
{
	int rc;
	printk(KERN_ALERT "OOOOOOOO: My own create\n");
	rc = lower_iops->create(dir, dentry, mode, nd);
	lower_aops = dentry->d_inode->i_mapping->a_ops;
	if (lower_aops) {
		printk(KERN_ALERT "OOOOOOOO: lower_aops is not null\n");
		droidcry_copy_address_space_operations();
	} else {
		printk(KERN_ALERT "OOOOOOOO: lower_aops is null\n");
	}
	dentry->d_inode->i_mapping->a_ops = &droidcry_aops;
	return rc;	
}

struct inode_operations droidcry_iops = {
	.lookup = droidcry_lookup,
	.create = droidcry_create,
};

void droidcry_copy_inode_operations(void)
{
	/* 使用前确定lower_iops准备就绪 */
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

/**
 * 拷贝/替换　file　的操作表内容
 * 凡是没有直接拷贝的函数指针都要重新实现,并在新的操作表中赋值
 */
struct file_operations {
	struct module *owner;
	loff_t (*llseek) (struct file *, loff_t, int);
	ssize_t (*read) (struct file *, char __user *, size_t, loff_t *);
	ssize_t (*write) (struct file *, const char __user *, size_t, loff_t *);
	ssize_t (*aio_read) (struct kiocb *, const struct iovec *, unsigned long, loff_t);
	ssize_t (*aio_write) (struct kiocb *, const struct iovec *, unsigned long, loff_t);
	int (*readdir) (struct file *, void *, filldir_t);
	unsigned int (*poll) (struct file *, struct poll_table_struct *);
	long (*unlocked_ioctl) (struct file *, unsigned int, unsigned long);
	long (*compat_ioctl) (struct file *, unsigned int, unsigned long);
	int (*mmap) (struct file *, struct vm_area_struct *);
	int (*open) (struct inode *, struct file *);
	int (*flush) (struct file *, fl_owner_t id);
	int (*release) (struct inode *, struct file *);
	int (*fsync) (struct file *, loff_t, loff_t, int datasync);
	int (*aio_fsync) (struct kiocb *, int datasync);
	int (*fasync) (int, struct file *, int);
	int (*lock) (struct file *, int, struct file_lock *);
	ssize_t (*sendpage) (struct file *, struct page *, int, size_t, loff_t *, int);
	unsigned long (*get_unmapped_area)(struct file *, unsigned long, unsigned long, unsigned long, unsigned long);
	int (*check_flags)(int);
	int (*flock) (struct file *, int, struct file_lock *);
	ssize_t (*splice_write)(struct pipe_inode_info *, struct file *, loff_t *, size_t, unsigned int);
	ssize_t (*splice_read)(struct file *, loff_t *, struct pipe_inode_info *, size_t, unsigned int);
	int (*setlease)(struct file *, long, struct file_lock **);
	long (*fallocate)(struct file *file, int mode, loff_t offset,
			  loff_t len);
}; 

 
static int droidcry_init(void)
{
	const char * pathname = "/mnt/obb";
	struct path secret_path;
		
	printk(KERN_ALERT "OOOOOOOO: droidcry initializing ...\n");
	
	kern_path(pathname, LOOKUP_FOLLOW, &secret_path);
	/* 为加密目录保存之前的iops,然后替换之 */
	lower_iops = secret_path.dentry->d_inode->i_op;
	droidcry_copy_inode_operations();
	/* TODO: 替换之前应该先检测目录或其中文件是否正在被使用 */
	secret_path.dentry->d_inode->i_op = &droidcry_iops;
	/* TODO: 只是一个平坦模型,如果需要支持目录,还得修改mkdir操作 */
	
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


