#ifndef QUEUE_H
#define QUEUE_H

#include "error/error.h"
#include "data_structures/tree/node.h"

typedef TreeNode* QueueUnit;

struct Queue {
	QueueUnit data;
	struct Queue* next;
};

Error enqueue(Queue** queue, QueueUnit data);
Error dequeue(Queue** queue, QueueUnit* data);

#endif
