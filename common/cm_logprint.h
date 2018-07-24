/*********************************************************
 *
 * Filename: common/cm_logprint.h
 *   Author: jmdvirus
 *   Create: 2018年07月24日 星期二 10时03分53秒
 *
 *********************************************************/
#ifndef CM_LOGPRINT_H
#define CM_LOGPRINT_H

#include "cm_print.h"

#ifdef __cplusplus
extern "C" {
#endif

extern CMLogPrint logPrint;

#define CM_LOG_PRINT_INIT(name)  \
    cm_print_init_simple(&logPrint, name);

#define CM_LOG_PRINT_DEINIT() \
    cm_print_deinit(&logPrint);

#define CM_LOG_SET_LOG_FILE(filename) \
    cm_print_set_log_file(&logPrint, filename);

#define cminfo(fmt, ...) cm_print_info(&logPrint, __func__, __LINE__, __FILE__, fmt, ##__VA_ARGS__);

#define cmerror(fmt, ...) cm_print_error(&logPrint, __func__, __LINE__, __FILE__, fmt, ##__VA_ARGS__);

#define cmwarn(fmt, ...) cm_print_warn(&logPrint, __func__, __LINE__, __FILE__, fmt, ##__VA_ARGS__);

#define cmdebug(fmt, ...) cm_print_debug(&logPrint, __func__, __LINE__, __FILE__, fmt, ##__VA_ARGS__);

#ifdef __cplusplus
}
#endif

#endif

