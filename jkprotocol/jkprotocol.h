/*
 *===================================================================
 *
 *          Name: jkprotocol.h
 *        Create: 2015年10月20日 星期二 14时16分22秒
 *
 *   Discription: 
 *       Version: 1.0.0
 *
 *        Author: yuwei.zhang@besovideo.com
 *
 *===================================================================
 */
#ifndef __JK_PROTOCOL_H
#define __JK_PROTOCOL_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct tagJKProtocol *JKProtocol;

enum {
    JK_PROTOCOL_CMD_REGISTER = 1,
    JK_PROTOCOL_CMD_NOTIFY = 2,
    JK_PROTOCOL_CMD_CONTROL = 4,
};

enum {
    JK_PROTOCOL_SUBCMD_SAVEFILE = 1,
};

/*
 * @func: init a JKProtocol
 * @id: It must unique, 
 */
int jk_protocol_init(JKProtocol *pro, const char *id);

/*
 * @func: Deinit everything.
 */
int jk_protocol_deinit(JKProtocol *pro);

/*
 * @func: Generate register command.
 */
int jk_protocol_generate_register(JKProtocol pro);

/*
 * @func: Generate Savefile notify command.
 */
int jk_protocol_generate_notify_savefile(JKProtocol pro, const char *filename);

/*
 * @func: Generate Control Savefile command.
 */
int jk_protocol_generate_control_savefile(JKProtocol pro, const char *filename);

/*
 * @func: Generate string with @data, save to @save
 * @save: Must enough to contain them.
 */
int jk_protocol_generate_string(JKProtocol pro, const char *data, char *save);

/*
 * @func: Parse @data to JKProtocol.
 */
int jk_protocol_parse(JKProtocol pro, const char *data, int len);

/*
 * @func: return cmd.
 */
int jk_protocol_cmd(JKProtocol p);

/*
 * @func: return subcmd.
 */
int jk_protocol_subcmd(JKProtocol p);

/*
 * @func: return data.
 * Data need freed if needn't it.
 */
char *jk_protocol_data(JKProtocol p);

/*
 * @func: Free data when needn't it.
 */
int jk_protocol_data_free(JKProtocol p);

#ifdef __cplusplus
}
#endif

#endif // __JK_PROTOCOL_H
/*=============== End of file: jkprotocol.h =====================*/
