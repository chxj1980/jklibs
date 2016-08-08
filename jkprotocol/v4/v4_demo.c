//
// Created by v on 16-8-4.
//
#include <stdio.h>
#include <string.h>
#include <malloc.h>

#include "v4.h"

#include "rt_print.h"

char *globaldata = NULL;

int demo_v4_generate() {
    JKProV4 v4;
    int ret = jk_pro_v4_init(&v4, 1, 0);
    if (ret != 0) {
        rterror("Error init v4 protocol %d", ret);
        return -1;
    }

    char *sdata = "This is demo.";
    ret = v4.set_header(&v4, 0, strlen(sdata));
    if (ret != 0) {
        rterror("Error set header %d", ret);
        return -2;
    }

    ret = v4.set_body(&v4, sdata);
    if (ret != 0) {
        rterror("Error set body %d", ret);
        return -3;
    }

    ret = v4.to_bytes(&v4, &globaldata);
    if (ret != 0) {
        rterror("Error set to bytes %d", ret);
        return -4;
    }

    // Free result
//    if (result) {
//        free(result);
//        result = NULL;
//    }

    v4.deinit(&v4);

    return 0;
}

int demo_v4_parse(const char *result) {
    JKProV4 v4;
    jk_pro_v4_init(&v4, 0, 0);
    int ret = v4.parse(&v4, result);
    if (ret != 0) {
        rterror("Error parse %d", ret);
        return -1;
    }

    rtinfo("data: %s", v4.szBody.szData);

    v4.deinit(&v4);
    return 0;
}

int main() {

    demo_v4_generate();

    demo_v4_parse(globaldata);

    if (globaldata) {
        free(globaldata);
    }

    return 0;
}

