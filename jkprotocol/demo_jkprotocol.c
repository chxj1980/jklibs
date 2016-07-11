/*
 *===========================================================================
 *
 *          Name: demo_jkprotocol.c
 *        Create: 2015年10月20日 星期二 16时32分56秒
 *
 *   Discription: 
 *
 *        Author: yuwei.zhang
 *         Email: yuwei.zhang@besovideo.com
 *
 *===========================================================================
 */

#include <stdio.h>
#include <string.h>

#include "jkprotocol.h"
#include "rt_print.h"

int test_protocol_register()
{
    rtdebug("Protocol register TEST start.");
    JKProtocol p = NULL;
    int ret = jk_protocol_init(&p, "1234");
    if (ret < 0) {
        rterror("protocol init fail. %d.", ret);
        return ret;
    }
    ret = jk_protocol_generate_register(p);
    if (ret < 0) {
        rterror("protocol generate register fail %d.", ret);
        return ret;
    }
    char data[2048] = {0};
    ret = jk_protocol_generate_string(p, "", data);
    if (ret < 0) {
        rterror("protocol generate string fail %d.", ret);
        return ret;
    }
    rtinfo("Generate out string: [%s]", data);
    jk_protocol_deinit(&p);
    rtdebug("Protocol register TEST end. ");
    return 0;
}

int test_protocol_control_savefile()
{
    rtdebug("Protocol control savefile TEST start.");
    JKProtocol p = NULL;
    int ret = jk_protocol_init(&p, "12345");
    if (ret < 0) {
        rterror("protocol init fail. %d.", ret);
        return ret;
    }
    ret = jk_protocol_generate_control_savefile(p, "/tmp/tfile.log");
    if (ret < 0) {
        rterror("protocol generate register fail %d.", ret);
        return ret;
    }
    char data[2048] = {0};
    ret = jk_protocol_generate_string(p, "This is test", data);
    if (ret < 0) {
        rterror("protocol generate string fail %d.", ret);
        return ret;
    }
    rtinfo("Generate out string: [%s]", data);
    jk_protocol_deinit(&p);
    rtdebug("Protocol control savefile TEST end. ");
    return 0;
}

int test_protocol_notify_savefile()
{
    rtdebug("Protocol notify savefile TEST start.");
    JKProtocol p = NULL;
    int ret = jk_protocol_init(&p, "12346");
    if (ret < 0) {
        rterror("protocol init fail. %d.", ret);
        return ret;
    }
    ret = jk_protocol_generate_notify_savefile(p, "/tmp/tfile.log");
    if (ret < 0) {
        rterror("protocol generate register fail %d.", ret);
        return ret;
    }
    char data[2048] = {0};
    ret = jk_protocol_generate_string(p, "This is file content test!", data);
    if (ret < 0) {
        rterror("protocol generate string fail %d.", ret);
        return ret;
    }
    rtinfo("Generate out string: [%s]", data);
    jk_protocol_deinit(&p);
    rtdebug("Protocol notify savefile TEST end. ");
    return 0;
}

int test_protocol_parse(const char *data, int len)
{
    rtdebug("Protocol parse TEST start.");

    JKProtocol p = NULL;
    int ret = jk_protocol_init(&p, "1111");
    if (ret < 0) {
        rterror("protocol init fail. %d", ret);
        return -1;
    }
    ret = jk_protocol_parse(p, data, len);
    if (ret < 0) {
        rterror("protocol parse fail %d", ret);
        return -2;
    }

    int cmd = jk_protocol_cmd(p);
    rtdebug("cmd: %d", cmd);
    int subcmd = jk_protocol_subcmd(p);
    rtdebug("subcmd: %d", subcmd);

    const char *outdata = jk_protocol_data(p);
    if (outdata) {
        rtdebug("data: %s", outdata);
    }

    rtdebug("Protocol parse TEST end.");
    return 0;
}

int main()
{
    //test_protocol_register();

    test_protocol_control_savefile();
    //test_protocol_notify_savefile();

    //const char *data = "0.0.1\r\n12346\r\n2\r\n1\r\n1445335976\r\n0e7f48a4188a2d984ce7d5961217234a\r\n"
     //                  "/tmp/tfile.log\r\nThis is file content test!";
    //int len = strlen(data);
    //test_protocol_parse(data, len);


    return 0;
}

/*=============== End of file: demo_jkprotocol.c ==========================*/
