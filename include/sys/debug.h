#ifndef DEBUG_HEADER
#define DEBUG_HEADER

#include <arch/color.h>

#include <sys/io.h>
#include <sys/stdlib.h>

#define KERN_INFO "[Info] : "
#define KERN_CRIT "[Fatal] : "
#define KERN_WARN "[Warning] : "

#define DBG_FG FOREGROUND_CYAN
#define DBG_BG BACKGROUND_BLACK

#define DBG_PRINT(x) do { uint8_t fg, bg; sys_tty_get_attr(&fg, &bg); sys_tty_set_attr(DBG_FG, DBG_BG); printf(x); sys_tty_set_attr(fg, bg); } while(0)

#endif	/* DEBUG_HEADER */
