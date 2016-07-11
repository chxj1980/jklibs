//
// Created by v on 16-1-5.
// Author: jmdvirus@roamter.com
//

#include <stdio.h>
#include <string.h>
#include "jkprotocolbin.h"
#include "rt_print.h"

static JKProtocolBin pbin_gen;
static JKProtocolBin pbin_parse;

int test_protocol_bin_generate(JKProtocolBin pbin, char *save, int *len)
{
    if (!pbin) return -1;

    rtinfo("set args ...");
    jk_protocol_bin_set_args(pbin, JK_PRO_BIN_CMD_NOTIFY, JK_PRO_BIN_SUBCMD_KEEPALIVE, "12b4de12ae8f");
    char theData[1024] = "This si for test result.";
    int lend = strlen(theData);
    rtinfo("set data ...");
    jk_protocol_bin_set_data(pbin, lend, theData);

    rtinfo("generate data ....");
    int ret = jk_protocol_bin_generate_data(pbin, save, len);
    if (ret < 0) {
        rterror("Generate data failed. %d", ret);
        return -2;
    }

    return 0;
}

int test_protocol_bin_parse(JKProtocolBin pbin, char *data, int len)
{
    if (!pbin) return -1;

    rtinfo("parse data ...")
    int ret = jk_protocol_bin_parse_data(pbin, data, len);
    if (ret < 0) {
        rterror("parse data failed %d", ret);
        return -2;
    }

    rtinfo("result: version %d, cmd %d, subcmd %d, direction %d, id %s,"
                   " sequence %d, length: %d, data: %s", jk_protocol_bin_get_version(pbin),
           jk_protocol_bin_get_cmd(pbin), jk_protocol_bin_get_subcmd(pbin),
           jk_protocol_bin_get_direction(pbin), jk_protocol_bin_get_id(pbin),
           jk_protocol_bin_get_sequence(pbin), jk_protocol_bin_get_data_length(pbin),
           jk_protocol_bin_get_data(pbin));

    return 0;
}

int main() {
    // demo of jkprotocolbin
    rtinfo("start test protocol bin generate.");
    int ret = jk_protocol_bin_init(&pbin_gen);
    if (ret < 0) {
        rterror("fail init protocol %d", ret);
        return -1;
    }

    char saveData[1024] = {0};
    int len = 1024;
    test_protocol_bin_generate(pbin_gen, saveData, &len);
    rtinfo("Get data len %d", len);
    rtinfo("THe original data is : %s", saveData+120);

    jk_protocol_bin_deinit(&pbin_gen);
    rtinfo("test protobol bin generate end...")

    rtinfo("test protocol bin parse start ...")

    int ret1 = jk_protocol_bin_init(&pbin_parse);
    if (ret1 < 0) {
        rterror("fail init protocol parse %d", ret);
        return -2;
    }

    test_protocol_bin_parse(pbin_parse, saveData, len);

    jk_protocol_bin_deinit(&pbin_parse);

    rtinfo("test protocol bin parse end ...")
}
