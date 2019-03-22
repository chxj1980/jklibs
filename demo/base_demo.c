//
// Created by v on 16-8-8.
//

#include <stdio.h>
#include <malloc.h>
#include <errno.h>
#include <string.h>

#include "cm_print.h"

CMLogPrint logPrint;

char *base_demo_func_return()
{
    char p[20] = "hello";
    return p;
}

int base_demo_size_func(char p1[100])
{
    printf("func sizeof(char p1) = %ld\n", sizeof(*p1));
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

    printf("sizeof size_t = %ld\n", sizeof(t));
    printf("sizeof int = %ld\n", sizeof(i));
    printf("sizeof short = %ld\n", sizeof(s));
    printf("sizeof unsigned int = %ld\n", sizeof(ui));
    printf("sizeof unsigned short = %ld\n", sizeof(us));
    printf("sizeof char = %ld\n", sizeof(c));
    printf("sizeof unsigned char = %ld\n", sizeof(uc));
    printf("sizeof float = %ld\n", sizeof(f));
    printf("sizeof long = %ld\n", sizeof(l));
    printf("sizeof long long = %ld\n", sizeof(ll));
    printf("sizeof unsigned long = %ld\n", sizeof(ul));
    printf("sizeof unsigned long long = %ld\n", sizeof(ull));
    printf("sizeof short* = %ld\n", sizeof(short*));
    printf("sizeof unsigned short * = %ld\n", sizeof(unsigned short *));
    printf("sizeof char * = %ld\n", sizeof(char*));
    printf("sizeof float * = %ld\n", sizeof(float*));
    return 0;
}

int base_demo_shit_length()
{
    printf("1<<31 = %d\n", (1<<31)-1);
    return 0;
}

int base_demo_priority()
{
	int x = 6, y = 2, z = 3;
	printf("6/2*3=%d\n", x/y*z);
	printf("(6/2)*3 = %d\n", (x/y)*z);
	return 0;
}

int main(int argc, char **argv) {

	if (argc > 1) {
		char *a = argv[1];
		int e = atoi(a);
		printf("strerror(%d) = %s\n", e, strerror(e));
	}

    base_demo_size();
    base_demo_size_o();

    char t[100];
    base_demo_size_func(t);

    char *p = base_demo_func_return();
    printf("p = %s\n", p);

    base_demo_shit_length();

	base_demo_priority();
    return 0;
}
