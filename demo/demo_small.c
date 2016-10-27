//
// Created by v on 16-10-21.
//

#include <stdio.h>
#include <malloc.h>

void TestLongConvert() {
    struct tagNormal {
        int base;
        char tt[32];
    };

    struct tagNormal *nm = (struct tagNormal*)malloc(sizeof(*nm));

    printf("normal: %p \n", nm);
    printf("(int)nm: %x \n", (int)nm);
    printf("(long)nm: %x \n", (long)nm);
    printf("(int)nm: %x \n", (int*)nm);
}

int main() {
    TestLongConvert();

    return 0;
}
