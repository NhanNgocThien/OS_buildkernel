#include <stdio.h>
#include <stdlib.h>
#include "queue.h"

int empty(struct queue_t * q) {
	return (q->size == 0);
}

void enqueue(struct queue_t * q, struct pcb_t * proc) {
	/* TODO: put a new process to queue [q] */	
    if(q->size == MAX_QUEUE_SIZE) return;
    if(empty(q)) {
        q->proc[0] = proc;
    }
    else {
        uint32_t highest_prior = proc->priority;
        if(highest_prior > q->proc[q->size - 1]->priority) {
            q->proc[q->size] = proc;
        }
        else {
            q->proc[q->size] = q->proc[q->size - 1];
            int i = q->size - 1;
            while(i > 0) {
                if(highest_prior > q->proc[i - 1]->priority) {
                    q->proc[i] = proc;
                    q->size++;
                    return;
                }
                q->proc[i] = q->proc[i - 1];
                i--;
            }
            q->proc[0] = proc;
        }
    }
    q->size++;
    return;
}

struct pcb_t * dequeue(struct queue_t * q) {
	/* TODO: return a pcb whose prioprity is the highest
	 * in the queue [q] and remember to remove it from q
	 * */
    if(!empty(q)) {
        struct pcb_t *newPCB = q->proc[q->size - 1];
        q->proc[q->size - 1] = NULL;
        q->size--;
        return newPCB;
    }
    return NULL;
}

