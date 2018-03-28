/*
 */

#include "qrencode.h"

#include "rt_print.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <malloc.h>

int main(int argc, char **args) {
    char *file = args[1];
    if (access(file, F_OK) != 0) {
        rterror("Error open file [%s]", file);
        return -1;
    }

    struct stat st;
    int ret = stat(file, &st);
    if (ret != 0) {
        rterror("Error stat file [%s] [%d]", file, ret);
        return -2;
    }

    rtinfo("file [%s] size is [%u]", file, st.st_size);
    unsigned char *data = (unsigned char*)malloc(st.st_size);
    long len = st.st_size;

    FILE *f = fopen(file, "r");
    if (f) {
        len = fread(data, 1, len, f);
        fclose(f);
    }

    rtinfo("Got data of image [%ld]", len);

    QRcode *code = QRcode_encodeData(len, (const unsigned char*)data, 0, QR_ECLEVEL_M);

    rtinfo("version [%d] width [%d], data [%s]", code->version, 
        code->width, code->data);

    if (data) free(data);
    QRcode_free(code);

    return 0;
}
