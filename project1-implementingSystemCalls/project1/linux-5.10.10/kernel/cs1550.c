#include <linux/syscalls.h>
#include <linux/kernel.h>
#include <linux/uaccess.h>
#include <linux/spinlock.h>
#include <linux/stddef.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/cs1550.h>

//read-write lock to protect list
static DEFINE_RWLOCK(lock);
//head of the semaphore list
static LIST_HEAD(sem_list);
//sem id nums
static long next = 0;

/**
 * Creates a new semaphore. The long integer value is used to
 * initialize the semaphore's value.
 *
 * The initial `value` must be greater than or equal to zero.
 *
 * On success, returns the identifier of the created
 * semaphore, which can be used with up() and down().
 *
 * On failure, returns -EINVAL or -ENOMEM, depending on the
 * failure condition.
 */
SYSCALL_DEFINE1(cs1550_create, long, value)
{
	if (value < 0) {
		return -EINVAL;
	}

	

	struct cs1550_sem *sem = kmalloc(sizeof(struct cs1550_sem),GFP_KERNEL);

	if (sem == NULL) {
		return -ENOMEM;
	}
	
	sem->value = value;
	spin_lock_init(&sem->spinlock);
	
	write_lock(&lock);

	INIT_LIST_HEAD(&sem->node); // initialize previous and next pointers
	INIT_LIST_HEAD(&sem->waiting);
	list_add(&sem->node, &sem_list);


	sem->sem_id = next;
	next++;
	
	write_unlock(&lock);
	

	return sem->sem_id;
}

/**
 * Performs the down() operation on an existing semaphore
 * using the semaphore identifier obtained from a previous call
 * to cs1550_create().
 *
 * This decrements the value of the semaphore, and *may cause* the
 * calling process to sleep (if the semaphore's value goes below 0)
 * until up() is called on the semaphore by another process.
 *
 * Returns 0 when successful, or -EINVAL or -ENOMEM if an error
 * occurred.
 */
SYSCALL_DEFINE1(cs1550_down, long, sem_id)
{
	struct cs1550_sem *sem = NULL;
	
	
	read_lock(&lock);
	list_for_each_entry(sem, &sem_list, node) {
		if (sem->sem_id == sem_id) {
			spin_lock(&sem->spinlock);
			if (sem->value > 0) {
				//printk(KERN_WARNING "DOWN: pid %d\n saw no queue", current->pid);
				sem->value--;
				spin_unlock(&sem->spinlock);
				read_unlock(&lock);
				return 0;
			} else {
				struct cs1550_task *ntask = kmalloc(sizeof(struct cs1550_task),GFP_ATOMIC);

				if (ntask == NULL) {
					spin_unlock(&sem->spinlock);
					read_unlock(&lock);
					return -ENOMEM;
				}

				INIT_LIST_HEAD(&ntask->list);
				ntask->task = current;
				//printk(KERN_WARNING "DOWN: adding pid %d\n", current->pid);
				list_add_tail(&ntask->list, &sem->waiting);
				set_current_state(TASK_INTERRUPTIBLE);
				spin_unlock(&sem->spinlock);
				read_unlock(&lock);
				schedule();
				return 0;
			}
			spin_unlock(&sem->spinlock);
		}
	}
	read_unlock(&lock);
	return -EINVAL;
}

/**
 * Performs the up() operation on an existing semaphore
 * using the semaphore identifier obtained from a previous call
 * to cs1550_create().
 *
 * This increments the value of the semaphore, and *may cause* the
 * calling process to wake up a process waiting on the semaphore,
 * if such a process exists in the queue.
 *
 * Returns 0 when successful, or -EINVAL if the semaphore ID is
 * invalid.
 */
SYSCALL_DEFINE1(cs1550_up, long, sem_id)
{
	
	struct cs1550_sem *sem = NULL;

	read_lock(&lock);
	list_for_each_entry(sem, &sem_list, node) {
		if(sem->sem_id == sem_id) {
			spin_lock(&sem->spinlock);
			sem->value++;
			if(!list_empty(&sem->waiting)) {
				struct cs1550_task *remtask = list_first_entry(&sem->waiting, struct cs1550_task, list);
				//printk(KERN_WARNING "UP: waking pid %d\n", remtask->task->pid);
				sem->value--;
				wake_up_process(remtask->task);
				list_del(&remtask->list);
				kfree(remtask);
				spin_unlock(&sem->spinlock);
				read_unlock(&lock);
				return 0;
			} else {
				spin_unlock(&sem->spinlock);
				read_unlock(&lock);
				return 0;
			}
		}
	}
	read_unlock(&lock);
	return -EINVAL;
}

/**
 * Removes an already-created semaphore from the system-wide
 * semaphore list using the identifier obtained from a previous
 * call to cs1550_create().
 *
 * Returns 0 when successful or -EINVAL if the semaphore ID is
 * invalid or the semaphore's process queue is not empty.
 */
SYSCALL_DEFINE1(cs1550_close, long, sem_id)
{
	struct cs1550_sem *sem = NULL;
	write_lock(&lock);
	
	list_for_each_entry(sem, &sem_list, node) {
		if(sem->sem_id == sem_id) {
			spin_lock(&sem->spinlock);
			if(!list_empty(&sem->waiting)) {
				spin_unlock(&sem->spinlock);
				write_unlock(&lock);
				return -EINVAL;
			}
			spin_unlock(&sem->spinlock);
			list_del(&sem->node);
			kfree(sem);
			write_unlock(&lock);
			return 0;
		}
	}
	
	write_unlock(&lock);
	return -EINVAL;
}
