/**
 *===================================================================
 *
 *          Name: cm_list.h
 *        Create: 2014年04月17日 星期四 16时08分31秒
 *
 *   Discription: 
 *       Version: 1.0.0
 *
 *
 *===================================================================
 */
#ifndef __CM_LIST_H
#define __CM_LIST_H

//
// This list if from linux kernel
//
struct list_head {
    struct list_head *next, *prev;
};

/*
 * init head let prev and next all point to head
 */
#define CM_INIT_LIST_HEAD(type) \
           type->prev = type; type->next = type;

// Init head
//
void cm_init_list_head(struct list_head *head);

// add to tail
//
void cm_list_add_tail(struct list_head *item, struct list_head *head);
// add to front
//
void cm_list_add_front(struct list_head *item, struct list_head *head);

//
// If this is the only one item, I will not del it
// You can free it and set point to NULL
// Or do nothing for next use
//
void cm_list_del_item(struct list_head *item);

//
// return 1--is empty (in fact it is the only one item)
//        0 -- not empty
//
int cm_list_empty(struct list_head *head);

#define jkoffsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)

#define cm_container_of(ptr, type, member)   ({ \
      const typeof(((type*)0)->member) *_mptr = (ptr);  \
         (type*)((char*)_mptr - jkoffsetof(type, member)); \
                                 })

//
// Use this to get the struct point
// ptr -- current point of struct list_head
// type -- the struct name(the struct define name
//         not object or point name) of where you put the list_head struct
// member -- what you call of the struct list_head name in the struct you use
// example:
// typedef struct {
//     struct list_head test_list_head;
// } test_list;
// test_list *global = ...
// struct list_head *cur = global->test_list_head
// now the ptr is cur(cur->next, cur->prev)
// ptr is what struct we want to access and use 
// type is test_list
// member is test_list_head
//
#define cm_list_item(ptr, type, member) \
       cm_container_of(ptr, type, member)

#define cm_list_each(pos, head) \
    for (pos = (head)->next; pos != (head); pos = pos->next)

#define cm_list_each_safe(pos, n, head) \
    for (pos = (head)->next, n = pos->next; pos != (head); \
        pos = n, n = pos->next)

#define cm_list_each_entry(pos, head, member)              \
    for (pos = cm_list_item((head)->next, typeof(*pos), member);  \
         &pos->member != (head);    \
         pos = cm_list_item(pos->member.next, typeof(*pos), member))

#endif // CM_LIST_H

/*=============== End of file: cm_list.h =====================*/
