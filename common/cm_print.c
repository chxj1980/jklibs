/*
 *===========================================================================
 *
 *          Name: cm_print.c
 *        Create: Sun 28 Oct 2012 11:53:44 AM CST
 *
 *   Discription: 
 *       Version: 1.0.0
 *
 *        Author: jmdvirus
 *
 *===========================================================================
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include <sys/time.h>
#include <errno.h>
#include <sys/stat.h>

#include "cm_print.h"

#define CM_COLOR_NONE	"\033[0m "
#define CM_COLOR_BLACK	"\033[0;30m "
#define CM_COLOR_DARK_GRAY		"\033[1;30m "
#define CM_COLOR_BLUE			"\033[0;34m "
#define CM_COLOR_LIGHT_BLUE	"\033[1;34m "
#define CM_COLOR_GREEN			"\033[0;32m "
#define CM_COLOR_LIGHT_GREE    "\033[1;32m "
#define CM_COLOR_CYAN			"\033[0;36m "
#define CM_COLOR_LIGHT_CYAN	"\033[1;36m "
#define CM_COLOR_RED			"\033[0;31m "
#define CM_COLOR_LIGHT_RED		"\033[1;31m "
#define CM_COLOR_PURPLE		"\033[0;35m "
#define CM_COLOR_LIGHT_PURPLE	"\033[1;35m "
#define CM_COLOR_BROWN			"\033[0;33m "
#define CM_COLOR_YELLOW		"\033[1;33m "
#define CM_COLOR_LIGHT_GRAY	"\033[0;37m "
#define CM_COLOR_WHITE			"\033[1;37m "


#define RT_NAME_LEN_SUPPORT              64
#define RT_MAX_STRING_LEN                512
#define RT_STRING_MAX_LEN_SUPPORT        3056

#define JKIFCONSOLE  \
		((cm_p->save_type & CM_PRINT_LOG_TYPE_CONSOLE) == CM_PRINT_LOG_TYPE_CONSOLE)

#define JKIFOWNFILE \
		((cm_p->save_type & CM_PRINT_LOG_TYPE_OWNFILE) == CM_PRINT_LOG_TYPE_OWNFILE)

// Remove line change when console out
#define JKLINE  \
    do { \
        if JKIFCONSOLE \
            ; \
        if JKIFOWNFILE \
	    ; \
    } while(0)

#define JKLOGPF(fd, fmt, ...) \
    fprintf(fd, fmt, ##__VA_ARGS__)

#define JKLOGP(fmt, ...) \
    JKLOGPF(stderr, fmt, ##__VA_ARGS__)

#define JKLOGPX(fmt, ...) \
    do { \
        if JKIFCONSOLE \
            JKLOGP(fmt, ##__VA_ARGS__); \
        if JKIFOWNFILE \
            if (cm_p->file)  {  \
                JKLOGPF(cm_p->file, fmt, ##__VA_ARGS__); \
		    fflush(cm_p->file); \
            } \
    } while(0)

#define JKLOGVPF(fd, fmt, ...) \
    vfprintf(fd, fmt, ##__VA_ARGS__)

#define JKLOGVP(fmt, ...) \
	JKLOGVPF(stderr, fmt, ##__VA_ARGS__)

#define JKLOGVPX(fmt, ...) \
    do { \
        if JKIFCONSOLE \
            JKLOGVP(fmt, ##__VA_ARGS__); \
        if JKIFOWNFILE \
            if (cm_p->file) { \
                JKLOGVPF(cm_p->file, fmt, ##__VA_ARGS__); \
		    fflush(cm_p->file); \
            } \
    } while(0)

#define RTNAME(name)              \
    JKLOGPX("[%s]", name)

#define RTTIMESIMPLE()                                                             \
  do{                                                                        \
    struct timeval _tNow; struct tm _tmNow;                                  \
    gettimeofday(&_tNow, NULL);                                              \
    memcpy(&_tmNow, localtime(&_tNow.tv_sec), sizeof(_tmNow));               \
    JKLOGPX("[%02d-%02d][%02d:%02d:%02d]",                         \
        _tmNow.tm_mon+1, _tmNow.tm_mday,                \
        _tmNow.tm_hour, _tmNow.tm_min, _tmNow.tm_sec);   \
  }while(0)

#define RTTIME() RTTIMESIMPLE()

#define RTTIMEMORE()                                            \
  do{                                                                        \
    struct timeval _tNow; struct tm _tmNow;                                  \
    gettimeofday(&_tNow, NULL);                                              \
    memcpy(&_tmNow, localtime(&_tNow.tv_sec), sizeof(_tmNow));               \
    JKLOGPX("[%04d-%02d-%02d][%02d:%02d:%02d.%03ld]",                         \
        _tmNow.tm_year+1900, _tmNow.tm_mon+1, _tmNow.tm_mday,                \
        _tmNow.tm_hour, _tmNow.tm_min, _tmNow.tm_sec, _tNow.tv_usec/1000);   \
  }while(0)

#define RTFUNC(func)             \
  JKLOGPX("%s()", func)        

#define RTLINE(line)             \
  JKLOGPX("%04d", line)        

#define RTFILE(file)             \
  JKLOGPX("%s", file) 

#define RTLEFT()   JKLOGPX("%s", "[")
#define RTRIGHT()  JKLOGPX("%s", "]")
#define RTCOLON()  JKLOGPX("%s", ":")
#define RTAT()     JKLOGPX("%s", "@")

#define RTALL(name, func, line, file)             \
  RTNAME(name);                 \
  RTTIME();                     \
  RTLEFT();                     \
  RTFUNC(func);                 \
  RTAT();                       \
  RTFILE(file);                 \
  RTCOLON();                    \
  RTLINE(line);                 \
  RTRIGHT()

#define RTERRORTIPS(err)     JKLOGPX("%s", err)
#define RTWARNTIPS(warn)     JKLOGPX("%s", warn)
#define RTINFOTIPS(info)     JKLOGPX("%s", info)
#define RTMESSAGETIPS(message)  JKLOGPX("%s", message)
#define RTDEBUGTIPS(debug)   JKLOGPX("%s", debug)
#define RTCYCLETIPS(cycle)   JKLOGPX("%s", cycle)
#define RTTIPS(tips)         JKLOGPX("%s", tips)

#define  RT_MAX_NAME_LEN            32

#define  RT_MAX_KIND_COUNTS         64

typedef struct __RT_Print {
    int              cm_print_level;
    int              cm_print_style;
    char             cm_name[RT_MAX_NAME_LEN+1];
    char             cm_style[RT_MAX_STRING_LEN+1];
    char            *cm_color;
    int              cm_enable_color;
    int              cm_used;
    int              save_type;  // how to save, 2 console, -1 none, 1 to file(use zlog, so depends on config of zlog.conf)
    char             filepath[RT_MAX_STRING_LEN];
    FILE            *file;
    unsigned long long         file_maxsize;
} RT_Print;

static RT_Print cm_print_out = {
    CM_PRINT_ALL,
    CM_PRINT_STYLE_ALL,
    "cm_print",
    "",
    (char*)CM_COLOR_NONE,
    CM_COLOR_FALSE,
    0,2
};

static RT_Print *cm_p = &cm_print_out;

CM_EXTERN_C_FUNC int cm_print_init(int cm_print_level, 
              int cm_print_style, int cm_print_enable_color, 
              const char *name)
{
    if (cm_p != NULL) {
        if (cm_p->cm_used == 1) {
            JKLOGPX("Warn: you have inited a print, exit...\n");
            return -3;
        }
    }

    cm_p->cm_enable_color = cm_print_enable_color;
    cm_p->cm_color = (char*)CM_COLOR_NONE;
    cm_p->cm_print_level = cm_print_level;
    cm_p->cm_print_style = cm_print_style;
    cm_p->save_type = CM_PRINT_LOG_TYPE_CONSOLE;
    if (name != NULL) {
        int len = strlen(name);
        if (len > RT_NAME_LEN_SUPPORT || len > RT_MAX_NAME_LEN) {
            JKLOGPX("WARN: long name [%d] > [%d] max support[%d], will be trancated\n", 
                   len, RT_MAX_NAME_LEN, RT_NAME_LEN_SUPPORT);
        }
        if (len <= 0) sprintf(cm_p->cm_name, "%s", "cm_print");
    } else {
        sprintf(cm_p->cm_name, "%s", "(cm_p)rint");
    }
    strncpy(cm_p->cm_name, name, sizeof(cm_p->cm_name));
    memset(cm_p->cm_style, 0, sizeof(cm_p->cm_style));

    cm_p->cm_used = 1;

    return 0;
}

CM_EXTERN_C_FUNC int cm_print_deinit()
{
    if (cm_p) { 
#ifdef USE_ZLOG
    if (cm_p->save_type == CM_PRINT_LOG_TYPE_FILE)
    zlog_fini();
#endif
    if (cm_p->file) fclose(cm_p->file);
    cm_p->cm_used = 0; 
    }
    return 0;
}

CM_EXTERN_C_FUNC int cm_print_set_log_file(const char *path)
{
    if (!cm_p) return -1;
    sprintf(cm_p->filepath, "%s", path);
    cm_p->file = fopen(path, "w");
    if (!cm_p->file) return -2;
    cm_p->save_type = CM_PRINT_LOG_TYPE_OWNFILE;

    cm_p->file_maxsize = 1024 * 1024; // 1M

    return 0;
}

CM_EXTERN_C_FUNC int cm_print_set_file_maxsize(unsigned long long maxsize)
{
    cm_p->file_maxsize = maxsize;
    return 0;
}

/**
 * Check log file size
 * @param size
 * @return 1 - log file > size
 */
int cm_print_file_size_check(unsigned long long size)
{
    if (!cm_p->file) return -1;
    struct stat st;
    int i = stat(cm_p->filepath, &st);
    if (i < 0) return -2;
    return st.st_size > size ? 1 : 0;
}

CM_EXTERN_C_FUNC int cm_print_reopen_file()
{
    if (!cm_p) return -1;
    if (cm_p->file) fclose(cm_p->file);
    cm_p->file = NULL;
    cm_print_set_log_file(cm_p->filepath);
    return 0;
}

CM_EXTERN_C_FUNC int cm_print_init_file_conf(const char *filepath, const char *processName)
{
    if (!cm_p) return -1;
#ifdef USE_ZLOG
    if (cm_p->save_type == CM_PRINT_LOG_TYPE_FILE)
    dzlog_init(filepath, processName);
#endif
    return 0;
}

// @type: 0 console, -1 none (not print), 1 to file (depends on /etc/zlog.conf)
CM_EXTERN_C_FUNC int cm_print_set_save_type(int type)
{
    if (cm_p) { cm_p->save_type = type; }
    return 0;
}

CM_EXTERN_C_FUNC int cm_print_get_log_type()
{
    if (cm_p) return cm_p->save_type;
    return -1;
}

CM_EXTERN_C_FUNC int cm_print_set_color(int level)
{
    if (cm_print_file_size_check(cm_p->file_maxsize) > 0) {
        cm_print_reopen_file();
    }

    if ((cm_p)->cm_enable_color != CM_COLOR_TRUE) return -1;

    if (level & CM_PRINT_NONE) {
        (cm_p)->cm_color = (char*)CM_COLOR_NONE;
    } else if (level & CM_PRINT_ERROR) {
        (cm_p)->cm_color = (char*)CM_COLOR_RED;
    } else if (level & CM_PRINT_WARN) {
        (cm_p)->cm_color = (char*)CM_COLOR_YELLOW;
    } else if (level & CM_PRINT_INFO) {
        (cm_p)->cm_color = (char*)CM_COLOR_WHITE;
    } else if (level & CM_PRINT_DEBUG) {
        (cm_p)->cm_color = (char*)CM_COLOR_GREEN;
    } else if (level & CM_PRINT_CYCLE) {
        (cm_p)->cm_color = (char*)CM_COLOR_LIGHT_CYAN;
    } else if (level & CM_PRINT_MESSAGE) {
        (cm_p)->cm_color = (char*)CM_COLOR_LIGHT_GRAY;
    } else {
    	(cm_p)->cm_color = (char*)CM_COLOR_CYAN;
    }

    return 0;
}


CM_EXTERN_C_FUNC int cm_is_print_level_none(int level)
{
    if (cm_p == NULL) return -1;
    if (level & CM_PRINT_NONE) return -2;
    return 0;
}

// about name time, func....
CM_EXTERN_C_FUNC int cm_print_before(int type, const char *func, int line, const char *file)
{
    if (cm_p == NULL) return -1;

    int style = (cm_p)->cm_print_style;
    int has_done = 0;

    if ((cm_p)->cm_enable_color == CM_COLOR_TRUE) {
        if ((cm_p)->cm_color != NULL)
            JKLOGPX("%s", (cm_p)->cm_color);
    }
    if (type == -1) {
        memset((cm_p)->cm_style, 0, sizeof((cm_p)->cm_style));
        goto out;
    }
    if (style != CM_PRINT_STYLE_NONE) {
        switch(type) {
            case CM_PRINT_ERROR:
                RTERRORTIPS("[ERROR]**");
            break;
            case CM_PRINT_WARN:
                RTWARNTIPS("[WARN]*");
            break;
            case CM_PRINT_INFO:
                RTINFOTIPS("[INFO]");
            break;
            case CM_PRINT_MESSAGE:
                RTMESSAGETIPS("[MSG]");
            break;
            case CM_PRINT_DEBUG:
                RTDEBUGTIPS("[DEBUG]");
            break;
            case CM_PRINT_CYCLE:
                RTCYCLETIPS("[CYCLE]");
            break;
            default:
            break;
        }
    }

    if (style & CM_PRINT_STYLE_ALL) {
        RTALL((cm_p)->cm_name, func, line, file);
    } else if (style & CM_PRINT_STYLE_NONE) {
        memset((cm_p)->cm_style, 0, sizeof((cm_p)->cm_style));
        goto out;
    } else {
        if (strlen((cm_p)->cm_name) > 0 && !(style & CM_PRINT_NONAME)) {
            RTNAME((cm_p)->cm_name);
        } 
        if (!(style & CM_PRINT_NOTIME)) {
            RTTIME();
        } 
        if (!(style & CM_PRINT_NOFUNC)) {
            RTLEFT();RTFUNC(func);
            has_done = 1;
        } 
        if (!(style & CM_PRINT_NOLINE)) {
            if (has_done == 0) RTLEFT();
            else if (has_done == 1) RTCOLON();
            RTLINE(line);
            has_done = 1;
        }
        if (!(style & CM_PRINT_NOFILE)) { 
            if (has_done == 0) RTLEFT();
            else if (has_done == 1) RTAT();
            RTFILE(file);
            has_done = 1;
        }
        if (has_done == 1) RTRIGHT();
    }

    if (style != CM_PRINT_STYLE_NONE) {
        
        RTTIPS(" ==> ");
    }

out:
    JKLOGPX("%s", cm_p->cm_style);

    return 0;
}

CM_EXTERN_C_FUNC int cm_print(int type, const char *func, int line, const char *file, const char *format, ...)
{
    if ((cm_p) == NULL) return -1;
    if ((cm_p)->cm_print_level & CM_PRINT_NONE) return -2;
    if (!((cm_p)->cm_print_level & type)) return -3;

    cm_print_set_color(type);

    va_list arg_ptr;
    va_start(arg_ptr, format);

    cm_print_before(type, func, line, file);
    JKLOGVPX(format, arg_ptr);

    va_end(arg_ptr);
    JKLINE;

    return 0;
}

CM_EXTERN_C_FUNC int cm_print_style_none(const char *func, int line, const char *file, const char *format, ...)
{
    if ((cm_p) == NULL) return -1;
    //if (!(cm_p->cm_print_level & CM_PRINT_NONE)) return -2;
    cm_print_set_color(CM_PRINT_DEBUG);

    va_list arg_ptr;
    va_start(arg_ptr, format);

    cm_print_before(-1, func, line, file);
    JKLOGVPX(format, arg_ptr);
    if ((cm_p)->cm_enable_color == CM_COLOR_TRUE) {
        JKLOGPX(CM_COLOR_NONE);
    }
    va_end(arg_ptr);
    JKLINE;

    return 0;
}

CM_EXTERN_C_FUNC int cm_print_error(const char *func, int line, const char *file, const char *format, ...)
{
    if ((cm_p) == NULL) return -1;
    if (cm_is_print_level_none((cm_p)->cm_print_level) < 0)  return -2;
    cm_print_set_color(CM_PRINT_ERROR);

    va_list arg_ptr;
    va_start(arg_ptr, format);

    cm_print_before(CM_PRINT_ERROR, func, line, file);
    JKLOGVPX(format, arg_ptr);
    if ((cm_p)->cm_enable_color == CM_COLOR_TRUE) {
        JKLOGPX(CM_COLOR_NONE);
    }

    va_end(arg_ptr);
    JKLINE;

    return 0;
}

CM_EXTERN_C_FUNC int cm_print_warn(const char *func, int line, const char *file, const char *format, ...)
{
    if ((cm_p) == NULL) return -1;
    if (cm_is_print_level_none((cm_p)->cm_print_level) < 0) return -2;
    if (!((cm_p)->cm_print_level & CM_PRINT_WARN)) return -2;
    cm_print_set_color(CM_PRINT_WARN);

    va_list arg_ptr;
    va_start(arg_ptr, format);

    cm_print_before(CM_PRINT_WARN, func, line, file);
    JKLOGVPX(format, arg_ptr);
    if ((cm_p)->cm_enable_color == CM_COLOR_TRUE) {
        JKLOGPX(CM_COLOR_NONE);
    }

    va_end(arg_ptr);
    JKLINE;

    return 0;
} 

CM_EXTERN_C_FUNC int cm_print_info(const char *func, int line, const char *file, const char *format, ...)
{
    if ((cm_p) == NULL) return -1;
    if (cm_is_print_level_none((cm_p)->cm_print_level) < 0) return -2;
    if (!((cm_p)->cm_print_level & CM_PRINT_INFO)) return -2;
    cm_print_set_color(CM_PRINT_INFO);

    va_list arg_ptr;
    va_start(arg_ptr, format);

    cm_print_before(CM_PRINT_INFO, func, line, file);
    JKLOGVPX(format, arg_ptr);
    if ((cm_p)->cm_enable_color == CM_COLOR_TRUE) {
        JKLOGPX(CM_COLOR_NONE);
    }

    va_end(arg_ptr);
    JKLINE;

    return 0;
} 

CM_EXTERN_C_FUNC int cm_print_message(const char *func, int line, const char *file, const char *format, ...)
{
    if ((cm_p) == NULL) return -1;
    if (cm_is_print_level_none((cm_p)->cm_print_level) < 0) return -2;
    //if (!(cm_p->cm_print_level & CM_PRINT_INFO)) return -2;
    cm_print_set_color(CM_PRINT_MESSAGE);

    va_list arg_ptr;
    va_start(arg_ptr, format);

    cm_print_before(CM_PRINT_MESSAGE, func, line, file);
    JKLOGVPX(format, arg_ptr);
    if ((cm_p)->cm_enable_color == CM_COLOR_TRUE) {
        JKLOGPX(CM_COLOR_NONE);
    }

    va_end(arg_ptr);
    JKLINE;

    return 0;
} 

CM_EXTERN_C_FUNC int cm_print_debug(const char *func, int line, const char *file, const char *format, ...)
{
    if ((cm_p) == NULL) return -1;
    if (cm_is_print_level_none((cm_p)->cm_print_level) < 0) return -2;
    if (!((cm_p)->cm_print_level & CM_PRINT_DEBUG)) return -2;
    cm_print_set_color(CM_PRINT_DEBUG);

    va_list arg_ptr;
    va_start(arg_ptr, format);

    cm_print_before(CM_PRINT_DEBUG, func, line, file);
    JKLOGVPX(format, arg_ptr);
    if ((cm_p)->cm_enable_color == CM_COLOR_TRUE) {
        JKLOGPX(CM_COLOR_NONE);
    }

    va_end(arg_ptr);
    JKLINE;

    return 0;
} 

CM_EXTERN_C_FUNC int cm_print_cycle(const char *func, int line, const char *file, const char *format, ...)
{
    if (cm_p == NULL) return -1;
    if (cm_is_print_level_none((cm_p)->cm_print_level) < 0) return -2;
    if (!((cm_p)->cm_print_level & CM_PRINT_CYCLE)) return -2;
    cm_print_set_color(CM_PRINT_CYCLE);

    va_list arg_ptr;
    va_start(arg_ptr, format);

    cm_print_before(CM_PRINT_CYCLE, func, line, file);
    JKLOGVPX(format, arg_ptr);
    if ((cm_p)->cm_enable_color == CM_COLOR_TRUE) {
        JKLOGPX(CM_COLOR_NONE);
    }

    va_end(arg_ptr);
    JKLINE;

    return 0;
} 

CM_EXTERN_C_FUNC int cm_print_change_level(int level)
{
    if ((cm_p) == NULL) return -1;

    return (cm_p)->cm_print_level = level;
}

CM_EXTERN_C_FUNC int cm_print_stacm_cycle()
{
    if ((cm_p) == NULL) return -1;
 
    return (cm_p)->cm_print_level |= CM_PRINT_CYCLE;
}

CM_EXTERN_C_FUNC int cm_print_stop_cycle()
{
    if ((cm_p) == NULL) return -1;

    return (cm_p)->cm_print_level &= ~CM_PRINT_CYCLE;
}
 
CM_EXTERN_C_FUNC int cm_print_stacm_debug()
{
    if ((cm_p) == NULL) return -1;

    return (cm_p)->cm_print_level |= CM_PRINT_DEBUG;
}

CM_EXTERN_C_FUNC int cm_print_stop_debug()
{
    if ((cm_p) == NULL) return -1;

    return (cm_p)->cm_print_level &= ~CM_PRINT_DEBUG;
}

CM_EXTERN_C_FUNC void cm_print_error_string(int errno)
{
    switch(errno) {
        case EIO: cmerror("EIO(%d): [%s] io operation error\n", errno, strerror(errno));
        break;
        case EINTR: cmerror("EINTR(%d): [%s] interrupt\n", errno, strerror(errno));
        break;
        case EAGAIN: cmerror("EAGAIN(%d): [%s] deal again later\n", errno, strerror(errno));
        break;
        case EFAULT: cmerror("EFAULT(%d): [%s] pointer param error\n", errno, strerror(errno));
        break;
        case EBADF: cmerror("EBADF(%d): [%s] bad file descriptor\n", errno, strerror(errno));
        break;
        case ENOMEM: cmerror("ENOMEM(%d): [%s] no memory\n", errno, strerror(errno));
        break;
        case EINVAL: cmerror("EINVAL(%d): [%s] invalid param \n", errno, strerror(errno));
        break;
        default:
        cmerror("(%d):[%s]", errno, strerror(errno));
        break;
    }
}

/*=============== End of file: cm_print.c ==========================*/

