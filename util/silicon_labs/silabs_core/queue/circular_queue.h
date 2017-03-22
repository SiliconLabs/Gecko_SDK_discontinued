/***************************************************************************//**
 * @file circular_queue.h
 * @brief A circular queue of pointers. If the queue becomes full the next add
 * will overwrite the values currently stored in the queue and advance the
 * pointers.
 *
 * @copyright Copyright 2015 Silicon Laboratories, Inc. http://www.silabs.com
 ******************************************************************************/

#ifndef CIRCULAR_QUEUE_H__
#define CIRCULAR_QUEUE_H__

// Get the standard include types
#include <stdint.h>
#include <stdbool.h>

#ifdef CONFIGURATION_HEADER
#include CONFIGURATION_HEADER
#endif

#ifndef MAX_QUEUE_LENGTH
#define MAX_QUEUE_LENGTH 10
#endif

// -----------------------------------------------------------------------------
// Structures and Types
// -----------------------------------------------------------------------------
typedef struct {
  uint16_t head;
  uint16_t count;
  uint16_t size;
  void* data[MAX_QUEUE_LENGTH];
} Queue_t;

// -----------------------------------------------------------------------------
// Standard Queue Functions
// -----------------------------------------------------------------------------

/**
 * Function to initialize a queue structure with the given size. Note that this
 * size must be less than the MAX_QUEUE_LENGTH define.
 * @param queue A pointer to the queue structure to initialize
 * @param size The number of entries we want to allow you to store in this queue
 * @return Returns true if we were able to initialize the queue and false
 * otherwise.
 */
bool queueInit(Queue_t *queue, uint16_t size);

/**
 * Add the specified data pointer to the end of the queue.
 * @param queue The queue to add the item to.
 * @param data The pointer object to store in the queue.
 * @return Returns true if the data was stored and false otherwise.
 */
bool queueAdd(Queue_t *queue, void *data);

/**
 * Remove an item from the head of the queue and return its pointer.
 * @param queue The queue to remove the item from.
 * @return Returns a pointer to the data that was at the head of the queue. If
 * the queue is empty this value will be NULL. It's worth noting that NULL can
 * also be a valid stored pointer.
 */
void* queueRemove(Queue_t *queue);

/**
 * Determine if the given queue is empty.
 * @param queue The queue to check the status of.
 * @return Returns true if the queue is empty and false otherwise.
 */
bool queueIsEmpty(Queue_t *queue);

/**
 * Determine if the given queue is full.
 * @param queue The queue to check the status of.
 * @return Returns true if the queue is full and false otherwise.
 */
bool queueIsFull(Queue_t *queue);

#endif // CIRCULAR_QUEUE_H__
