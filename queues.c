#include "queues.h"


//Queue Allocators
//three types of queues used, priority process queues, FIFO process queues, and FIFO message envelopes
void ppq_allocate(priority_process_queue** ppq) { //NULL heads and tails
  (*ppq) = (priority_process_queue*) malloc(sizeof(priority_process_queue));
  assert((*ppq) != NULL);
  (*ppq)->pq_head[0] = NULL; (*ppq)->pq_tail[0] = NULL; 
  (*ppq)->pq_head[1] = NULL; (*ppq)->pq_tail[1] = NULL;
  (*ppq)->pq_head[2] = NULL; (*ppq)->pq_tail[2] = NULL;
  (*ppq)->pq_head[3] = NULL; (*ppq)->pq_tail[3] = NULL;
}

void mq_allocate(message_queue** mq) {
  (*mq) = (message_queue*) malloc(sizeof(message_queue));
  assert((*mq) != NULL);
  (*mq)->head   = NULL;
  (*mq)->tail   = NULL;
}

void pq_allocate(process_queue** pq) {
  (*pq) = (process_queue*) malloc(sizeof(process_queue));
  assert((*pq) != NULL);
  (*pq)->head = NULL;
  (*pq)->tail = NULL;
}

/*
void proc_allocate() {
  _process_list.head = NULL;
  _process_list.tail= NULL;
}*/

//Queue is_empty (if queue head is NULL, no elements, therefore empty)
int mq_is_empty(message_queue* mq) {
  assert(mq != NULL);
  return (mq->head == NULL);
}


int ppq_is_empty(priority_process_queue* ppq) {
  assert(ppq != NULL);
  return (ppq->pq_head[0] == NULL && 
  	  ppq->pq_head[1] == NULL &&
	  ppq->pq_head[2] == NULL &&
	  ppq->pq_head[3] == NULL);
}

int pq_is_empty(process_queue* pq) {
  assert(pq != NULL);
  return (pq->head == NULL);
}

/*
int proc_is_empty() {
  return (_process_list.head == NULL);
}*/

int ppq_is_empty_p(int p, priority_process_queue *ppq) {
  assert(ppq != NULL);
  return (ppq->pq_head[p] == NULL);
}


//Queue Enqueues
//add to tail, readjust tail pointer
//
void mq_enqueue(MessageEnvelope* env, message_queue* mq) {
  assert(env != NULL && mq != NULL);
  env->next = NULL;
  if (mq_is_empty(mq)) {
    mq->head = env;
    mq->tail = mq->head;
  } else {
    (mq->tail)->next = env;
    mq->tail = env;
  }
}

void pq_enqueue(PCB* next, process_queue* pq) {
  assert(pq != NULL && next != NULL);
  next->p_next = NULL;
  if (pq_is_empty(pq)) {
    pq->head = next;
    pq->tail = pq->head;
  } else {
    (pq->tail)->p_next = next;
    pq->tail = next;
  }
}

/*
void proc_enqueue(PCB* next) {
  assert(next != NULL);
  next->p_next = NULL;
  if (proc_is_empty()) {
    _process_list.head = next;
    _process_list.tail = _process_list.head;
  } else {
    _process_list.tail->p_next = next;
    _process_list.tail = next;
  }
}
*/

void ppq_enqueue(PCB* q_next, priority_process_queue* ppq) {
  assert(q_next != NULL && ppq != NULL);
  q_next->q_next = NULL;
  int priority = q_next->priority;
  if (ppq_is_empty_p(priority, ppq)) {
    ppq->pq_head[priority] = q_next;
    ppq->pq_tail[priority] = ppq->pq_head[priority];
  } else {
    ppq->pq_tail[priority]->q_next = q_next;
    ppq->pq_tail[priority] = q_next;
  }
}

//Queue dequeues
//remove head, reset head/tail pointer as necessary
//return NULL on empty list
PCB* ppq_dequeue(priority_process_queue* ppq) {
  assert(ppq != NULL);
  PCB* ret;
  priority_process_queue _ppq = (*ppq);
  int i = 0;
  if (ppq_is_empty(ppq)) return NULL;
  for (; i < MIN_PRIORITY; i++) {
    if (!ppq_is_empty_p(i, ppq)) {
      ret = _ppq.pq_head[i];
      _ppq.pq_head[i] = _ppq.pq_head[i]->q_next;
      ret->q_next = NULL;
      if (_ppq.pq_head[i] == NULL)
        _ppq.pq_tail[i] = NULL;
      return ret;
    }
  }
  return NULL;
}

PCB* pq_dequeue(process_queue* pq) {
  if (pq_is_empty(pq)) return NULL;
  PCB* ret = NULL;
  ret = pq->head;
  ret->p_next = NULL;
  pq->head = (pq->head)->p_next;
  if (pq->head == NULL)
    pq->tail = NULL;
  return ret;
}


/*
PCB* proc_dequeue() {
  if (proc_is_empty()) { return NULL; }
  PCB* ret  = NULL;
  ret = _process_list.head;
  ret->p_next = NULL;
  _process_list.head = _process_list.head->p_next;
  if (_process_list.head == NULL) 
    _process_list.tail = NULL;
  return ret;
}*/
  
MessageEnvelope* mq_dequeue(message_queue* mq) {
  assert(mq != NULL);
  if (mq_is_empty(mq)) return NULL;
  MessageEnvelope* deq = mq->head;
  mq->head = (mq->head)->next;
  if (mq->head == NULL)
    mq->tail = NULL;
  deq->next = NULL;
  return deq;
}

//Queue peeks
//return head
PCB* ppq_peek(priority_process_queue* ppq) {
  assert(ppq != NULL);
  priority_process_queue _ppq = (*ppq);
  int i = 0;
  for (; i < MIN_PRIORITY; i++) 
    if (!ppq_is_empty_p(i, ppq)) 
      return _ppq.pq_head[i];

  return NULL;
}

PCB* pq_peek(process_queue* pq) {
  assert(pq != NULL);
  return pq->head;
}
/*
PCB* proc_peek() {
  return _process_list.head;
}*/

MessageEnvelope* mq_peek(message_queue* mq) {
  assert(mq != NULL);
  return mq->head;
}

//Queue removes
//return target or NULL of target not contained
//if contained, remove target from queue
//iterate through queue until target is encountered, juggle references
PCB* ppq_remove(PCB* target, priority_process_queue* ppq) {
  assert(ppq != NULL);
  priority_process_queue _ppq = (*ppq);
  int priority = target->priority;
  PCB* t = _ppq.pq_head[priority];
  if (target == _ppq.pq_head[priority]) {
    _ppq.pq_head[priority] = _ppq.pq_head[priority]->q_next;
    return target;
  }
  while (t->q_next != NULL) {
    if (t->q_next == target) {
      if (_ppq.pq_tail[priority] == t->q_next) { 
        _ppq.pq_tail[priority] = t; 
	return target;
      }
      t->q_next = (t->q_next)->q_next;
      return target;
    } else {
      t = t->q_next;
    }
  }

  return NULL;
}

PCB* pq_remove(PCB* target, process_queue* pq) {
  assert(target != NULL && pq != NULL);
  PCB* next = pq->head;
  if (target == pq->head) {
    pq->head = (pq->head)->p_next;
    return target;
  }
  while (next->p_next != target && next->p_next != NULL) {
    next = next->p_next;
  }
  if (next->p_next == target) {
    if (next->p_next == pq->tail) {
      pq->tail = next;
      return target;
    }
    next->p_next = (next->p_next)->p_next;
    return target;
  } else { return NULL; }
}
/*
PCB* proc_remove(PCB* target) {
  PCB* next = _process_list.head;
  if (target == _process_list.head) {
    _process_list.head = _process_list.head->p_next; 
    return target;
  }
  while (next->p_next != target && next->p_next != NULL) {
    next = next->p_next;
  }
  if (next->p_next == target) {
    if (next->p_next == _process_list.tail) {
      _process_list.tail = next;
      return target;
    }
    next->p_next = (next->p_next)->p_next;
    return target;
  } else { return NULL; }
}*/

MessageEnvelope* mq_remove(MessageEnvelope* target, message_queue* mq) {
  assert(target != NULL && mq != NULL);
  MessageEnvelope* next = mq->head;
  if (mq_is_empty(mq)) return NULL;
  if (target == mq->head) {
    mq->head = (mq->head)->next;
    return target;
  }
  while (next->next != target && next->next != NULL) {
    next = next->next;
  }
  if (next->next == target) {
    if (next->next == mq->tail) {
      mq->tail = next;
      return target;
    }
    next->next = (next->next)->next;
    return target;
  } else {
    return NULL;
  }
}



//Queue deallocators
//called at cleanup
void pq_free(process_queue* pq) {
  assert(pq != NULL);
  PCB* next = pq->head;
  PCB* store;
  while (next != NULL) {
    store = next->p_next;
    free(next);
    next = store;
  }
  pq->head = NULL;
  pq->tail = NULL;
  free(pq);
}

/*void proc_free() { //safely deallocate PCBs on global process list
 // int i = 0;
  PCB* next = _process_list.head;
  PCB* store;
  next = _process_list.head;
  while (next != NULL) { //iterate through list, free resources held
    store = next->p_next;
    //todo: free message queues, envelopes, stack ptr, context
    free(next);
    next = store;
  }
  _process_list.head = NULL;
  _process_list.tail = NULL;
}*/

void mq_free(message_queue* mq) {
  MessageEnvelope* env;
  while (!mq_is_empty(mq)) {
    env = mq_dequeue(mq);
    free(env);
  }
  free(mq);
}

 
int ppq_free(priority_process_queue* ppq) {
  //assume all PCBs have been safely deallocated, only free queue memory
  //TODO: make less retarded
  free(ppq);
return 0;
}
