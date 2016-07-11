/**
 *===================================================================
 *
 *          Name: jk_list.h
 *        Create: 2014年04月17日 星期四 16时08分31秒
 *
 *   Discription: 
 *       Version: 1.0.0
 *
 *        Author: yuwei.zhang@besovideo.com
 *
 *===================================================================
 */
#ifndef __JK_LIST_H
#define __JK_LIST_H

//
// This list if from linux kernel
//
struct list_head {
    struct list_head *next, *prev;
};

/*
 * init head let prev and next all point to head
 */
#define JK_INIT_LIST_HEAD(type) \
           type->prev = type; type->next = type;

// Init head
//
void jk_init_list_head(struct list_head *head);

// add to tail
//
void jk_list_add_tail(struct list_head *item, struct list_head *head);
// add to front
//
void jk_list_add_front(struct list_head *item, struct list_head *head);

//
// If this is the only one item, I will not del it
// You can free it and set point to NULL
// Or do nothing for next use
//
void jk_list_del_item(struct list_head *item);

//
// return 1--is empty (in fact it is the only one item)
//        0 -- not empty
//
int jk_list_empty(struct list_head *head);

#define jkoffsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)

#define jk_container_of(ptr, type, member)   ({ \
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
#define jk_list_item(ptr, type, member) \
       jk_container_of(ptr, type, member)

#endif // JK_LIST_H

/*=============== End of file: jk_list.h =====================*/
