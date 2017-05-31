#include <linux/module.h>
#include <linux/slab.h>

static int __init malloc_init(void){
    void *temp,*temp2;
    int i =0;

    temp = kmalloc(999, GFP_KERNEL);
    kfree(temp);

    temp = kmalloc(99999, GFP_KERNEL);
    temp2 = kmalloc(99999, GFP_KERNEL);
    kfree(temp);

    temp = kmalloc(999, GFP_KERNEL);
    kfree(temp);

    kfree(temp2);

    for(i = 0; i < 9999;i++){
        temp = kmalloc(i, GFP_KERNEL);
        kfree(temp);
    }

    return 0;
}

static void __exit malloc_exit(void){
    

}

module_init(malloc_init);
module_exit(malloc_exit);

MODULE_AUTHOR("Kevin Turkington, Alessandro Lim");
MODULE_LICENSE("GPL");