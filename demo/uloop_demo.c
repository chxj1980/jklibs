//
// Created by v on 16-7-13.
// Author: jmdvirus@roamter.com
//

#include <stdio.h>

#include "libubox/uloop.h"

struct uloop_timeout demo;

static uloop_timeout_handler do_some(struct uloop_timeout *t) {
    printf("This is uloop print\n");
    uloop_timeout_set(t, 3000);
    return NULL;
}

int main() {
    printf("Start uloop demo \n");

    uloop_init();

    demo.cb = do_some;
    demo.pending = false;

    uloop_timeout_add(&demo);
    uloop_timeout_set(&demo, 2000);

    uloop_run();

    printf("End uloop demo \n");
}