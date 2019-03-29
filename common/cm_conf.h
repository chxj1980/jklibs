
/*
 */
#ifndef __MISC_JKCONF_H
#define __MISC_JKCONF_H

#include <stdio.h>

typedef struct {
    char             key[32];
    char             value[256];
} CMConfig;

int cm_config_read(const char *file, CMConfig *conf, int max);
int cm_config_write(const char *file, CMConfig *conf, int max);

char *cm_config_value(CMConfig *conf, int max, char *key);
/**
 * Insert value return index
 * @param conf
 * @param max current count, will add one if add a new one.
 * @param key
 * @param value
 * @return index of where add
 */
int cm_config_set_value(CMConfig *conf, int max, char *key, char *value);

/**
 * Write @buf to @filename
 * @param filename
 * @param buf
 * @param len
 * @return
 */
int cm_config_write_buf(const char *filename, char *buf, int len);

/**
 * Read to @buf from @filename
 * @param filename
 * @param buf
 * @param len
 * @return write length
 */
int cm_config_read_buf(const char *filename, char *buf, int len);

#endif
