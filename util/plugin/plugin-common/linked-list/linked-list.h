// Copyright 2016 Silicon Laboratories, Inc.                                *80*

#ifndef __LINKED_LIST_H
#define __LINKED_LIST_H

typedef struct _EmberAfPluginLinkedListElement {
  struct _EmberAfPluginLinkedListElement* next;
  void* content;
} EmberAfPluginLinkedListElement;

typedef struct {
  EmberAfPluginLinkedListElement* head;
  EmberAfPluginLinkedListElement* tail;
  uint32_t count;
} EmberAfPluginLinkedList;

/** @brief Linked List Init
 *
 * This function will return an initialized and empty LinkedList object.  This
 * makes use of `malloc()` and can be freed when the `emberAfPluginLinkedListDeinit()`
 * function is called.
 */
EmberAfPluginLinkedList* emberAfPluginLinkedListInit(void);

/** @brief Linked List Deinit
 *
 * This function will empty and free a LinkedList object.
 *
 * @param list Pointer to the list to clear and free
 */
void emberAfPluginLinkedListDeinit(EmberAfPluginLinkedList* list);

/** @brief Linked List Push Back
 *
 * This function will push an element on to the back of a list.  This
 * makes use of `malloc()` and will call the corresponding `free()` the pop
 * function is called.
 *
 * @param list Pointer to the list to push the element to
 * @param content void pointer to an object or value to push to back of the list
 */
void emberAfPluginLinkedListPushBack(EmberAfPluginLinkedList* list, void* content);

/** @brief Linked List Pop Front
 *
 * This function will pop an element off of the front of a list.
 *
 * @param list Pointer to the list to pop an element from
 */
void emberAfPluginLinkedListPopFront(EmberAfPluginLinkedList* list);

/** @brief Linked List Next Element
 *
 * This function returns a pointer to the next element to the provided element.
 * If the provided element is NULL, it will return the head item on the list.
 *
 * @param list Pointer to the list to get the next element from
 * @param elementPosition Pointer to the list element to get the next item from
 */
EmberAfPluginLinkedListElement* emberAfPluginLinkedListNextElement(EmberAfPluginLinkedList* list,
	                                                               EmberAfPluginLinkedListElement* elementPosition);

#endif //__LINKED_LIST_H
