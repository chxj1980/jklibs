//
// Created by v on 16-8-8.
//

#include <stdio.h>

#include "rt_print.h"

int base_demo_size()
{
    size_t t;
    int i;
    short s;
    unsigned int ui;
    unsigned short us;
    char c;
    unsigned char uc;
    long l;
    long long ll;
    unsigned long ul;
    unsigned  long long ull;

    printf("size_t = %d\n", sizeof(t));
    printf("int = %d\n", sizeof(i));
    printf("short = %d\n", sizeof(s));
    printf("unsigned int = %d\n", sizeof(ui));
    printf("unsigned short = %d\n", sizeof(us));
    printf("char = %d\n", sizeof(c));
    printf("unsigned char = %d\n", sizeof(uc));
    printf("long = %d\n", sizeof(l));
    printf("long long = %d\n", sizeof(ll));
    printf("unsigned long = %d\n", sizeof(ul));
    printf("unsigned long long = %d\n", sizeof(ull));
    return 0;
}

int base_demo_shit_length()
{
    printf("1<<31 = %d", 1<<31-1);
    return 0;
}

int main() {
    base_demo_size();

    base_demo_shit_length();
    return 0;
}
