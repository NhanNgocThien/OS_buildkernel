
#include "queue.h"
#include "sched.h"
#include <pthread.h>
#include <stdio.h>

static struct queue_t ready_queue;
static struct queue_t run_queue;
static pthread_mutex_t queue_lock;

int queue_empty(void) {
	return (empty(&ready_queue) && empty(&run_queue));
}

void init_scheduler(void) {
	ready_queue.size = 0;
	run_queue.size = 0;
	pthread_mutex_init(&queue_lock, NULL);
}

struct pcb_t * get_proc(void) {
	/*TODO: get a process from [ready_queue]. If ready queue
	 * is empty, push all processes in [run_queue] back to
	 * [ready_queue] and return the highest priority one.
	 * Remember to use lock to protect the queue.
	 * */
	struct pcb_t * proc = NULL;
	pthread_mutex_lock(&queue_lock);
    if(empty(&ready_queue) && !empty(&run_queue)){
		PRINT_SCHED(
			printf("\t-----------Ready Queue Empty------------\n");
			printf("\t\tRun queue - size %d ; PID/Priority:  ", run_queue.size);
			for(int j = 0; j < run_queue.size; j++) {
				if(j < run_queue.size - 1) printf("%d-%d -> ", run_queue.proc[j]->pid, run_queue.proc[j]->priority);
				else printf("%d-%d\n", run_queue.proc[j]->pid, run_queue.proc[j]->priority);
			}
		)
		
		int i = 0;
		while(i < run_queue.size) {
			ready_queue.proc[i] = run_queue.proc[i];
			run_queue.proc[i] = NULL;
			i++;
		}
		ready_queue.size = run_queue.size;
		run_queue.size = 0;

		PRINT_SCHED(
			printf("\t-----------Ready Queue Pushed------------\n");
			printf("\t\tReady queue - size %d ; PID/Priority:  ", ready_queue.size);
			for(int j = 0; j < ready_queue.size; j++) {
				if(j < ready_queue.size - 1) printf("%d-%d -> ", ready_queue.proc[j]->pid, ready_queue.proc[j]->priority);
				else printf("%d-%d\n", ready_queue.proc[j]->pid, ready_queue.proc[j]->priority);
			}
		)

		proc = dequeue(&ready_queue);		
    }
    else if(!empty(&ready_queue)){
		PRINT_SCHED(
			printf("\t-----------Ready Queue Status------------\n");
			printf("\t\tReady queue - size %d ; PID/Priority:  ", ready_queue.size);
			for(int j = 0; j < ready_queue.size; j++) {
				if(j < ready_queue.size - 1) printf("%d-%d -> ", ready_queue.proc[j]->pid, ready_queue.proc[j]->priority);
				else printf("%d-%d\n", ready_queue.proc[j]->pid, ready_queue.proc[j]->priority);
			}
		)
		
		proc = dequeue(&ready_queue);	
	}
	pthread_mutex_unlock(&queue_lock);
    return proc;

}

void put_proc(struct pcb_t * proc) {
	pthread_mutex_lock(&queue_lock);
	enqueue(&run_queue, proc);
	pthread_mutex_unlock(&queue_lock);
}

void add_proc(struct pcb_t * proc) {
	pthread_mutex_lock(&queue_lock);
	enqueue(&ready_queue, proc);
	pthread_mutex_unlock(&queue_lock);	
}


