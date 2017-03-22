/*
*********************************************************************************************************
*                                               uC/Common
*                                   Common Features for Micrium Stacks
*
*                         (c) Copyright 2013-2015; Micrium, Inc.; Weston, FL
*
*               All rights reserved.  Protected by international copyright laws.
*
*               uC/Common is provided in source form to registered licensees ONLY.  It is
*               illegal to distribute this source code to any third party unless you receive
*               written permission by an authorized Micrium representative.  Knowledge of
*               the source code may NOT be used to develop a similar product.
*
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*
*               You can find our product's user manual, API reference, release notes and
*               more information at https://doc.micrium.com
*
*               You can contact us at http://www.micrium.com
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                uC/Common - Singly-linked Lists (SList)
*
* Filename      : slist.h
* Version       : V1.01.00
* Programmer(s) : EJ
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*********************************************************************************************************
*                                               MODULE
*
* Note(s) : (1) This library header file is protected from multiple pre-processor inclusion through
*               use of the library module present pre-processor macro definition.
*********************************************************************************************************
*********************************************************************************************************
*/

#ifndef  COLL_SLIST_MODULE_PRESENT                              /* See Note #1.                                         */
#define  COLL_SLIST_MODULE_PRESENT


/*
*********************************************************************************************************
*********************************************************************************************************
*                                            INCLUDE FILES
*********************************************************************************************************
*********************************************************************************************************
*/

#include  <cpu.h>


/*
*********************************************************************************************************
*********************************************************************************************************
*                                             DATA TYPES
*********************************************************************************************************
*********************************************************************************************************
*/

typedef  struct  slist_member  SLIST_MEMBER;

struct  slist_member {
    SLIST_MEMBER  *p_next;
};


/*
*********************************************************************************************************
*********************************************************************************************************
*                                              MACROS
*********************************************************************************************************
*********************************************************************************************************
*/


#define  OFFSET_OF(type, member)                      ((CPU_SIZE_T)&(((type *)0)->member))
#define  CONTAINER_OF(p_member, parent_type, member)  (parent_type *)((CPU_ADDR)(p_member) - ((CPU_ADDR)(&((parent_type *)0)->member)))

#define  SLIST_FOR_EACH(list_head, iterator)          for ((iterator) = (list_head); (iterator) != DEF_NULL; (iterator) = (iterator)->p_next)

#define  SLIST_FOR_EACH_ENTRY(list_head, entry, type, member) for (  (entry) = SLIST_ENTRY(list_head, type, member); \
                                                                   &((entry)->member.p_next) != DEF_NULL; \
                                                                     (entry) = SLIST_ENTRY((entry)->member.p_next, type, member))

#define  SLIST_ENTRY                                  CONTAINER_OF


/*
*********************************************************************************************************
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*********************************************************************************************************
*/


void           SList_Init     (SLIST_MEMBER  **p_head_ptr);

void           SList_Push     (SLIST_MEMBER  **p_head_ptr,
                               SLIST_MEMBER   *p_item);

void           SList_PushBack (SLIST_MEMBER  **p_head_ptr,
                               SLIST_MEMBER   *p_item);

SLIST_MEMBER  *SList_Pop      (SLIST_MEMBER  **p_head_ptr);

void           SList_Add      (SLIST_MEMBER   *p_item,
                               SLIST_MEMBER   *p_pos);

void           SList_Rem      (SLIST_MEMBER  **p_head_ptr,
                               SLIST_MEMBER   *p_item);

void           SList_Sort     (SLIST_MEMBER  **p_head_ptr,
                               CPU_BOOLEAN   (*cmp_fnct)(SLIST_MEMBER  *p_item_l, SLIST_MEMBER  *p_item_r));

#endif /* COLL_SLIST_MODULE_PRESENT */
