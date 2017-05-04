/*
 * elevator noop
 */
#include <linux/blkdev.h>
#include <linux/elevator.h>
#include <linux/bio.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/init.h>

/*
 * References:
 * [0] http://www.bricktou.cn/block/idx_noop-iosched_func_en.html
 */

struct noop_data {
	struct list_head queue;
};

static void noop_merged_requests(struct request_queue *q, struct request *rq,
				 struct request *next)
{
	//deletes entry from list and reinitialize it[0].
	list_del_init(&next->queuelist);
}

static int noop_dispatch(struct request_queue *q, int force)
{
	//*nd = elevator_data
	struct noop_data *nd = q->elevator->elevator_data;

	//Try to put the fields that are referenced together in the same cacheline. then
	//get the first element from a list
	if (!list_empty(&nd->queue)) {
		struct request *rq;
		rq = list_entry(nd->queue.next, struct request, queuelist);
		list_del_init(&rq->queuelist);
		elv_dispatch_sort(q, rq);
		return 1;
	}
	return 0;
}

static void noop_add_request(struct request_queue *q, struct request *rq)
{
	//*nd = elevator_data
	struct noop_data *nd = q->elevator->elevator_data;

	//add a new entry
	list_add_tail(&rq->queuelist, &nd->queue);
}

static struct request *
noop_former_request(struct request_queue *q, struct request *rq)
{
	//*nd = elevator_data
	struct noop_data *nd = q->elevator->elevator_data;

	//If prev == &queue then Returning NULL
	if (rq->queuelist.prev == &nd->queue)
		return NULL;

	//Returning get the prev element in list
	return list_entry(rq->queuelist.prev, struct request, queuelist);
}

static struct request *
noop_latter_request(struct request_queue *q, struct request *rq)
{
	//*nd = elevator_data
	struct noop_data *nd = q->elevator->elevator_data;

	//If next == &queue then Returning NULL
	if (rq->queuelist.next == &nd->queue)
		return NULL;
	
	//Returning get the next element in list
	return list_entry(rq->queuelist.next, struct request, queuelist);
}

static int noop_init_queue(struct request_queue *q, struct elevator_type *e)
{
	struct noop_data *nd;
	struct elevator_queue *eq;

	//alloc the elevator itself
	eq = elevator_alloc(q, e);
	//If !eq then Returning -Out of memory
	if (!eq)
		return -ENOMEM;

	//create a node?
	nd = kmalloc_node(sizeof(*nd), GFP_KERNEL, q->node);
	//if node creation failed
	if (!nd) {
		kobject_put(&eq->kobj);
		//return out of memory
		return -ENOMEM;
	}
	eq->elevator_data = nd;

	//initialize head
	INIT_LIST_HEAD(&nd->queue);

	//lock queue
	spin_lock_irq(q->queue_lock);
	//set elevator
	q->elevator = eq;
	//unlock queue
	spin_unlock_irq(q->queue_lock);
	return 0;
}

static void noop_exit_queue(struct elevator_queue *e)
{
	//*nd = elevator_data
	struct noop_data *nd = e->elevator_data;

	//detect data structure corruption.
	BUG_ON(!list_empty(&nd->queue));

	//free elevator_data
	kfree(nd);
}

static struct elevator_type elevator_noop = {
	.ops = {
		.elevator_merge_req_fn		= noop_merged_requests,
		.elevator_dispatch_fn		= noop_dispatch,
		.elevator_add_req_fn		= noop_add_request,
		.elevator_former_req_fn		= noop_former_request,
		.elevator_latter_req_fn		= noop_latter_request,
		.elevator_init_fn		= noop_init_queue,
		.elevator_exit_fn		= noop_exit_queue,
	},
	.elevator_name = "noop",
	.elevator_owner = THIS_MODULE,
};

static int __init noop_init(void)
{
	//Returning elv_register( &identifies an elevator type, such as AS or deadline)
	return elv_register(&elevator_noop);
}

static void __exit noop_exit(void)
{
	//elv_unregister( &identifies an elevator type, such as AS or deadline)
	elv_unregister(&elevator_noop);
}

module_init(noop_init);
module_exit(noop_exit);


MODULE_AUTHOR("Jens Axboe");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("No-op IO scheduler");
