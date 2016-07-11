/*
 *===========================================================================
 *
 *          Name: jk_list.c
 *        Create: 2014年04月17日 星期四 14时49分36秒
 *
 *   Discription: 
 *
 *        Author: yuwei.zhang
 *         Email: yuwei.zhang@besovideo.com
 *
 *===========================================================================
 */

#include <stdio.h>
#include <stdlib.h>

#include "jk_list.h"

void jk_init_list_head(struct list_head *head)
{
    head->prev = head;
    head->next = head;
}

void jk_list_add(struct list_head *item, struct list_head *prev, 
         struct list_head *next)
{
    next->prev = item;
    item->next = next;
    item->prev = prev;
    prev->next = item;
}

void jk_list_add_tail(struct list_head *item, struct list_head *head)
{
    jk_list_add(item, head, head->next);
}

void jk_list_add_front(struct list_head *item, struct list_head *head)
{
    jk_list_add(item, head->prev, head);
}

void jk_list_del(struct list_head *prev, struct list_head *next)
{
    next->prev = prev;
    prev->next = next;
}

void jk_list_del_item(struct list_head *item)
{
    jk_list_del(item->prev, item->next);
}

int jk_list_empty(struct list_head *head)
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
    test_list *global = (test_list*)malloc(sizeof(test_list));
    if (!global) return -1;
    global->id = 23;
    sprintf(global->name, "test");

    struct list_head *head = &global->test_list_item;
    jk_init_list_head(head);

    test_list *item = (test_list*)malloc(sizeof(test_list));
    if (!item) return -1;
    item->id = 32;
    sprintf(item->name, "tvalue");
    jk_list_add_tail(&item->test_list_item, head);

    test_list *newitem = (test_list*)malloc(sizeof(test_list));
    if (!newitem) return -2;
    newitem->id = 83;
    sprintf(newitem->name, "down");
    jk_list_add_front(&newitem->test_list_item, head);

    {
    // Before del item we print all out
    struct list_head *cur = head->next;
    do {
        test_list *find = jk_list_item(cur, test_list, test_list_item);    
        if (find) printf("------------- [%d] [%s]\n", find->id, find->name);
        if (cur == head) break;
        cur = cur->next;
    } while(1);
    }

    struct list_head *tmp = head;
    struct list_head *cur = head->next;  // resever next point
    jk_list_del_item(tmp);
    int ii = jk_list_empty(head);
    printf("----should not 1: %d\n", ii);
    head = cur;
    tmp = head;
    cur = head->next;
    jk_list_del_item(tmp);
    head = cur;

    // this is the last item
    int i = jk_list_empty(head);
    printf("----------%d\n",i);

    // When we delete two item, let's see again
    do {
        test_list *find = jk_list_item(cur, test_list, test_list_item);    
        if (find) printf("------------- [%d] [%s]\n", find->id, find->name);
        if (cur == head) break;
        cur = cur->next;
    } while(1);

    return 0;
}
#endif

/*=============== End of file: jk_list.c ==========================*/
