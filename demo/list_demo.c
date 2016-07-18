//
// Created by v on 16-7-18.
// Author: jmdvirus@roamter.com
//

#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include "libubox/list.h"

struct DemoList {
    struct list_head head;
    int    data;
};

struct list_head demos = LIST_HEAD_INIT(demos);

void debug_lists()
{
    struct DemoList *item;
    list_for_each_entry(item, &demos, head) {
        if (item) {
            printf("item.data: %d\n", item->data);
        }
    }
}

int demo_list()
{
    debug_lists();
    printf("\n");

    struct DemoList listAdd;
    listAdd.data = 32;
    list_add(&listAdd.head, &demos);

    debug_lists();
    printf("\n");

    struct DemoList listAdd2;
    listAdd2.data = 99;
    list_add_tail(&listAdd2.head, &demos);

    debug_lists();
    printf("\n");

    struct DemoList listAdd3;
    listAdd3.data = 199;
    list_add_tail(&listAdd3.head, &demos);

    debug_lists();
    printf("\n");

    list_del(&listAdd2);

    debug_lists();
    printf("\n");
    return 0;
}

int main() {
    demo_list();

    return 0;
}