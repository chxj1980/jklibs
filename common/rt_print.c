/*
 *===========================================================================
 *
 *          Name: rt_print.c
 *        Create: Sun 28 Oct 2012 11:53:44 AM CST
 *
 *   Discription: 
 *       Version: 1.0.0
 *
 *        Author: yuwei.zhang
 *         Email: yuwei.zhang@besovideo.com
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

#include "rt_print.h"

#define RT_COLOR_NONE	"\033[0m "
#define RT_COLOR_BLACK	"\033[0;30m "
#define RT_COLOR_DARK_GRAY		"\033[1;30m "
#define RT_COLOR_BLUE			"\033[0;34m "
#define RT_COLOR_LIGHT_BLUE	"\033[1;34m "
#define RT_COLOR_GREEN			"\033[0;32m "
#define RT_COLOR_LIGHT_GREE    "\033[1;32m "
#define RT_COLOR_CYAN			"\033[0;36m "
#define RT_COLOR_LIGHT_CYAN	"\033[1;36m "
#define RT_COLOR_RED			"\033[0;31m "
#define RT_COLOR_LIGHT_RED		"\033[1;31m "
#define RT_COLOR_PURPLE		"\033[0;35m "
#define RT_COLOR_LIGHT_PURPLE	"\033[1;35m "
#define RT_COLOR_BROWN			"\033[0;33m "
#define RT_COLOR_YELLOW		"\033[1;33m "
#define RT_COLOR_LIGHT_GRAY	"\033[0;37m "
#define RT_COLOR_WHITE			"\033[1;37m "


#define RT_NAME_LEN_SUPPORT              64
#define RT_MAX_STRING_LEN                512
#define RT_STRING_MAX_LEN_SUPPORT        3056

#define RTNAME(name)              \
    printf("[%s]", name)

#define RTTIMESIMPLE()                                                             \
  do{                                                                        \
    struct timeval _tNow; struct tm _tmNow;                                  \
    gettimeofday(&_tNow, NULL);                                              \
    memcpy(&_tmNow, localtime(&_tNow.tv_sec), sizeof(_tmNow));               \
    printf("[%02d-%02d][%02d:%02d:%02d]",                         \
        _tmNow.tm_mon+1, _tmNow.tm_mday,                \
        _tmNow.tm_hour, _tmNow.tm_min, _tmNow.tm_sec);   \
  }while(0)

#define RTTIME() RTTIMESIMPLE()

#define RTTIMEMORE()                                            \
  do{                                                                        \
    struct timeval _tNow; struct tm _tmNow;                                  \
    gettimeofday(&_tNow, NULL);                                              \
    memcpy(&_tmNow, localtime(&_tNow.tv_sec), sizeof(_tmNow));               \
    printf("[%04d-%02d-%02d][%02d:%02d:%02d.%03ld]",                         \
        _tmNow.tm_year+1900, _tmNow.tm_mon+1, _tmNow.tm_mday,                \
        _tmNow.tm_hour, _tmNow.tm_min, _tmNow.tm_sec, _tNow.tv_usec/1000);   \
  }while(0)

#define RTFUNC(func)             \
  printf("%s()", func)        

#define RTLINE(line)             \
  printf("%04d", line)        

#define RTFILE(file)             \
  printf("%s", file) 

#define RTLEFT()   printf("[")
#define RTRIGHT()  printf("]")
#define RTCOLON()  printf(" : ")
#define RTAT()     printf(" @ ")

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

#define RTERRORTIPS(err)     printf("%s", err)
#define RTWARNTIPS(warn)     printf("%s", warn)
#define RTINFOTIPS(info)     printf("%s", info)
#define RTMESSAGETIPS(message)  printf("%s", message)
#define RTDEBUGTIPS(debug)   printf("%s", debug)
#define RTCYCLETIPS(cycle)   printf("%s", cycle)
#define RTTIPS(tips)         printf("%s", tips)

#define  RT_MAX_NAME_LEN            32

#define  RT_MAX_KIND_COUNTS         64

typedef struct __RT_Print {
    int              rt_print_level;
    int              rt_print_style;
    char             rt_name[RT_MAX_NAME_LEN+1];
    char             rt_style[RT_MAX_STRING_LEN+1];
    char            *rt_color;
    int              rt_enable_color;
    int              rt_used;
    int              rt_kind_cur;
    int              rt_kind[RT_MAX_KIND_COUNTS];
    FILE            *file;
    int              save_type;  // how to save, 0 console, -1 none, 1 to file(use zlog, so depends on config of zlog.conf)
} RT_Print;

static RT_Print rt_print_out = {
    RT_PRINT_ALL,
    RT_PRINT_STYLE_ALL,
    "rt_print",
    "",
    (char*)RT_COLOR_NONE,
    RT_COLOR_FALSE,
    0,0
};

static RT_Print *rt_p = &rt_print_out;

RT_EXTERN_C_FUNC int rt_print_init(int rt_print_level, 
              int rt_print_style, int rt_print_enable_color, 
              const char *name)
{
    if (rt_p != NULL) {
        if (rt_p->rt_used == 1) {
            printf("Warn: you have inited a print, exit...\n");
            return -3;
        }
    }

    rt_p->rt_enable_color = rt_print_enable_color;
    rt_p->rt_color = (char*)RT_COLOR_NONE;
    rt_p->rt_print_level = rt_print_level;
    rt_p->rt_print_style = rt_print_style;
    rt_p->save_type = RT_PRINT_LOG_TYPE_CONSOLE;
    if (name != NULL) {
        int len = strlen(name);
        if (len > RT_NAME_LEN_SUPPORT || len > RT_MAX_NAME_LEN) {
            printf("WARN: long name [%d] > [%d] max support[%d], will be trancated\n", 
                   len, RT_MAX_NAME_LEN, RT_NAME_LEN_SUPPORT);
        }
        if (len <= 0) sprintf(rt_p->rt_name, "%s", "rt_print");
    } else {
        sprintf(rt_p->rt_name, "%s", "(rt_p)rint");
    }
    strncpy(rt_p->rt_name, name, sizeof(rt_p->rt_name));
    memset(rt_p->rt_style, 0, sizeof(rt_p->rt_style));

    rt_p->rt_used = 1;
    int i;
    for (i = 0; i < RT_MAX_KIND_COUNTS; i++) {
        rt_p->rt_kind[i] = 0;
    }

    return 0;
}

RT_EXTERN_C_FUNC int rt_print_deinit()
{
    if (rt_p) { 
#ifdef USE_ZLOG
    if (rt_p->save_type == RT_PRINT_LOG_TYPE_FILE)
    zlog_fini();
#endif
    rt_p->rt_used = 0; 
    }
    return 0;
}

RT_EXTERN_C_FUNC int rt_print_init_file_conf(const char *filepath, const char *processName)
{
    if (!rt_p) return -1;
#ifdef USE_ZLOG
    if (rt_p->save_type == RT_PRINT_LOG_TYPE_FILE)
    dzlog_init(filepath, processName);
#endif
    return 0;
}

// @type: 0 console, -1 none (not print), 1 to file (depends on /etc/zlog.conf)
RT_EXTERN_C_FUNC int rt_print_set_save_type(int type)
{
    if (rt_p) { rt_p->save_type = type; }
    return 0;
}

RT_EXTERN_C_FUNC int rt_print_get_log_type()
{
    if (rt_p) return rt_p->save_type;
    return -1;
}

RT_EXTERN_C_FUNC int rt_print_set_color(int level)
{
    if ((rt_p)->rt_enable_color != RT_COLOR_TRUE) return -1;

    if (level & RT_PRINT_NONE) {
        (rt_p)->rt_color = (char*)RT_COLOR_NONE;
    } else if (level & RT_PRINT_ERROR) {
        (rt_p)->rt_color = (char*)RT_COLOR_RED;
    } else if (level & RT_PRINT_WARN) {
        (rt_p)->rt_color = (char*)RT_COLOR_YELLOW;
    } else if (level & RT_PRINT_INFO) {
        (rt_p)->rt_color = (char*)RT_COLOR_WHITE;
    } else if (level & RT_PRINT_DEBUG) {
        (rt_p)->rt_color = (char*)RT_COLOR_GREEN;
    } else if (level & RT_PRINT_CYCLE) {
        (rt_p)->rt_color = (char*)RT_COLOR_LIGHT_CYAN;
    } else if (level & RT_PRINT_MESSAGE) {
        (rt_p)->rt_color = (char*)RT_COLOR_LIGHT_GRAY;
    } else {
    	(rt_p)->rt_color = (char*)RT_COLOR_CYAN;
    }

    return 0;
}


RT_EXTERN_C_FUNC int rt_is_print_level_none(int level)
{
    if (rt_p == NULL) return -1;
    if (level & RT_PRINT_NONE) return -2;
    return 0;
}

// about name time, func....
RT_EXTERN_C_FUNC int rt_print_before(int type, const char *func, int line, const char *file)
{
    if (rt_p == NULL) return -1;
    char    pout[1024];

    int style = (rt_p)->rt_print_style;
    int has_done = 0;

    if ((rt_p)->rt_enable_color == RT_COLOR_TRUE) {
        if ((rt_p)->rt_color != NULL)
            printf("%s", (rt_p)->rt_color);
    }
    if (type == -1) {
        memset((rt_p)->rt_style, 0, sizeof((rt_p)->rt_style));
        goto out;
    }

    if (style & RT_PRINT_STYLE_ALL) {
        RTALL((rt_p)->rt_name, func, line, file);
    } else if (style & RT_PRINT_STYLE_NONE) {
        memset((rt_p)->rt_style, 0, sizeof((rt_p)->rt_style));
        goto out;
    } else {
        if (strlen((rt_p)->rt_name) > 0 && !(style & RT_PRINT_NONAME)) {
            RTNAME((rt_p)->rt_name);
        } 
        if (!(style & RT_PRINT_NOTIME)) {
            RTTIME();
        } 
        if (!(style & RT_PRINT_NOFUNC)) {
            RTLEFT();RTFUNC(func);
            has_done = 1;
        } 
        if (!(style & RT_PRINT_NOLINE)) {
            if (has_done == 0) RTLEFT();
            else if (has_done == 1) RTCOLON();
            RTLINE(line);
            has_done = 1;
        }
        if (!(style & RT_PRINT_NOFILE)) { 
            if (has_done == 0) RTLEFT();
            else if (has_done == 1) RTAT();
            RTFILE(file);
            has_done = 1;
        }
        if (has_done == 1) RTRIGHT();
    }

    if (style != RT_PRINT_STYLE_NONE) {
        switch(type) {
            case RT_PRINT_ERROR:
                RTERRORTIPS("[ERROR]**");
            break;
            case RT_PRINT_WARN:
                RTWARNTIPS("[WARN]*");
            break;
            case RT_PRINT_INFO:
                RTINFOTIPS("[INFO]");
            break;
            case RT_PRINT_MESSAGE:
                RTMESSAGETIPS("[MSG]");
            break;
            case RT_PRINT_DEBUG:
                RTDEBUGTIPS("[DEBUG]");
            break;
            case RT_PRINT_CYCLE:
                RTCYCLETIPS("[CYCLE]");
            break;
            default:
            break;
        }
        RTTIPS(" ==> ");
    }

out:
    sprintf(pout, "%s", (rt_p)->rt_style);
    printf("%s", pout);

    return 0;
}

RT_EXTERN_C_FUNC int rt_print(int type, const char *func, int line, const char *file, const char *format, ...)
{
    if ((rt_p) == NULL) return -1;
    if ((rt_p)->rt_print_level & RT_PRINT_NONE) return -2;
    if (!((rt_p)->rt_print_level & type)) return -3;

    rt_print_set_color(type);

    char  out[RT_STRING_MAX_LEN_SUPPORT];
    memset(out, 0, sizeof(out));

    va_list arg_ptr;
    va_start(arg_ptr, format);
    vsnprintf(out, sizeof(out), format, arg_ptr);

    rt_print_before(type, func, line, file);
    printf("%s", out);

    va_end(arg_ptr);

    return 0;
}

RT_EXTERN_C_FUNC int rt_print_style_none(const char *func, int line, const char *file, const char *format, ...)
{
    if ((rt_p) == NULL) return -1;
    //if (!(rt_p->rt_print_level & RT_PRINT_NONE)) return -2;
    rt_print_set_color(RT_PRINT_DEBUG);

    char  out[RT_STRING_MAX_LEN_SUPPORT];
    memset(out, 0, sizeof(out));
    va_list arg_ptr;
    va_start(arg_ptr, format);
    vsnprintf(out, sizeof(out), format, arg_ptr);

    rt_print_before(-1, func, line, file);
    printf("%s", out);
    if ((rt_p)->rt_enable_color == RT_COLOR_TRUE) {
        printf(RT_COLOR_NONE);
    }
    va_end(arg_ptr);

    return 0;
}

RT_EXTERN_C_FUNC int rt_print_error(const char *func, int line, const char *file, const char *format, ...)
{
    if ((rt_p) == NULL) return -1;
    if (rt_is_print_level_none((rt_p)->rt_print_level) < 0)  return -2;
    rt_print_set_color(RT_PRINT_ERROR);

    char  out[RT_STRING_MAX_LEN_SUPPORT];
    memset(out, 0, sizeof(out));

    va_list arg_ptr;
    va_start(arg_ptr, format);
    vsnprintf(out, sizeof(out), format, arg_ptr);

    rt_print_before(RT_PRINT_ERROR, func, line, file);
    printf("%s", out);
    if ((rt_p)->rt_enable_color == RT_COLOR_TRUE) {
        printf(RT_COLOR_NONE);
    }

    va_end(arg_ptr);

    return 0;
}

RT_EXTERN_C_FUNC int rt_print_warn(const char *func, int line, const char *file, const char *format, ...)
{
    if ((rt_p) == NULL) return -1;
    if (rt_is_print_level_none((rt_p)->rt_print_level) < 0) return -2;
    if (!((rt_p)->rt_print_level & RT_PRINT_WARN)) return -2;
    rt_print_set_color(RT_PRINT_WARN);

    char  out[RT_STRING_MAX_LEN_SUPPORT];
    memset(out, 0, sizeof(out));

    va_list arg_ptr;
    va_start(arg_ptr, format);
    vsnprintf(out, sizeof(out), format, arg_ptr);

    rt_print_before(RT_PRINT_WARN, func, line, file);
    printf("%s", out);
    if ((rt_p)->rt_enable_color == RT_COLOR_TRUE) {
        printf(RT_COLOR_NONE);
    }

    va_end(arg_ptr);

    return 0;
} 

RT_EXTERN_C_FUNC int rt_print_info(const char *func, int line, const char *file, const char *format, ...)
{
    if ((rt_p) == NULL) return -1;
    if (rt_is_print_level_none((rt_p)->rt_print_level) < 0) return -2;
    if (!((rt_p)->rt_print_level & RT_PRINT_INFO)) return -2;
    rt_print_set_color(RT_PRINT_INFO);

    char  out[RT_STRING_MAX_LEN_SUPPORT];
    memset(out, 0, sizeof(out));

    va_list arg_ptr;
    va_start(arg_ptr, format);
    vsnprintf(out, sizeof(out),format, arg_ptr);

    rt_print_before(RT_PRINT_INFO, func, line, file);
    printf("%s", out);
    if ((rt_p)->rt_enable_color == RT_COLOR_TRUE) {
        printf(RT_COLOR_NONE);
    }

    va_end(arg_ptr);

    return 0;
} 

RT_EXTERN_C_FUNC int rt_print_message(const char *func, int line, const char *file, const char *format, ...)
{
    if ((rt_p) == NULL) return -1;
    if (rt_is_print_level_none((rt_p)->rt_print_level) < 0) return -2;
    //if (!(rt_p->rt_print_level & RT_PRINT_INFO)) return -2;
    rt_print_set_color(RT_PRINT_MESSAGE);

    char  out[RT_STRING_MAX_LEN_SUPPORT];
    memset(out, 0, sizeof(out));

    va_list arg_ptr;
    va_start(arg_ptr, format);
    vsnprintf(out, sizeof(out), format, arg_ptr);

    rt_print_before(RT_PRINT_MESSAGE, func, line, file);
    printf("%s", out);
    if ((rt_p)->rt_enable_color == RT_COLOR_TRUE) {
        printf(RT_COLOR_NONE);
    }

    va_end(arg_ptr);

    return 0;
} 

RT_EXTERN_C_FUNC int rt_print_debug(const char *func, int line, const char *file, const char *format, ...)
{
    if ((rt_p) == NULL) return -1;
    if (rt_is_print_level_none((rt_p)->rt_print_level) < 0) return -2;
    if (!((rt_p)->rt_print_level & RT_PRINT_DEBUG)) return -2;
    rt_print_set_color(RT_PRINT_DEBUG);

    char  out[RT_STRING_MAX_LEN_SUPPORT];
    memset(out, 0, sizeof(out));

    va_list arg_ptr;
    va_start(arg_ptr, format);
    vsnprintf(out, sizeof(out), format, arg_ptr);

    rt_print_before(RT_PRINT_DEBUG, func, line, file);
    printf("%s", out);
    if ((rt_p)->rt_enable_color == RT_COLOR_TRUE) {
        printf(RT_COLOR_NONE);
    }

    va_end(arg_ptr);

    return 0;
} 

RT_EXTERN_C_FUNC int rt_print_cycle(const char *func, int line, const char *file, const char *format, ...)
{
    if (rt_p == NULL) return -1;
    if (rt_is_print_level_none((rt_p)->rt_print_level) < 0) return -2;
    if (!((rt_p)->rt_print_level & RT_PRINT_CYCLE)) return -2;
    rt_print_set_color(RT_PRINT_CYCLE);

    char  out[RT_STRING_MAX_LEN_SUPPORT];
    memset(out, 0, sizeof(out));

    va_list arg_ptr;
    va_start(arg_ptr, format);
    vsnprintf(out, sizeof(out), format, arg_ptr);

    rt_print_before(RT_PRINT_CYCLE, func, line, file);
    printf("%s", out);
    if ((rt_p)->rt_enable_color == RT_COLOR_TRUE) {
        printf(RT_COLOR_NONE);
    }

    va_end(arg_ptr);

    return 0;
} 

RT_EXTERN_C_FUNC int rt_print_change_level(int level)
{
    if ((rt_p) == NULL) return -1;

    return (rt_p)->rt_print_level = level;
}

RT_EXTERN_C_FUNC int rt_print_start_cycle()
{
    if ((rt_p) == NULL) return -1;
 
    return (rt_p)->rt_print_level |= RT_PRINT_CYCLE;
}

RT_EXTERN_C_FUNC int rt_print_stop_cycle()
{
    if ((rt_p) == NULL) return -1;

    return (rt_p)->rt_print_level &= ~RT_PRINT_CYCLE;
}
 
RT_EXTERN_C_FUNC int rt_print_start_debug()
{
    if ((rt_p) == NULL) return -1;

    return (rt_p)->rt_print_level |= RT_PRINT_DEBUG;
}

RT_EXTERN_C_FUNC int rt_print_stop_debug()
{
    if ((rt_p) == NULL) return -1;

    return (rt_p)->rt_print_level &= ~RT_PRINT_DEBUG;
}


RT_EXTERN_C_FUNC int rt_print_add_kind(int kind)
{
    if (!rt_p) return -1;
    int i = 0;
    for (i = 0; i < RT_MAX_KIND_COUNTS; i++) {
        if (rt_p->rt_kind[i] == 0) {
            rt_p->rt_kind[i] = kind;
            rt_p->rt_kind_cur++;
            break;
        }
    }
    return rt_p->rt_kind_cur;
}

RT_EXTERN_C_FUNC int rt_print_del_kind(int kind)
{
    if (!rt_p) return -1;
    int i;
    for (i = 0; i < RT_MAX_KIND_COUNTS; i++) {
        if (rt_p->rt_kind[i] == 0) break;
        if (rt_p->rt_kind[i] == kind) {
            rt_p->rt_kind[i] = rt_p->rt_kind[rt_p->rt_kind_cur-1];
            rt_p->rt_kind_cur--;
        }
    }
	return rt_p->rt_kind_cur;
}

RT_EXTERN_C_FUNC int rt_print_kind(int kind, const char *func, int line, const char *file, const char *format, ...)
{
    if ((rt_p) == NULL) return -1;
    if (rt_is_print_level_none((rt_p)->rt_print_level) < 0) return -2;
    int i;
    for (i = 0; i < rt_p->rt_kind_cur; i++) {
        if (rt_p->rt_kind[i] == 0) break;
        if (rt_p->rt_kind[i] == kind) break;
    }
    if (i == rt_p->rt_kind_cur) return -3;
    rt_print_set_color(0);

    char  out[RT_STRING_MAX_LEN_SUPPORT];
    memset(out, 0, sizeof(out));

    va_list arg_ptr;
    va_start(arg_ptr, format);
    vsprintf(out, format, arg_ptr);

    rt_print_before(RT_PRINT_DEBUG, func, line, file);
    printf("%s", out);
    if ((rt_p)->rt_enable_color == RT_COLOR_TRUE) {
        printf(RT_COLOR_NONE);
    }

    va_end(arg_ptr);

    return 0;
}

RT_EXTERN_C_FUNC void rt_print_error_string(int errno)
{
    switch(errno) {
        case EIO: rterror("EIO(%d): [%s] io operation error\n", errno, strerror(errno));
        break;
        case EINTR: rterror("EINTR(%d): [%s] interrupt\n", errno, strerror(errno));
        break;
        case EAGAIN: rterror("EAGAIN(%d): [%s] deal again later\n", errno, strerror(errno));
        break;
        case EFAULT: rterror("EFAULT(%d): [%s] pointer param error\n", errno, strerror(errno));
        break;
        case EBADF: rterror("EBADF(%d): [%s] bad file descriptor\n", errno, strerror(errno));
        break;
        case ENOMEM: rterror("ENOMEM(%d): [%s] no memory\n", errno, strerror(errno));
        break;
        case EINVAL: rterror("EINVAL(%d): [%s] invalid param \n", errno, strerror(errno));
        break;
        default:
        rterror("(%d):[%s]", errno, strerror(errno));
        break;
    }
}

///////////////////////////////////////////////////////////////////////////////
// section - test of write to file
///////////////////////////////////////////////////////////////////////////////

RT_EXTERN_C_FUNC int rt_open_file(const char *path)
{
    if (rt_p == NULL) return -1;

    rt_p->file = fopen(path, "a+");
    if (!rt_p->file) return -2;

    return 0;
}

RT_EXTERN_C_FUNC int rt_close_file()
{
    if (rt_p == NULL) return -1;
  
    if (rt_p->file) { fclose(rt_p->file); rt_p->file = NULL; }

    return 0;
}

RT_EXTERN_C_FUNC int rt_writeto_file(const char *func, int line, const char *file, const char *format, ...)
{
    if (rt_p == NULL) return -1;
    char  out[RT_STRING_MAX_LEN_SUPPORT];
    memset(out, 0, sizeof(out));

    va_list arg_ptr;
    va_start(arg_ptr, format);
    vsprintf(out, format, arg_ptr);

    int ret = fwrite(out, 1, strlen(out), rt_p->file);
    if (ret <= 0) {
        printf("error [%d]: \n", ret);
    }

    va_end(arg_ptr);

    return 0;
}

#if 0
int main()
{
    RTVERSION("test for print start \n");
    rtmsg("form message start");

    return 0;
}
#endif

/*=============== End of file: rt_print.c ==========================*/

