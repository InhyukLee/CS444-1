#include <linux/module.h>
#include <linux/slab.h>
#include <linux/random.h>

/**
 * Testing module for slob module.
 */

static int __init malloc_init(void){
    void *temp,*temp2;
    int i = 0,rando,rando2;

    for(i = 0; i < 999999;i++){

        get_random_bytes(&rando, sizeof(rando));
        get_random_bytes(&rando2, sizeof(rando2));

        temp = kmalloc(rando2, GFP_KERNEL);
        kfree(temp);

         temp2 = kmalloc(rando, GFP_KERNEL);
        kfree(temp2);
    }

    return 0;
}

static void __exit malloc_exit(void){
    

}

module_init(malloc_init);
module_exit(malloc_exit);

MODULE_AUTHOR("Kevin Turkington, Alessandro Lim");
MODULE_LICENSE("GPL");