//
// Created by v on 16-3-3.
// Author: jmdvirus@roamter.com
//

#include <stdio.h>
#include "libconfig.h"


int demo_read_write_file(const char *filename) {
    config_t cfg;
    config_setting_t *setting;

    config_init(&cfg);

    int ret = config_read_file(&cfg, filename);
    if (!ret) {
        printf("config read from file failed: %d\n", ret);
        config_destroy(&cfg);
        return -1;
    }

    char *loglevel;
    ret = config_lookup_string(&cfg, "global.loglevel", &loglevel);
    if (ret < 0) {
        printf("error kookup\n");
    } else {
        printf("loglevel: %s\n", loglevel);
    }

    setting = config_lookup(&cfg, "global");
    if (setting != NULL) {
        char *loglevel;
        ret = config_setting_lookup_string(setting, "loglevel", &loglevel);
        if (ret < 0) {
            printf("get error %d\n", ret);
        } else {
            printf("Get log level %s\n", loglevel);
        }
    } else {
        printf("No this option\n");
    }


    config_destroy(&cfg);

    return 0;
}

int main(int argc, char **args) {
    demo_read_write_file("/tmp/demo.cfg");
    printf("Just for test. ");
    return 0;
}
