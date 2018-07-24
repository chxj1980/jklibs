
/*
 */

#include "cm_conf.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <time.h>

int config_read(const char *file, CMConfig *conf, int max)
{
    FILE *f = NULL;
#ifdef _WIN32
    fopen_s(&f, file, "rb");
#else
    f = fopen(file, "r");
#endif
    if (f) 
    {   
        char line[128] = { 0 };
        int i = 0;
        while (1)
        {
            if (i > max) break;
            if (feof(f)) break;
            char *p = fgets(line, 128, f); 
            if (!p)
            {
                break;
            }
            if (line[0] == '\0' || line[0] == '#')
            {
                continue;
            }
            char key[32] = { 0 };
            char value[256] = { 0 };
#ifdef _WIN32
            char *sl = line;
            char *tosave = key;
            while(true)
            {
                if (*sl == '\0') break;
                if (*sl == '\r' || *sl == '\n')
                {
                    sl++;
                    continue;
                }
                if (*sl == '=')
                {
                    tosave = value;
                    sl++;
                }
                *tosave++ = *sl++;

            }
            int ret = 2;
//          int ret = sscanf_s(line, "%[^=]=%s", key, value);
#else
            int ret = sscanf(line, "%[^=]=%s", key, value);
#endif
            if (ret == 2)
            {
                strncpy(conf[i].key, key, sizeof(key));
                strncpy(conf[i].value, value, sizeof(value));
                i++;
            }
        }
        fclose(f);
        return i;
    }
    else 
    {
        return -2;
    }   

    return 0;
}

int config_write(const char *file, CMConfig *conf, int max)
{
    FILE *f = NULL;
    f = fopen(file, "w");
    if (!f) return -1;

    time_t now = time(NULL);
    char *timestr = ctime(&now);

    char line[512] = {0};
    sprintf(line, "%s @ %s\n", "# Generated by program.", timestr);
    fwrite(line, 1, strlen(line), f);

    int i;
    for (i = 0; i < max; i++) {
        sprintf(line, "%s=%s\n", conf[i].key, conf[i].value);
        fwrite(line, 1, strlen(line), f);
    }

    fclose(f);

    return 0;
}

char * config_value(CMConfig *conf, int max, char *key)
{
    int i = 0;
    for (i = 0; i < max; i++) {
        if (strcmp(conf[i].key, key) == 0) {
            return conf[i].value;
        }
    }
    return NULL;
}

int config_set_value(CMConfig *conf, int max, char *key, char *value)
{
    int i = 0;
    for (i = 0; i < max; i++) {
        if (strcmp(conf[i].key, key) == 0) {
            strncpy(conf[i].value, value, sizeof(conf[i].value));
            break;
        }
    }
    if (i == max) {
        strncpy(conf[i].key, key, sizeof(conf[i].key));
        strncpy(conf[i].value, value, sizeof(conf[i].value));
    }
    return i;
}

int config_write_buf(const char *filename, char *buf, int len)
{
    FILE *f = fopen(filename, "w");
    if (f) {
        int n = fwrite(buf, 1, len, f);
        fclose(f);
        return n;
    }
    return -1;
}

int config_read_buf(const char *filename, char *buf, int len)
{
    FILE *f = fopen(filename, "r");
    if (f) {
        int n = fread(buf, 1, len, f);
        fclose(f);
        return n;
    }
    return -1;
}
