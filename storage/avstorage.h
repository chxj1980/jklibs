//
// Created by v on 19-2-22.
//

#ifndef JKLIBS_AVSTORAGE_H
#define JKLIBS_AVSTORAGE_H

/**
 * Used for save av files
 */

#ifdef __cpluplus
extern "C" {
#endif

typedef struct tagAVStorage *AVStorageP;

int cm_avstorage_init(AVStorageP *stor, const char *savePath,
                      unsigned int timePerFile, unsigned int sizePerFile);

int cm_avstorage_deinit(AVStorageP *stor);

int cm_avstorage_v_push(AVStorageP stor, unsigned char *data, size_t len, int forceNew);

#ifdef __cplusplus
}
#endif

#endif //JKLIBS_AVSTORAGE_H
