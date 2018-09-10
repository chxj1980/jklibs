//
// Created by v on 16-8-8.
//

#include <stdio.h>
#include <malloc.h>

#include "rt_print.h"

char *base_demo_func_return()
{
    char p[20] = "hello";
    return p;
}

int base_demo_size_func(char p1[100])
{
    printf("func sizeof(char p1) = %ld\n", sizeof(p1));
    return 0;
}

int base_demo_size_o()
{
    char *p = (char*)malloc(100);
    
    printf("sizeof(char*p) = %ld\n", sizeof(p));

    char p1[20] = "hello";
    printf("sizeof(char p1) = %ld\n", sizeof(p1));

    void *p2 = (void*)malloc(50);
    printf("sizeof(void *p2) = %ld\n", sizeof(p2));

    if (p) free(p);
    if (p2) free(p2);
    return 0;
}

int base_demo_size()
{
    size_t t;
    int i;
    short s;
    unsigned int ui;
    unsigned short us;
    char c;
    unsigned char uc;
    float f;
    long l;
    long long ll;
    unsigned long ul;
    unsigned  long long ull;

    printf("size_t = %ld\n", sizeof(t));
    printf("int = %ld\n", sizeof(i));
    printf("short = %ld\n", sizeof(s));
    printf("unsigned int = %ld\n", sizeof(ui));
    printf("unsigned short = %ld\n", sizeof(us));
    printf("char = %ld\n", sizeof(c));
    printf("unsigned char = %ld\n", sizeof(uc));
    printf("float = %ld\n", sizeof(f));
    printf("long = %ld\n", sizeof(l));
    printf("long long = %ld\n", sizeof(ll));
    printf("unsigned long = %ld\n", sizeof(ul));
    printf("unsigned long long = %ld\n", sizeof(ull));
    printf("short* = %ld\n", sizeof(short*));
    printf("unsigned short * = %ld\n", sizeof(unsigned short *));
    printf("char * = %ld\n", sizeof(char*));
    printf("float * = %ld\n", sizeof(float*));
    return 0;
}

int base_demo_shit_length()
{
    printf("1<<31 = %d\n", (1<<31)-1);
    return 0;
}

int main() {
    base_demo_size();
    base_demo_size_o();

    char t[100];
    base_demo_size_func(t);

    char *p = base_demo_func_return();
    printf("p = %s\n", p);

    base_demo_shit_length();
    return 0;
}
