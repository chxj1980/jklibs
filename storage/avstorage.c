//
// Created by v on 19-2-22.
//

#include <string.h>
#include <stdio.h>
#include <unistd.h>

#include "avstorage.h"
#include "cm_utils.h"
#include "cm_logprint.h"

struct tagAVStorage {
    char         szSavePath[256];
    unsigned int          iTimePerFile;
    unsigned int       iSizePerFile;

    char         szFilePrefix[64];

    char         szCurrentFileName[512];
    char         szExt[8];

    FILE         *pWrite;
    time_t       iLastOpen;
} ;

int cm_avstorage_init(AVStorageP *stor, const char *savePath,
                      unsigned int timePerFile, unsigned int sizePerFile)
{
    AVStorageP in = (AVStorageP) cm_mem_malloc(sizeof(struct tagAVStorage));
    if (!in) return -1;

    cm_strncpy(in->szSavePath, savePath, strlen(savePath)+1);
    in->iTimePerFile = timePerFile;
    in->iSizePerFile = sizePerFile;
    in->szCurrentFileName[0] = '\0';
    strcpy(in->szFilePrefix, "jk-avstorage");
    strcpy(in->szExt, "h265");
    in->pWrite = NULL;
    in->iLastOpen = 0;

    cmdebug("avstorage init done\n");

    *stor = in;
    return 0;
}

int cm_avstorage_deinit(AVStorageP *stor)
{
    if (!stor) return -1;
    AVStorageP in = *stor;
    if (in) {
        cm_mem_free(in);
    }
    *stor = NULL;
    cmdebug("avstorage deinit done\n");
    return 0;
}

static int cm_avstorage_generate_filename(AVStorageP stor)
{
    if (!stor) return -1;
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    sprintf(stor->szCurrentFileName, "%s/%s-%04d%02d%02d-%02d%02d%02d.%s", stor->szSavePath,
            stor->szFilePrefix, tm->tm_year+1900, tm->tm_mon+1,
            tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec,
            stor->szExt);

    cminfo("avstorage generate filename %s\n", stor->szCurrentFileName);
    return 0;
}

static int cm_avstorage_reopen(AVStorageP stor)
{
    if (!stor) return -1;

    if (stor->pWrite) {
        fflush(stor->pWrite);
        fclose(stor->pWrite);
    }
    cm_avstorage_generate_filename(stor);
    stor->pWrite = fopen(stor->szCurrentFileName, "w");

    return stor->pWrite == NULL ? -1 : 0;
}

int cm_avstorage_v_push(AVStorageP stor, unsigned char *data, size_t len, int forceNew)
{
    if (!stor || !data || len <= 0) return -1;

    int reopen = 0;
    time_t now = time(NULL);
    if (stor->iLastOpen == 0) {
        stor->iLastOpen = now;
    } else if (now - stor->iLastOpen >= stor->iTimePerFile) {
        reopen = 1;
    }

//    cmdebug("avstorage forceNew [%d], filename [%s], reopen [%d]\n",
//            forceNew, stor->szCurrentFileName, reopen);
    if (forceNew || stor->szCurrentFileName[0] == '\0' || reopen) {
        int ret = cm_avstorage_reopen(stor);
        if (ret < 0) {
            return -2;
        }
        cmdebug("avstorage need open file [%s]\n", stor->szCurrentFileName);
        stor->iLastOpen = now;
    }


    size_t ret = fwrite(data, 1, len, stor->pWrite);
    if (ret < 0) {
        cmerror("avstorage write error %d\n", ret);
        return -3;
    }

    return ret;
}