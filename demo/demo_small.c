//
// Created by v on 16-10-21.
//

#include <stdio.h>
#include <string.h>
//#include <malloc.h>

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

void *hello;

void hello_test() {
    int a = 23;
    hello = &a;
}

void findsome() {
    char *dst = "Content-Length:28";
    char *p = strstr(dst, "Content-Length");
    int length = 0;
    sscanf(p, "%*[a-zA-Z\-]%*[\:\ ]%d", &length);
    printf("-------- hello length: %d\n", length);
}

void format_arges(int n) {
    char *data = "839281211";
    char fmt[8];
    sprintf(fmt, "%%%dd", n);
    printf("fmt: %s\n", fmt);
    int save = 0;
    sscanf(data, fmt, &save);
    printf("give out %d\n", save);
}

void test_cmp() {

}

int main() {
//    TestLongConvert();

//    hello_test();
//    printf("------ heloo %d\n", *(int*)hello);

//    findsome();

    format_arges(4);
    return 0;
}
