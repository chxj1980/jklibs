/*
 *===========================================================================
 *
 *          Name: isProgramRunning.c
 *        Create: 2015年09月23日 星期三 15时48分12秒
 *
 *   Discription: 
 *
 *        Author: jmdvirus
 *         Email: jmdvirus@roamter.com
 *
 *===========================================================================
 */

#include <stdio.h>

#include "bvpu_utils.h"

int main(int argc, char **args)
{
    if (argc <= 1) {
        printf("Usage: %s programname\n", args[0]);
        return -1;
    }
    return is_program_running(argc-1, (const char**)args);

    return 0;
}


/*=============== End of file: isProgramRunning.c ==========================*/
