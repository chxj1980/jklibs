/*
 *===================================================================
 *
 *          Name: rt_print.h
 *        Create: Sat 27 Oct 2012 09:49:46 AM CST
 *
 *   Discription: 
 *       Version: 1.0.0
 *
 *        Author: jmdvirus
 *         Email: jmdkina@gmail.com
 *
 *===================================================================
 */
#ifndef __RT_PRINT_H
#define __RT_PRINT_H

#ifdef  __cplusplus 
#define RT_EXTERN_C_START       extern "C" {
#define RT_EXTERN_C_END         }
#define RT_EXTERN_C_FUNC        extern "C"
#else
#define RT_EXTERN_C_START
#define RT_EXTERN_C_END 
#define RT_EXTERN_C_FUNC
#endif

#ifdef USE_ZLOG
#include "zlog/zlog.h"
#endif

/* use for rt_print_level */
enum {
    RT_PRINT_NONE = 1, RT_PRINT_ERROR = 1 << 1,
    RT_PRINT_WARN = 1 << 2, RT_PRINT_INFO = 1 << 3,
    RT_PRINT_MESSAGE = 1 << 4,
    RT_PRINT_DEBUG = 1 << 5,
    RT_PRINT_CYCLE = 1 << 6,
    RT_PRINT_ALL = RT_PRINT_ERROR|RT_PRINT_WARN|RT_PRINT_INFO|RT_PRINT_MESSAGE|RT_PRINT_DEBUG|RT_PRINT_CYCLE,
};

/* use for rt_print_style */
// you can use some of them except none and all
// none use above RT_PRINT_NONE
// time style %04y:%02m:%02d
enum {
    RT_PRINT_STYLE_ALL = 1 << 1, // [name][time][func():line@file] ==> 
    RT_PRINT_STYLE_NONE = 1 << 2,  // xxx
    RT_PRINT_NONAME = 1 << 3, // [time]...
    RT_PRINT_NOTIME = 1 << 4, // [name][func... 
    RT_PRINT_NOFUNC = 1 << 5, // [name][time][line@file] ==>
    RT_PRINT_NOLINE = 1 << 6, // [name][time][func()@file] ==>
    RT_PRINT_NOFILE = 1 << 7, // [name][time][func():line] ==>
};

enum {
    RT_PRINT_LOG_TYPE_NONE = -1,
    RT_PRINT_LOG_TYPE_CONSOLE = 0,
    RT_PRINT_LOG_TYPE_FILE = 1,
};

enum {
    RT_COLOR_FALSE = 0,
    RT_COLOR_TRUE,
};
 
RT_EXTERN_C_FUNC int rt_print_init(int rt_print_level, 
          int rt_print_style, int rt_print_enable_color, 
          const char *name);
RT_EXTERN_C_FUNC int rt_print_deinit();

// @type: 0 console, -1 none (not print), 1 to file (depends on /etc/zlog.conf)
// RT_PRINT_LOG_TYPE_*
RT_EXTERN_C_FUNC int rt_print_set_save_type(int type);
RT_EXTERN_C_FUNC int rt_print_get_log_type();
RT_EXTERN_C_FUNC int rt_print_init_file_conf(const char *filepath, const char *processName);
// Please set the config file path and the process name if 
// you want set log to file
// @conf: it is config file, and it tell program how to write log and where.
#define RTLOG_INIT(conf, name) rt_print_init_file_conf(conf, name);

RT_EXTERN_C_FUNC int rt_print_change_level(int level);
RT_EXTERN_C_FUNC int rt_print_start_debug();
RT_EXTERN_C_FUNC int rt_print_stop_debug();
RT_EXTERN_C_FUNC int rt_print_start_cycle();
RT_EXTERN_C_FUNC int rt_print_stop_cycle();

// @func: print log with diff type
// please use the micro define.
RT_EXTERN_C_FUNC int rt_print_info(const char *func, int line, const char *file,
                     const char *format, ...);
RT_EXTERN_C_FUNC int rt_print_warn(const char *func, int line, const char *file,
                     const char *format, ...);
RT_EXTERN_C_FUNC int rt_print_message(const char *func, int line, const char *file,
                     const char *format, ...);
RT_EXTERN_C_FUNC int rt_print_error(const char *func, int line, const char *file,
                     const char *format, ...);
RT_EXTERN_C_FUNC int rt_print_debug(const char *func, int line, const char *file,
                     const char *format, ...);
RT_EXTERN_C_FUNC int rt_print_cycle(const char *func, int line, const char *file,
                     const char *format, ...);
RT_EXTERN_C_FUNC int rt_print_none(const char *func, int line, const char *file,
                     const char *format, ...);
RT_EXTERN_C_FUNC void rt_print_error_string(int errno);

// You may control print level with kind,
// add kind, the rtkind will print out
// del kind, the rtkind will print nothing
RT_EXTERN_C_FUNC int rt_print_add_kind(int kind);
RT_EXTERN_C_FUNC int rt_print_del_kind(int kind);
RT_EXTERN_C_FUNC int rt_print_kind(int kind, const char *func, int line, const char *file, const char *format, ...);
#define rtkind(kind, format, ...) rt_print_kind(kind, __func__, __LINE__, __FILE__, format, ##__VA_ARGS__);

#ifdef USE_ZLOG
#define rtdzlog_error(format, ...) dzlog_error(format, ##__VA_ARGS__)
#define rtdzlog_info(format, ...) dzlog_info(format, ##__VA_ARGS__)
#define rtdzlog_warn(format, ...) dzlog_warn(format, ##__VA_ARGS__)
#define rtdzlog_debug(format, ...) dzlog_debug(format, ##__VA_ARGS__)
#define rtdzlog_notice(format, ...) dzlog_notice(format, ##__VA_ARGS__)
#else
#define rtdzlog_error(format, ...)
#define rtdzlog_info(format, ...)
#define rtdzlog_warn(format, ...)
#define rtdzlog_debug(format, ...)
#define rtdzlog_notice(format, ...)
#endif

// Caller please call these functions below

#define rterror(format, ...)    \
        switch (rt_print_get_log_type()) {    \
            case RT_PRINT_LOG_TYPE_FILE:    \
               rtdzlog_error(format, ##__VA_ARGS__);    \
            break;  \
            case RT_PRINT_LOG_TYPE_CONSOLE: rt_print_error(__func__, __LINE__, __FILE__, format, ##__VA_ARGS__); printf("\n"); break; \
            default: break;  \
        };   

#define rtinfo(format, ...)    \
        switch (rt_print_get_log_type()) {    \
            case RT_PRINT_LOG_TYPE_FILE:    \
               rtdzlog_info(format, ##__VA_ARGS__);    \
            break;  \
            case RT_PRINT_LOG_TYPE_CONSOLE: rt_print_info(__func__, __LINE__, __FILE__, format, ##__VA_ARGS__); printf("\n"); break; \
            default: break;  \
        };   

#define rtwarn(format, ...)    \
        switch (rt_print_get_log_type()) {    \
            case RT_PRINT_LOG_TYPE_FILE:    \
               rtdzlog_warn(format, ##__VA_ARGS__);    \
            break;  \
            case RT_PRINT_LOG_TYPE_CONSOLE: rt_print_warn(__func__, __LINE__, __FILE__, format, ##__VA_ARGS__); printf("\n"); break; \
            default: break;  \
        };   

#define rtmsg(format, ...)    \
        switch (rt_print_get_log_type()) {    \
            case RT_PRINT_LOG_TYPE_FILE:    \
               rtdzlog_notice(format, ##__VA_ARGS__);    \
            break;  \
            case RT_PRINT_LOG_TYPE_CONSOLE: rt_print_message(__func__, __LINE__, __FILE__, format, ##__VA_ARGS__); printf("\n"); break; \
            default: break;  \
        };   

#define rtdebug(format, ...)    \
        switch (rt_print_get_log_type()) {    \
            case RT_PRINT_LOG_TYPE_FILE:    \
               rtdzlog_debug(format, ##__VA_ARGS__);    \
            break;  \
            case RT_PRINT_LOG_TYPE_CONSOLE: rt_print_debug(__func__, __LINE__, __FILE__, format, ##__VA_ARGS__); printf("\n");break; \
            default: break;  \
        };   

// rtcycle and rtnone force print to console.
#define rtcycle(format, ...)    \
        rt_print_cycle(__func__, __LINE__, __FILE__, format, ##__VA_ARGS__); printf("\n");

#define rtnone(format, ...)    \
        rt_print_style_none(__func__, __LINE__, __FILE__, format, ##__VA_ARGS__); printf("\n");


// It will print out the meanful string with the number
// force to console
#define rterrno(no)   rt_print_error_string(no);

#define RTVERSION(fmt, args...)                                                 \
    do{	printf("\033[01;31;31m");                                              \
	printf(fmt, ##args);                                                   \
	printf(" Build time: %s, %s.\n\033[0m", __DATE__, __TIME__);           \
    }while(0)

#endif

/*=============== End of file: rt_print.h =====================*/
