#include <linux/version.h>
#include <linux/module.h>   
#include <linux/kernel.h>    
#include <linux/init.h>      

#include <linux/proc_fs.h>  
#include <linux/seq_file.h>  
#include <linux/fs_struct.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("mpouzol");
MODULE_DESCRIPTION("Global Information Grid");

static int ft_show(struct seq_file *m, void *v) 
{
  struct dentry *curdentry;

  list_for_each_entry(curdentry, &current->fs->root.mnt->mnt_root->d_subdirs, d_child)
	{
		if (curdentry->d_flags & DCACHE_MOUNTED)
			seq_printf(m, "%s    /%s\n", curdentry->d_name.name, curdentry->d_name.name);

	}
  return 0;
}

static int __init ft_init(void) {
  proc_create_single("mymounts", 0, NULL, ft_show);
  return 0;
}

static void __exit ft_cleanup(void) {
  remove_proc_entry("mymounts", NULL);
}

module_init(ft_init);
module_exit(ft_cleanup);