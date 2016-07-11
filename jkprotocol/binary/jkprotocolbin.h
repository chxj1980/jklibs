//
// Created by v on 16-1-4.
// Author: jmdvirus@roamter.com
//


#ifndef KFCONFIG_JKPROTOCOLBIN_H
#define KFCONFIG_JKPROTOCOLBIN_H

enum {
    JK_PRO_BIN_CMD_QUERY = 1,
    JK_PRO_BIN_CMD_CONTROL,
    JK_PRO_BIN_CMD_NOTIFY,
};

enum {
    JK_PRO_BIN_SUBCMD_REGISTER = 0X01,
    JK_PRO_BIN_SUBCMD_KEEPALIVE,

    JK_PRO_BIN_SUBCMD_COMMAND = 0x100, // exec command
    JK_PRO_BIN_SUBCMD_FILE, // get file data

    JK_PRO_BIN_SUBCMD_OFFLINE = 0x500,
};

typedef struct tagJKProtocolBin *JKProtocolBin;

int jk_protocol_bin_init(JKProtocolBin *pbin);

int jk_protocol_bin_deinit(JKProtocolBin *pbin);

int jk_protocol_bin_get_curseq(JKProtocolBin pbin);

int jk_protocol_bin_set_args(JKProtocolBin  pbin, int cmd, int subcmd, char *id);

// We don't copy data, so please keep data exist until you don't need it with the interface.
int jk_protocol_bin_set_data(JKProtocolBin pbin, int len, char *data);

// Set response code
// ret : 0 success, others fail
// retcode: success/fail code max 1<<7-1
int jk_protocol_bin_set_response(JKProtocolBin pbin, int ret, int retcode);

// Finally, you must call this function to genearate data and will save to @save.
// We don't malloc @save, you must be sure it is enough space to save data.
// About > 40 bytes + len data
int jk_protocol_bin_generate_data(JKProtocolBin pbin, char *save, int *len);


int jk_protocol_bin_parse_data(JKProtocolBin pbin, char *data, int len);

// Don't forget free the data received.
// only use when parse data success.
int jk_protocol_bin_parse_data_free(JKProtocolBin pbin);

int jk_protocol_bin_get_version(JKProtocolBin pbin);

char* jk_protocol_bin_get_id(JKProtocolBin pbin);

int jk_protocol_bin_get_sequence(JKProtocolBin pbin);

int jk_protocol_bin_get_cmd(JKProtocolBin pbin);

int jk_protocol_bin_get_subcmd(JKProtocolBin pbin);

int jk_protocol_bin_get_direction(JKProtocolBin pbin);

int jk_protocol_bin_get_ret(JKProtocolBin pbin);

int jk_protocol_bin_get_retcode(JKProtocolBin pbin);

char* jk_protocol_bin_get_data(JKProtocolBin pbin);

int jk_protocol_bin_get_data_length(JKProtocolBin pbin);

#endif //KFCONFIG_JKPROTOCOLBIN_H
