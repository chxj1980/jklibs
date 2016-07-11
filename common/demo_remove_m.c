//
// Created by v on 16-3-3.
// Author: jmdvirus@roamter.com
//

#include <stdio.h>

int main() {
    char *teststring = "code.org/^M\"";
    char *p = teststring;
    char save[1024] = {0};
    char *n = save;
    while (p) {
        if (*p == '\0') break;
        printf("*p: %c\n", *p);
        if (*p == '\r' && *(p+1) == '\n') {
            printf("hello 0d\n");
            p++;
        } else {
            *n = *p;
            p++;
            n++;
        }
    }

    return 0;
}