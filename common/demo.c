/**
 * Create: 20180515
 * Author: jmdvirus
 */

#include "cm_print.h"
#include "cm_common.h"
#include "cm_interface.h"

#ifdef LOCAL_DEBUG
#define JDH_SERVER_SERVER_D      "127.0.0.1"
#define JDH_SERVER_PORT_D        8080
#else
#define JDH_SERVER_SERVER_D      "106.14.61.92"
#define JDH_SERVER_PORT_D        8081
#endif

int callback(int cmd, void *data)
{
    switch(cmd) {
        case JDH_CMD_PLAYMUSIC: {
            JDHCmdMusic *m = (JDHCmdMusic *) data;
            cmdebug("play music : [%s]\n", m->szFilePath);
        }
            break;
        default:
            break;
    }
    return 0;
}

int main(int argc, char **args) {
    CMVERSION("Program start\n");
    jdh_demo_start(8);

    JDH_Handle jdh;
    cmdebug("start init\n");
    int ret = jdh_init(&jdh, callback);
    if (ret != 0) {
        cmdebug("init failed [%d]\n", ret);
        return -1;
    }

    jdh_set_serverinfo(jdh, JDH_SERVER_SERVER_D, JDH_SERVER_PORT_D);

    cmdebug("start process\n");
    jdh_start(jdh);

    while(1) {
        usleep(50000);
    }

    jdh_deinit(&jdh);
    cmdebug("Program end\n");
    return 0;
}