#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "queue.h"

int empty(struct queue_t * q) {
	return (q->size == 0);
}

void enqueue(struct queue_t * q, struct pcb_t * proc) {
	/* TODO: put a new process to queue [q] */	
    if(q->size == MAX_QUEUE_SIZE || proc == NULL) return;
    if(empty(q)) {
        q->proc[0] = proc;
    }
    else {
        int i = 0,
            pos = 0;
        uint32_t highest_prior = proc->priority;
        if(highest_prior > q->proc[0]->priority) {
            memmove(q->proc + 1, q->proc, sizeof(proc)*q->size);
            q->proc[0] = proc;
            q->size++;
            return;
        }
        while(i < q->size - 1) {
            if(highest_prior > q->proc[i + 1]->priority) {
                pos = i + 1;
                memmove(q->proc + pos + 1, q->proc + pos, sizeof(proc)*(q->size - pos));
                q->proc[pos] = proc;
                q->size++;
                return;
            }
            i++;
        }
        q->proc[q->size] = proc;
    }
    q->size++;
    return;
}

struct pcb_t * dequeue(struct queue_t * q) {
	/* TODO: return a pcb whose prioprity is the highest
	 * in the queue [q] and remember to remove it from q
	 * */
    if(empty(q)) return NULL;
    struct pcb_t *newPCB = NULL;
    newPCB = q->proc[q->size - 1];
    q->proc[q->size - 1] = NULL;
    q->size--;
    return newPCB;
}

