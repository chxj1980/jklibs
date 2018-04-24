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
    printf("func sizeof(char p1) = %d\n", sizeof(p1));
    return 0;
}

int base_demo_size_o()
{
    char *p = (char*)malloc(100);
    
    printf("sizeof(char*p) = %d\n", sizeof(p));

    char p1[20] = "hello";
    printf("sizeof(char p1) = %d\n", sizeof(p1));

    void *p2 = (void*)malloc(50);
    printf("sizeof(void *p2) = %d\n", sizeof(p2));

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

    printf("size_t = %d\n", sizeof(t));
    printf("int = %d\n", sizeof(i));
    printf("short = %d\n", sizeof(s));
    printf("unsigned int = %d\n", sizeof(ui));
    printf("unsigned short = %d\n", sizeof(us));
    printf("char = %d\n", sizeof(c));
    printf("unsigned char = %d\n", sizeof(uc));
    printf("float = %d\n", sizeof(f));
    printf("long = %d\n", sizeof(l));
    printf("long long = %d\n", sizeof(ll));
    printf("unsigned long = %d\n", sizeof(ul));
    printf("unsigned long long = %d\n", sizeof(ull));
    printf("short* = %d\n", sizeof(short*));
    printf("unsigned short * = %d\n", sizeof(unsigned short *));
    printf("char * = %d\n", sizeof(char*));
    printf("float * = %d\n", sizeof(float*));
    return 0;
}

int base_demo_shit_length()
{
    printf("1<<31 = %d\n", 1<<31-1);
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
