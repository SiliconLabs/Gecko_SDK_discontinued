/***************************************************************************//**
 * @file circular_queue.c
 * @brief The source for a circular queue of pointer values.
 * @copyright Copyright 2015 Silicon Laboratories, Inc. http://www.silabs.com
 ******************************************************************************/

#include <stdlib.h>

#include "circular_queue.h"

#include "em_core.h"

bool queueInit(Queue_t *queue, uint16_t size)
{
  // Make sure we have enough room for this queue
  if(size > MAX_QUEUE_LENGTH || queue == NULL)
  {
    return false;
  }

  // Atomically initialize the queue structure
  CORE_DECLARE_IRQ_STATE;
  CORE_ENTER_CRITICAL();
  queue->size = size;
  queue->head = 0;
  queue->count = 0;
  CORE_EXIT_CRITICAL();

  return true;
}

bool queueAdd(Queue_t *queue, void *data)
{
  // Do nothing if there's no queue given
  if(queue == NULL)
  {
    return false;
  }

  CORE_DECLARE_IRQ_STATE;
  CORE_ENTER_CRITICAL();
  if(queue->count < queue->size)
  {
    uint16_t index = (queue->head + queue->count) % queue->size;

    // Insert this item at the end of the queue
    queue->data[index] = data;
    queue->count++;
  } else {
    // Overwrite what's at the head of the queue since we're out of space
    queue->data[queue->head] = data;
    queue->head = (queue->head + 1) % queue->size;
  }
  CORE_EXIT_CRITICAL();

  return true;
}

void* queueRemove(Queue_t *queue)
{
  void* ptr = NULL;

  // Do nothing if there's no queue given
  if(queue == NULL)
  {
    return NULL;
  }

  CORE_DECLARE_IRQ_STATE;
  CORE_ENTER_CRITICAL();
  if(queue->count > 0)
  {
    ptr = queue->data[queue->head];
    queue->head = (queue->head + 1) % queue->size;
    queue->count--;
  }
  CORE_EXIT_CRITICAL();

  return ptr;
}

bool queueIsEmpty(Queue_t *queue)
{
  bool result;

  if(queue == NULL)
  {
    return true;
  }

  CORE_DECLARE_IRQ_STATE;
  CORE_ENTER_CRITICAL();
  result = (queue->count == 0);
  CORE_EXIT_CRITICAL();

  return result;
}

bool queueIsFull(Queue_t *queue)
{
  bool result;

  if(queue == NULL)
  {
    return true;
  }

  CORE_DECLARE_IRQ_STATE;
  CORE_ENTER_CRITICAL();
  result = (queue->count == queue->size);
  CORE_EXIT_CRITICAL();

  return result;
}

