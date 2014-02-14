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
	.writepage = droidcry_writepage,
	.writepages = droidcry_writepages,
	.direct_IO = droidcry_direct_IO,
	.write_begin = droidcry_write_begin,
	.write_end = droidcry_write_end,
};


void droidcry_copy_address_space_operations(struct address_space_operations dest, struct address_space_operations *src)
{
	/* 使用前确定lower_aops准备就绪 */
	//dest.writepage		= src->writepage;
	//dest.readpage		= src->readpage;
	//dest.writepages	= src->writepages;
	dest.set_page_dirty= src->set_page_dirty;
	//dest.readpages		= src->readpages;
	//dest.write_begin	= src->write_begin;
	//dest.write_end		= src->write_end;
	dest.bmap			= src->bmap;
	dest.invalidatepage= src->invalidatepage;
	dest.releasepage	= src->releasepage;
	dest.freepage		= src->freepage;
	//dest.direct_IO		= src->direct_IO;
	dest.get_xip_mem	= src->get_xip_mem;
	dest.migratepage	= src->migratepage;
	dest.launder_page	= src->launder_page;
	dest.is_partially_uptodate= src->is_partially_uptodate;
	dest.error_remove_page	= src->error_remove_page;
}

/**
 * 拷贝/替换　file　的操作表内容
 * 凡是没有直接拷贝的函数指针都要重新实现,并在新的操作表中赋值
 */
static int droidcry_file_open(struct inode * inode, struct file * filp)
{
	printk(KERN_ALERT "OOOOOOO: my own file open\n");
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
	printk("Modifying %s 's operations\n", ret_dentry->d_iname);
	lower_fops = ret_dentry->d_inode->i_fop;	
	droidcry_copy_file_operations();
	ret_dentry->d_inode->i_fop = &droidcry_fops;
out:
	return ret_dentry;
}

static int droidcry_create(struct inode *dir, struct dentry *dentry, umode_t mode, struct nameidata *nd)
{
	int rc;
	printk(KERN_ALERT "OOOOOOOO: My own create\n");
	
	rc = lower_iops->create(dir, dentry, mode, nd);
	lower_fops = dentry->d_inode->i_fop; 
	printk(KERN_ALERT "lower_fops (0x%x) \n", lower_fops);
	if (lower_fops) {
		printk(KERN_ALERT "OOOOOOOO: lower_fops is not null(0x%x) <- Is the value strange?\n", lower_fops);
		droidcry_copy_file_operations();
	} else {
		printk(KERN_ALERT "OOOOOOOO: lower_fops is null\n");
	}
	printk("Creation complete. Modifying %s 's operations\n", dentry->d_iname);
	dentry->d_inode->i_fop = &droidcry_fops;
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


 
static int droidcry_init(void)
{
	const char * pathname = "/storage/sdcard";
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


