/*
 *===========================================================================
 *
 *          Name: cm_list.c
 *        Create: 2014年04月17日 星期四 14时49分36秒
 *
 *   Discription: 
 *
 *===========================================================================
 */

#include <stdio.h>
#include <stdlib.h>

#include "cm_list.h"

void cm_init_list_head(struct list_head *head)
{
    head->prev = head;
    head->next = head;
}

void cm_list_add(struct list_head *item, struct list_head *prev, 
         struct list_head *next)
{
    next->prev = item;
    item->next = next;
    item->prev = prev;
    prev->next = item;
}

void cm_list_add_tail(struct list_head *item, struct list_head *head)
{
    cm_list_add(item, head->prev, head);
}

void cm_list_add_front(struct list_head *item, struct list_head *head)
{
    cm_list_add(item, head, head->next);
}

void cm_list_del(struct list_head *prev, struct list_head *next)
{
    next->prev = prev;
    prev->next = next;
}

void cm_list_del_item(struct list_head *item)
{
    cm_list_del(item->prev, item->next);
}

int cm_list_empty(struct list_head *head)
{
    struct list_head *last = head->next;
    return last->next == last->next->next;
}

#if 0
typedef struct {
    struct list_head test_list_item;
    int       id;
    char      name[16];
} test_list;

int main()
{
    test_list global_start;
    cm_init_list_head(&global_start.test_list_item);

    test_list *global = (test_list*)malloc(sizeof(test_list));
    if (!global) return -1;
    global->id = 23;
    sprintf(global->name, "test");

    cm_list_add_front(&global->test_list_item, &global_start.test_list_item);

    printf("1 [%p] [%p][%p]\n", &global->test_list_item, global->test_list_item.next, global->test_list_item.prev);

    test_list *item = (test_list*)malloc(sizeof(test_list));
    if (!item) return -1;
    item->id = 32;
    sprintf(item->name, "tvalue");
    cm_list_add_front(&item->test_list_item, &global_start.test_list_item);
    printf("2 [%p]\n", &item->test_list_item);

    test_list *newitem = (test_list*)malloc(sizeof(test_list));
    if (!newitem) return -2;
    newitem->id = 83;
    sprintf(newitem->name, "down");
    cm_list_add_front(&newitem->test_list_item, &global_start.test_list_item);
    printf("3 [%p]\n", &newitem->test_list_item);

    struct list_head *tmp1, *p;
    cm_list_each_safe(tmp1,p, &global_start.test_list_item) {
        test_list *find = cm_list_item(tmp1, test_list, test_list_item);
        printf("---------[ %p] --- [%d][%s]\n", tmp1, find->id, find->name);
    }

    test_list *gg = global;
    cm_list_each_entry(gg, &global_start.test_list_item, test_list_item) {
        printf("------ %p ---- %d\n", gg, gg->id);
    }

#if 0
    {
    // Before del item we print all out
    struct list_head *cur = head->next;
    do {
        test_list *find = cm_list_item(cur, test_list, test_list_item);
        if (find) printf("----[%p]--------- [%d] [%s]\n", cur, find->id, find->name);
        if (cur == head) break;
        cur = cur->next;
    } while(1);
    }

    struct list_head *tmp = head;
    struct list_head *cur = head->next;  // resever next point
    cm_list_del_item(tmp);
    int ii = cm_list_empty(head);
    printf("----should not 1: %d\n", ii);
    head = cur;
    tmp = head;
    cur = head->next;
    cm_list_del_item(tmp);
    head = cur;

    // this is the last item
    int i = cm_list_empty(head);
    printf("----------%d\n",i);

    // When we delete two item, let's see again
    do {
        test_list *find = cm_list_item(cur, test_list, test_list_item);    
        if (find) printf("------------- [%d] [%s]\n", find->id, find->name);
        if (cur == head) break;
        cur = cur->next;
    } while(1);
#endif

    return 0;
}
#endif

/*=============== End of file: cm_list.c ==========================*/
