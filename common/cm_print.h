/*
 *===================================================================
 *
 *          Name: cm_print.h
 *        Create: Sat 27 Oct 2012 09:49:46 AM CST
 *
 *   Discription: 
 *       Version: 1.0.0
 *
 *        Author: jmdvirus
 *
 *===================================================================
 */
#ifndef __CM_PRINT_H
#define __CM_PRINT_H

#ifdef  __cplusplus 
#define CM_EXTERN_C_START       extern "C" {
#define CM_EXTERN_C_END         }
#define CM_EXTERN_C_FUNC        extern "C"
#else
#define CM_EXTERN_C_START
#define CM_EXTERN_C_END 
#define CM_EXTERN_C_FUNC
#endif

#ifdef USE_ZLOG
#include "zlog/zlog.h"
#endif

/* use for cm_print_level */
enum {
    CM_PRINT_NONE = 1, CM_PRINT_ERROR = 1 << 1,
    CM_PRINT_WARN = 1 << 2, CM_PRINT_INFO = 1 << 3,
    CM_PRINT_MESSAGE = 1 << 4,
    CM_PRINT_DEBUG = 1 << 5,
    CM_PRINT_CYCLE = 1 << 6,
    CM_PRINT_ALL = CM_PRINT_ERROR|CM_PRINT_WARN|CM_PRINT_INFO|CM_PRINT_MESSAGE|CM_PRINT_DEBUG|CM_PRINT_CYCLE,
};

/* use for cm_print_style */
// you can use some of them except none and all
// none use above CM_PRINT_NONE
// time style %04y:%02m:%02d
enum {
    CM_PRINT_STYLE_ALL = 1 << 1, // [name][time][func():line@file] ==> 
    CM_PRINT_STYLE_NONE = 1 << 2,  // xxx
    CM_PRINT_NONAME = 1 << 3, // [time]...
    CM_PRINT_NOTIME = 1 << 4, // [name][func... 
    CM_PRINT_NOFUNC = 1 << 5, // [name][time][line@file] ==>
    CM_PRINT_NOLINE = 1 << 6, // [name][time][func()@file] ==>
    CM_PRINT_NOFILE = 1 << 7, // [name][time][func():line] ==>
};

enum {
    CM_PRINT_LOG_TYPE_NONE = -1,
    CM_PRINT_LOG_TYPE_CONSOLE = 1<<1,
    CM_PRINT_LOG_TYPE_FILE = 1,
    CM_PRINT_LOG_TYPE_OWNFILE = 1<<2,
};

enum {
    CM_COLOR_FALSE = 0,
    CM_COLOR_TRUE,
};
 
CM_EXTERN_C_FUNC int cm_print_init(int cm_print_level, 
          int cm_print_style, int cm_print_enable_color, 
          const char *name);
CM_EXTERN_C_FUNC int cm_print_deinit();

CM_EXTERN_C_FUNC int cm_print_set_log_file(const char *path);
CM_EXTERN_C_FUNC int cm_print_reopen_file();
/**
 * Set max file size
 * @param maxsize [in] must as bytes
 * @return 0
 */
CM_EXTERN_C_FUNC int cm_print_set_file_maxsize(unsigned long long maxsize);

// @type: 0 console, -1 none (not print), 1 to file (depends on /etc/zlog.conf)
// CM_PRINT_LOG_TYPE_*
CM_EXTERN_C_FUNC int cm_print_set_save_type(int type);
CM_EXTERN_C_FUNC int cm_print_get_log_type();
CM_EXTERN_C_FUNC int cm_print_init_file_conf(const char *filepath, const char *processName);
// Please set the config file path and the process name if 
// you want set log to file
// @conf: it is config file, and it tell program how to write log and where.
#define CMLOG_INIT(conf, name) cm_print_init_file_conf(conf, name);

CM_EXTERN_C_FUNC int cm_print_change_level(int level);
CM_EXTERN_C_FUNC int cm_print_start_debug();
CM_EXTERN_C_FUNC int cm_print_stop_debug();
CM_EXTERN_C_FUNC int cm_print_start_cycle();
CM_EXTERN_C_FUNC int cm_print_stop_cycle();

// @func: print log with diff type
// please use the micro define.
CM_EXTERN_C_FUNC int cm_print_info(const char *func, int line, const char *file,
                     const char *format, ...);
CM_EXTERN_C_FUNC int cm_print_warn(const char *func, int line, const char *file,
                     const char *format, ...);
CM_EXTERN_C_FUNC int cm_print_message(const char *func, int line, const char *file,
                     const char *format, ...);
CM_EXTERN_C_FUNC int cm_print_error(const char *func, int line, const char *file,
                     const char *format, ...);
CM_EXTERN_C_FUNC int cm_print_debug(const char *func, int line, const char *file,
                     const char *format, ...);
CM_EXTERN_C_FUNC int cm_print_cycle(const char *func, int line, const char *file,
                     const char *format, ...);
CM_EXTERN_C_FUNC int cm_print_none(const char *func, int line, const char *file,
                     const char *format, ...);
CM_EXTERN_C_FUNC void cm_print_error_string(int errno);

#ifdef USE_ZLOG
#define cmdzlog_error(format, ...) dzlog_error(format, ##__VA_ARGS__)
#define cmdzlog_info(format, ...) dzlog_info(format, ##__VA_ARGS__)
#define cmdzlog_warn(format, ...) dzlog_warn(format, ##__VA_ARGS__)
#define cmdzlog_debug(format, ...) dzlog_debug(format, ##__VA_ARGS__)
#define cmdzlog_notice(format, ...) dzlog_notice(format, ##__VA_ARGS__)
#else
#define cmdzlog_error(format, ...)
#define cmdzlog_info(format, ...)
#define cmdzlog_warn(format, ...)
#define cmdzlog_debug(format, ...)
#define cmdzlog_notice(format, ...)
#endif

// Caller please call these functions below

#define cmerror(format, ...)    \
        switch (cm_print_get_log_type()) {    \
            case CM_PRINT_LOG_TYPE_FILE:    \
               cmdzlog_error(format, ##__VA_ARGS__);    \
            break;  \
            case CM_PRINT_LOG_TYPE_CONSOLE:  \
            case CM_PRINT_LOG_TYPE_OWNFILE: \
            default: \
                cm_print_error(__func__, __LINE__, __FILE__, format, ##__VA_ARGS__); break; \
        };   

#define cminfo(format, ...)    \
        switch (cm_print_get_log_type()) {    \
            case CM_PRINT_LOG_TYPE_FILE:    \
               cmdzlog_info(format, ##__VA_ARGS__);    \
            break;  \
            case CM_PRINT_LOG_TYPE_CONSOLE:  \
            case CM_PRINT_LOG_TYPE_OWNFILE: \
            default: \
                cm_print_info(__func__, __LINE__, __FILE__, format, ##__VA_ARGS__); break; \
        };   

#define cmwarn(format, ...)    \
        switch (cm_print_get_log_type()) {    \
            case CM_PRINT_LOG_TYPE_FILE:    \
               cmdzlog_warn(format, ##__VA_ARGS__);    \
            break;  \
            case CM_PRINT_LOG_TYPE_CONSOLE:  \
            case CM_PRINT_LOG_TYPE_OWNFILE: \
            default: \
                cm_print_warn(__func__, __LINE__, __FILE__, format, ##__VA_ARGS__); break; \
        };   

#define cmmsg(format, ...)    \
        switch (cm_print_get_log_type()) {    \
            case CM_PRINT_LOG_TYPE_FILE:    \
               cmdzlog_notice(format, ##__VA_ARGS__);    \
            break;  \
            case CM_PRINT_LOG_TYPE_CONSOLE:  \
            case CM_PRINT_LOG_TYPE_OWNFILE: \
            default: \
                cm_print_message(__func__, __LINE__, __FILE__, format, ##__VA_ARGS__); break; \
        };   

#define cmdebug(format, ...)    \
        switch (cm_print_get_log_type()) {    \
            case CM_PRINT_LOG_TYPE_FILE:    \
               cmdzlog_debug(format, ##__VA_ARGS__);    \
            break;  \
            case CM_PRINT_LOG_TYPE_CONSOLE:  \
            case CM_PRINT_LOG_TYPE_OWNFILE: \
            default: \
                cm_print_debug(__func__, __LINE__, __FILE__, format, ##__VA_ARGS__); break; \
        };   

// rtcycle and rtnone force print to console.
#define cmcycle(format, ...)    \
        cm_print_cycle(__func__, __LINE__, __FILE__, format, ##__VA_ARGS__);

#define cmnone(format, ...)    \
        cm_print_style_none(__func__, __LINE__, __FILE__, format, ##__VA_ARGS__);


// It will print out the meanful string with the number
// force to console
#define cmerrno(no)   cm_print_error_string(no);

#define CMVERSION(fmt, args...)                                                 \
    do{	printf("\033[01;31;31m");                                              \
	printf(" Build time: %s, %s. \033[0m", __DATE__, __TIME__);           \
	printf(fmt, ##args);                                                   \
    }while(0)

#endif

/*=============== End of file: cm_print.h =====================*/
