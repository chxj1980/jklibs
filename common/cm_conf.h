
/*
 */
#ifndef __MISC_JKCONF_H
#define __MISC_JKCONF_H

#include <stdio.h>

typedef struct {
    char             key[32];
    char             value[256];
} CMConfig;

int config_read(const char *file, CMConfig *conf, int max);
int config_write(const char *file, CMConfig *conf, int max);

char *config_value(CMConfig *conf, int max, char *key);
/**
 * Insert value return index
 * @param conf
 * @param max current count, will add one if add a new one.
 * @param key
 * @param value
 * @return index of where add
 */
int config_set_value(CMConfig *conf, int max, char *key, char *value);

/**
 * Write @buf to @filename
 * @param filename
 * @param buf
 * @param len
 * @return
 */
int config_write_buf(const char *filename, char *buf, int len);

/**
 * Read to @buf from @filename
 * @param filename
 * @param buf
 * @param len
 * @return write length
 */
int config_read_buf(const char *filename, char *buf, int len);

#endif