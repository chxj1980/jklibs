//
// Created by v on 19-2-22.
//

#include <time.h>
#include <unistd.h>

#include "avstorage.h"

#include "cm_logprint.h"

CMLogPrint logPrint;

void avstorage_test() {
    AVStorageP stor;
    cm_avstorage_init(&stor, "/tmp", 10, 20);

    unsigned int timeInter = 100;
    time_t last = time(NULL);
    unsigned char dataToWrite[1024];
    size_t dataLen = 1024;
    cminfo("Start \n")
    while (1) {
        time_t t = time(NULL);
        if (t - last > timeInter) break;

        cm_avstorage_v_push(stor, dataToWrite, dataLen, 0);
        sleep(1);
    }

    cminfo("End\n");

    cm_avstorage_deinit(&stor);
}

int main(int argc, char **args) {
    CM_LOG_PRINT_INIT("avstorage");
    avstorage_test();
    return 0;
}
