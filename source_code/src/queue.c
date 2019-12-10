#include <stdio.h>
#include <stdlib.h>
#include "queue.h"

int empty(struct queue_t * q) {
	return (q->size == 0);
}

void enqueue(struct queue_t * q, struct pcb_t * proc) {
	/* TODO: put a new process to queue [q] */	
  if(q->size < MAX_QUEUE_SIZE) {
		q->proc[q->size] = proc;   
		q->size = q->size + 1;
	}
	return;
}

struct pcb_t * dequeue(struct queue_t * q) {
	/* TODO: return a pcb whose prioprity is the highest
	 * in the queue [q] and remember to remove it from q
	 * */
	if(!empty(q)) {
		int i = 1,
			pos = 0;
		uint32_t highest_prior = q->proc[0]->priority;
		while(i < q->size) {
			if(highest_prior < q->proc[i]->priority) {
				pos = i;
				highest_prior = q->proc[i]->priority;
			}
			i++;
		}
		struct pcb_t *newPCB = q->proc[pos];
		
		for(int j = pos; j < q->size - 1; j++) {
			q->proc[j] = q->proc[j + 1];
		}
		q->proc[q->size - 1] = NULL;
		q->size--;
		return newPCB;
	}
	return NULL;
}

