#ifndef _CX_DEBUG_
#define _CX_DEBUG_


/* Huang Zhigeng, add, 11Oct14 < */
#define CX_LEVEL_FATAL	1
#define CX_LEVEL_ERR	2
#define CX_LEVEL_WARN	3
#define CX_LEVEL_DBG	4
#define CX_LEVEL_INFO	5
#define CX_LEVEL_DUMP	6
/*
 * brief		switch of print function.
 */
#ifndef CX_SWITCH_OF_LEVEL
#define CX_SWITCH_OF_LEVEL			CX_LEVEL_INFO
#endif /* CX_SWITCH_OF_LEVEL */
/* Huang Zhigeng, add, 11Oct14 > */

#ifndef __FILE_NO_DIR__
#include <string.h>
#if defined(_WIN32)
#define __FILE_NO_DIR__		((strrchr(__FILE__, '\\') ?strrchr(__FILE__, '\\'): __FILE__-1)+1)
#else
#define __FILE_NO_DIR__		((strrchr(__FILE__, '/') ?: __FILE__-1)+1)
#endif
#else
//#define __FILE_DEBUG__		__FILE_NO_DIR__
#endif

#define CX_STR(str)   #str

/*
	前景			背景			  颜色
	---------------------------------------
	30				40			  黑色
	31				41			  紅色
	32				42			  綠色
	33				43			  黃色
	34				44			  藍色
	35				45			  紫紅色
	36				46			  青藍色
	37				47			  白色

	代码			  意义
	-------------------------
	0				终端默认设置
	1				高亮显示
	4				使用下划线
	5				闪烁
	7				反白显示
	8				不可见
*/
#define COLOR(clr_code)	 clr_code

/* 为了windows的兼容性，不要使用\e，要使用\033 */
/* no background */
#define CLRk				COLOR("\033[0;30m")		/* blacK				*/
#define CLRr				COLOR("\033[0;31m")		/* Red	 				*/
#define CLRg				COLOR("\033[0;32m")		/* Green				*/
#define CLRy				COLOR("\033[0;33m")		/* Yellow				*/
#define CLRb				COLOR("\033[0;34m")		/* Blue					*/
#define CLRm				COLOR("\033[0;35m")		/* Magenta 				*/
#define CLRc				COLOR("\033[0;36m")		/* Cyan					*/
#define CLRw				COLOR("\033[0;37m")		/* White				*/

/* no background Underline*/
#define CLRku				COLOR("\033[4;30m")		/* blacK				*/
#define CLRru				COLOR("\033[4;31m")		/* Red	 				*/
#define CLRgu				COLOR("\033[4;32m")		/* Green				*/
#define CLRyu				COLOR("\033[4;33m")		/* Yellow				*/
#define CLRbu				COLOR("\033[4;34m")		/* Blue					*/
#define CLRmu				COLOR("\033[4;35m")		/* Magenta 				*/
#define CLRcu				COLOR("\033[4;36m")		/* Cyan					*/
#define CLRwu				COLOR("\033[4;37m")		/* White				*/

/* no background bLink*/
#define CLRkl				COLOR("\033[5;30m")		/* blacK				*/
#define CLRrl				COLOR("\033[5;31m")		/* Red	 				*/
#define CLRgl				COLOR("\033[5;32m")		/* Green				*/
#define CLRyl				COLOR("\033[5;33m")		/* Yellow				*/
#define CLRbl				COLOR("\033[5;34m")		/* Blue					*/
#define CLRml				COLOR("\033[5;35m")		/* Magenta 				*/
#define CLRcl				COLOR("\033[5;36m")		/* Cyan					*/
#define CLRwl				COLOR("\033[5;37m")		/* White				*/

/* white background */
#define CLRkw				COLOR("\033[0;30;47m")	/* blacK	on White	*/
#define CLRrw				COLOR("\033[0;31;47m")	/* Red	 	on White	*/
#define CLRgw				COLOR("\033[0;32;47m")	/* Green	on White	*/
#define CLRyw				COLOR("\033[0;33;47m")	/* Yellow	on White	*/
#define CLRbw				COLOR("\033[0;34;47m")	/* Blue		on White	*/
#define CLRmw				COLOR("\033[0;35;47m")	/* Magenta 	on White	*/
#define CLRcw				COLOR("\033[0;36;47m")	/* Cyan		on White	*/

/* blacK background */
#define CLRrk				COLOR("\033[0;31;40m")	/* Red	 	on blacK	*/
#define CLRgk				COLOR("\033[0;32;40m")	/* Green	on blacK	*/
#define CLRyk				COLOR("\033[0;33;40m")	/* Yellow	on blacK	*/
#define CLRbk				COLOR("\033[0;34;40m")	/* Blue		on blacK	*/
#define CLRmk				COLOR("\033[0;35;40m")	/* Magenta 	on blacK	*/
#define CLRck				COLOR("\033[0;36;40m")	/* Cyan		on blacK	*/
#define CLRwk				COLOR("\033[0;37;40m")	/* White	on blacK	*/

/* red background */
#define CLRkr				COLOR("\033[0;30;41m")	/* blacK	on Red		*/
#define CLRgr				COLOR("\033[0;32;41m")	/* Green	on Red		*/
#define CLRyr				COLOR("\033[0;33;41m")	/* Yellow	on Red		*/
#define CLRbr				COLOR("\033[0;34;41m")	/* Blue		on Red		*/
#define CLRmr				COLOR("\033[0;35;41m")	/* Magenta 	on Red		*/
#define CLRcr				COLOR("\033[0;36;41m")	/* Cyan		on Red		*/
#define CLRwr				COLOR("\033[0;37;41m")	/* White	on Red		*/

/* green background */
#define CLRkg				COLOR("\033[0;30;42m")	/* blacK	on Green	*/
#define CLRrg				COLOR("\033[0;31;42m")	/* Red	 	on Green	*/
#define CLRyg				COLOR("\033[0;33;42m")	/* Yellow	on Green	*/
#define CLRbg				COLOR("\033[0;34;42m")	/* Blue		on Green	*/
#define CLRmg				COLOR("\033[0;35;42m")	/* Magenta 	on Green	*/
#define CLRcg				COLOR("\033[0;36;42m")	/* Cyan		on Green	*/
#define CLRwg				COLOR("\033[0;37;42m")	/* White	on Green	*/

/* red background Underline*/
#define CLRkru				COLOR("\033[4;30;41m")	/* blacK	on Red		*/
#define CLRgru				COLOR("\033[4;32;41m")	/* Green	on Red		*/
#define CLRyru				COLOR("\033[4;33;41m")	/* Yellow	on Red		*/
#define CLRbru				COLOR("\033[4;34;41m")	/* Blue		on Red		*/
#define CLRmru				COLOR("\033[4;35;41m")	/* Magenta 	on Red		*/
#define CLRcru				COLOR("\033[4;36;41m")	/* Cyan		on Red		*/
#define CLRwru				COLOR("\033[4;37;41m")	/* White	on Red		*/

/* green background Underline*/
#define CLRkgu				COLOR("\033[4;30;42m")	/* blacK	on Green	*/
#define CLRrgu				COLOR("\033[4;31;42m")	/* Red	 	on Green	*/
#define CLRygu				COLOR("\033[4;33;42m")	/* Yellow	on Green	*/
#define CLRbgu				COLOR("\033[4;34;42m")	/* Blue		on Green	*/
#define CLRmgu				COLOR("\033[4;35;42m")	/* Magenta 	on Green	*/
#define CLRcgu				COLOR("\033[4;36;42m")	/* Cyan		on Green	*/
#define CLRwgu				COLOR("\033[4;37;42m")	/* White	on Green	*/

/* red background bLink*/
#define CLRkrl				COLOR("\033[5;30;41m")	/* blacK	on Red		*/
#define CLRgrl				COLOR("\033[5;32;41m")	/* Green	on Red		*/
#define CLRyrl				COLOR("\033[5;33;41m")	/* Yellow	on Red		*/
#define CLRbrl				COLOR("\033[5;34;41m")	/* Blue		on Red		*/
#define CLRmrl				COLOR("\033[5;35;41m")	/* Magenta 	on Red		*/
#define CLRcrl				COLOR("\033[5;36;41m")	/* Cyan		on Red		*/
#define CLRwrl				COLOR("\033[5;37;41m")	/* White	on Red		*/

/* green background bLink*/
#define CLRkgl				COLOR("\033[5;30;42m")	/* blacK	on Green	*/
#define CLRrgl				COLOR("\033[5;31;42m")	/* Red	 	on Green	*/
#define CLRygl				COLOR("\033[5;33;42m")	/* Yellow	on Green	*/
#define CLRbgl				COLOR("\033[5;34;42m")	/* Blue		on Green	*/
#define CLRmgl				COLOR("\033[5;35;42m")	/* Magenta 	on Green	*/
#define CLRcgl				COLOR("\033[5;36;42m")	/* Cyan		on Green	*/
#define CLRwgl				COLOR("\033[5;37;42m")	/* White	on Green	*/


/* Colored background */
#define CLRcb				COLOR("\033[0;36;44m")	/* cyan	on blue	*/
#define CLRym				COLOR("\033[0;33;45m")	/* yellow	on magen */

/* Generic foreground colors */
#define CLRhigh				CLRm					 /* Highlight color	*/
#define CLRmkBlink			COLOR("\033[5;35;40m")	/* Blink magenta on black */
#define CLRbold			 	CLRcb					/* Bold		color	*/
#define CLRbold2			CLRym					/* Bold2	 color	*/
#define CLRerr				CLRyr					/* Error	 color	*/
#define CLRBerr			 	CLRyrl					/* Blink Error color	*/
#define CLRn			 	COLOR("\033[0m")			/* Normal	color	*/
#define CLRnl				CLRn "\n"			/* Normal + newline */

#ifdef CX_DEBUG

/* 不需要这些宏，仅仅是为了方便source insight解析*/
#if 0 && CX_DEBUG
#define CX_FATAL
#define CX_ERR
#define CX_WARN
#define CX_DBG
#define CX_INFO
#define CX_DUMP
#endif

#define CX_FATAL_S   "FATAL"
#define CX_ERR_S     "ERR  "
#define CX_WARN_S    "WARN "
#define CX_DBG_S     "DBG  "
#define CX_INFO_S    "INFO "
#define CX_DUMP_S    "DUMP "

#define CX_FATAL_SC   CLRgrl"FATAL"CLRn
#define CX_ERR_SC     CLRrl"ERR  "CLRn
#define CX_WARN_SC    CLRr "WARN "CLRn
#define CX_DBG_SC     CLRy "DBG  "CLRn
#define CX_INFO_SC    CLRg "INFO "CLRn
#define CX_DUMP_SC         "DUMP "

#define CX_LOG_IND	" "			/* 各个元素直接的间隔符 */
#define CX_LOG_LASTIND	"==>"	/* 最后一个元素之后的间隔符 */

#define	CX_LOG_WITH_DAY				1<<0		/* show year/mouth/day */
#define	CX_LOG_WITH_TIME_NO			0<<1		/* show no time */
#define	CX_LOG_WITH_TIME_HMS		1<<1		/* show time as HH:MM:SS */
#define	CX_LOG_WITH_TIME_S			2<<1		/* show time as S* only */
#define	CX_LOG_WITH_TIME_US			1<<3		/* show US* */
#define	CX_LOG_WITH_TIME_MASK		7<<1		/* mask of all time */
#define	CX_LOG_WITH_LEVER			1<<4		/* show level */
#define	CX_LOG_WITH_LEVERCOLOR		1<<5		/* show level with color */
#define	CX_LOG_WITH_FUNC			1<<6		/* show function name */
#define	CX_LOG_WITH_FILE			1<<7		/* show file name */
#define	CX_LOG_WITH_LINE			1<<8		/* show line of file */
#define	CX_LOG_WITH_PID				1<<9		/* show process id */
#define	CX_LOG_WITH_TID				1<<10		/* show thread id */
#define	CX_LOG_WITH_NEWLINE			1<<11		/* show new line at last */

#define CX_LOG_OPTION_ALL 	(CX_LOG_WITH_DAY\
						| CX_LOG_WITH_TIME_HMS	\
						| CX_LOG_WITH_TIME_US	\
						| CX_LOG_WITH_LEVER		\
						| CX_LOG_WITH_LEVERCOLOR\
						| CX_LOG_WITH_FUNC		\
						| CX_LOG_WITH_FILE		\
						| CX_LOG_WITH_LINE		\
						| CX_LOG_WITH_PID		\
						| CX_LOG_WITH_TID		\
						| CX_LOG_WITH_NEWLINE)

/* 由这个宏来控制开启什么打印选项 */
#ifndef	CX_LOG_OPTION

/* Huang Zhigeng, add, 11Oct14 < */
#if defined(__KERNEL__)
#define CX_LOG_WITH_TIME_FMT	CX_LOG_WITH_TIME_S | CX_LOG_WITH_TIME_US
#else
#define CX_LOG_WITH_TIME_FMT	CX_LOG_WITH_DAY | CX_LOG_WITH_TIME_HMS
#endif
/* Huang Zhigeng, add, 11Oct14 > */

#define CX_LOG_OPTION 	(0	/* 默认不要开启CX_LOG_WITH_DAY和CX_LOG_WITH_TIME_HMS，否则内核编不过 */\
						| CX_LOG_WITH_TIME_FMT	\
						| CX_LOG_WITH_LEVER		\
						| CX_LOG_WITH_LEVERCOLOR\
						| CX_LOG_WITH_FUNC		\
						| CX_LOG_WITH_FILE		\
						| CX_LOG_WITH_LINE		\
						| CX_LOG_WITH_PID		\
						/*| CX_LOG_WITH_TID	*/	\
						| CX_LOG_WITH_NEWLINE)
#endif

#ifdef _WIN32	//win32
#include <string.h>

#define CX_PRINTF(...) printf(__VA_ARGS__)
#define CX_LOG_GETPID()		GetCurrentProcessId()
#define CX_LOG_GETTID()		GetCurrentThreadId()

#if CX_LOG_OPTION & (CX_LOG_WITH_DAY | CX_LOG_WITH_TIME_MASK)
#define CX_LOG_GET_TIME_INNER()		\
		SYSTEMTIME sys;			\
		GetLocalTime( &sys );
#else
#define CX_LOG_GET_TIME_INNER()
#endif

#if (CX_LOG_OPTION & CX_LOG_WITH_DAY)
#define CX_LOG_PRINT_DAY()	CX_PRINTF("%02d/%02d/%02d"CX_LOG_IND, sys.wYear, sys.wMonth, sys.wDay)
#else
#define CX_LOG_PRINT_DAY()
#endif

#if (CX_LOG_OPTION & CX_LOG_WITH_TIME_HMS)
#define CX_LOG_PRINT_TIME()	CX_PRINTF("%02d:%02d:%02d"CX_LOG_IND, sys.wHour, sys.wMinute, sys.wSecond)
#elif (CX_LOG_OPTION & CX_LOG_WITH_TIME_S)
#define CX_LOG_PRINT_TIME()	CX_PRINTF("%05d"CX_LOG_IND, sys.wHour*3600 + sys.wMinute*60 + sys.wSecond)
#else
#define CX_LOG_PRINT_TIME()
#endif

#if (CX_LOG_OPTION & CX_LOG_WITH_TIME_US)
#define CX_LOG_PRINT_TIME_US()	CX_PRINTF("%06ld"CX_LOG_IND, sys.wMilliseconds)
#else
#define CX_LOG_PRINT_TIME_US()
#endif

#else	//_WIN32:else linux

#if !defined(__KERNEL__) || defined(CX_DEBUG_USE_PRINTF_IN_KERNEL)
#define CX_PRINTF(args...) printf(args)
#else	/* defined(__KERNEL__) || defined(CX_DEBUG_USE_PRINTF_IN_KERNEL) */
#define CX_PRINTF(args...) printk(args)
#endif	/* defined(__KERNEL__) || defined(CX_DEBUG_USE_PRINTF_IN_KERNEL) */

#if defined(__KERNEL__)
#include <linux/time.h>
#define CX_LOG_GETTIMEOFDAY(tm, tz)	do{(void)tz;do_gettimeofday(tm);}while(0)
#if !defined(MODULE)	//不是编译成模块，直接在内核中
#include <linux/syscalls.h>
#define CX_LOG_GETPID()		sys_getpid()
#define CX_LOG_GETTID()		sys_gettid()
#else	/* !defined(MODULE)，else 在模块中，由于sys_*函数都没有导出，所以不能用 */
#include <linux/sched.h>
#define CX_LOG_GETPID()		task_tgid_vnr(current)
#define CX_LOG_GETTID()		task_pid_vnr(current)
#endif	/* !defined(MODULE)	 */

#else	/* defined(__KERNEL__)，else 用户空间 */
#include <pthread.h>
#include <sys/time.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

#define CX_LOG_GETTIMEOFDAY(tm, tz)	gettimeofday(tm, tz)
#define CX_LOG_GETPID()		getpid()
#define CX_LOG_GETTID()		pthread_self()
#endif	/* defined(__KERNEL__) */

#if CX_LOG_OPTION & (CX_LOG_WITH_DAY| CX_LOG_WITH_TIME_HMS)
#define CX_LOG_LOCAL_TIME()	LocalTime = localtime(&(tv.tv_sec));
#else
#define CX_LOG_LOCAL_TIME()	(void)LocalTime;
#endif

#if CX_LOG_OPTION & (CX_LOG_WITH_DAY| CX_LOG_WITH_TIME_MASK)
#define CX_LOG_GET_TIME_INNER()		\
        struct timeval tv;		\
        /* struct timezone tz; */		\
        struct tm *LocalTime;	\
        CX_LOG_GETTIMEOFDAY(&tv, NULL);\
        CX_LOG_LOCAL_TIME();
#else
#define CX_LOG_GET_TIME_INNER()
#endif

#if (CX_LOG_OPTION & CX_LOG_WITH_DAY)
#define CX_LOG_PRINT_DAY()	CX_PRINTF("%02d/%02d/%02d"CX_LOG_IND, LocalTime->tm_year+1900, LocalTime->tm_mon, LocalTime->tm_mday)
#else
#define CX_LOG_PRINT_DAY()
#endif

#if (CX_LOG_OPTION & CX_LOG_WITH_TIME_HMS)
#define CX_LOG_PRINT_TIME()	CX_PRINTF("%02d:%02d:%02d"CX_LOG_IND, LocalTime->tm_hour, LocalTime->tm_min, LocalTime->tm_sec)
#elif (CX_LOG_OPTION & CX_LOG_WITH_TIME_S)
#define CX_LOG_PRINT_TIME()	CX_PRINTF("%05ld"CX_LOG_IND, tv.tv_sec)
#else
#define CX_LOG_PRINT_TIME()
#endif

#if (CX_LOG_OPTION & CX_LOG_WITH_TIME_US)
#define CX_LOG_PRINT_TIME_US()	CX_PRINTF("%06ld"CX_LOG_IND, tv.tv_usec)
#else
#define CX_LOG_PRINT_TIME_US()
#endif

#endif	//_WIN32

#if CX_LOG_OPTION & (CX_LOG_WITH_LEVER)
#if CX_LOG_OPTION & (CX_LOG_WITH_LEVERCOLOR)
#define CX_LOG_PRINT_LEVEL(level)	CX_PRINTF(level##_SC)
#else
#define CX_LOG_PRINT_LEVEL(level)	CX_PRINTF(level##_S)
#endif
#else
#define CX_LOG_PRINT_LEVEL(level)
#endif

#if CX_LOG_OPTION & (CX_LOG_WITH_FUNC)
#define CX_LOG_PRINT_FUNC(func)		CX_PRINTF(CLRhigh"%20.20s"CLRn CX_LOG_IND, func)
#else
#define CX_LOG_PRINT_FUNC(func)
#endif

#if CX_LOG_OPTION & (CX_LOG_WITH_FILE)
#define CX_LOG_PRINT_FILE(file)		CX_PRINTF("%8.8s"CX_LOG_IND, file)
#else
#define CX_LOG_PRINT_FILE(file)
#endif

#if CX_LOG_OPTION & (CX_LOG_WITH_LINE)
#define CX_LOG_PRINT_LINE(line)		CX_PRINTF("%05d"CX_LOG_IND, line)
#else
#define CX_LOG_PRINT_LINE(line)
#endif

#if CX_LOG_OPTION & (CX_LOG_WITH_PID)
#if !defined(_WIN32) && defined(__KERNEL__) && !defined(MODULE)	//编译成内核且非模块
#define CX_LOG_PRINT_PID()		CX_PRINTF("%05ld"CX_LOG_IND, CX_LOG_GETPID())
#else
#define CX_LOG_PRINT_PID()		CX_PRINTF("%05d"CX_LOG_IND, CX_LOG_GETPID())
#endif
#else
#define CX_LOG_PRINT_PID()
#endif

#if CX_LOG_OPTION & (CX_LOG_WITH_TID)
#if !defined(_WIN32) && !defined(MODULE)	//非内核模块
#define CX_LOG_PRINT_TID()		CX_PRINTF("%05ld"CX_LOG_IND, CX_LOG_GETTID())
#else
#define CX_LOG_PRINT_TID()		CX_PRINTF("%05d"CX_LOG_IND, CX_LOG_GETTID())
#endif
#else
#define CX_LOG_PRINT_TID()
#endif



#if CX_LOG_OPTION & (CX_LOG_WITH_NEWLINE)
#define CX_LOG_NEWLINE		CLRnl
#else
#define CX_LOG_NEWLINE		" "
#endif

#define CX_LOG_LEVEL(level, ...)	\
	do{\
        CX_LOG_GET_TIME_INNER()		\
		CX_LOG_PRINT_DAY();\
		CX_LOG_PRINT_TIME();\
		CX_LOG_PRINT_TIME_US();\
		CX_LOG_PRINT_LEVEL(level);\
		CX_LOG_PRINT_FUNC(__FUNCTION__);\
		CX_LOG_PRINT_FILE(__FILE_NO_DIR__);\
		CX_LOG_PRINT_LINE(__LINE__);\
		CX_LOG_PRINT_PID();\
		CX_LOG_PRINT_TID();\
		CX_PRINTF(CX_LOG_LASTIND __VA_ARGS__);		\
		CX_PRINTF(CX_LOG_NEWLINE);		\
	}while(0)

/* 各个调试等级单独定义，可以单独控制是否开启 */
/* Modified by Huang Zhigeng, 11Oct14 < */
#if (CX_SWITCH_OF_LEVEL >= CX_LEVEL_FATAL)
#define CX_LOG_CX_FATAL(level, ...)		CX_LOG_LEVEL(level, __VA_ARGS__)
#else
#define CX_LOG_CX_FATAL(level, ...)
#endif

#if (CX_SWITCH_OF_LEVEL >= CX_LEVEL_ERR)
#define CX_LOG_CX_ERR(level, ...)   	CX_LOG_LEVEL(level, __VA_ARGS__)
#else
#define CX_LOG_CX_ERR(level, ...)
#endif

#if (CX_SWITCH_OF_LEVEL >= CX_LEVEL_WARN)
#define CX_LOG_CX_WARN(level, ...)   	CX_LOG_LEVEL(level, __VA_ARGS__)
#else
#define CX_LOG_CX_WARN(level, ...)
#endif

#if (CX_SWITCH_OF_LEVEL >= CX_LEVEL_DBG)
#define CX_LOG_CX_DBG(level, ...)   	CX_LOG_LEVEL(level, __VA_ARGS__)
#else
#define CX_LOG_CX_DBG(level, ...)
#endif

#if (CX_SWITCH_OF_LEVEL >= CX_LEVEL_INFO)
#define CX_LOG_CX_INFO(level, ...)   	CX_LOG_LEVEL(level, __VA_ARGS__)
#else
#define CX_LOG_CX_INFO(level, ...)
#endif

#if (CX_SWITCH_OF_LEVEL >= CX_LEVEL_DUMP)
#define CX_LOG_CX_DUMP(level, ...)   	CX_LOG_LEVEL(level, __VA_ARGS__)
#else
#define CX_LOG_CX_DUMP(level, ...)
#endif
/* Modified by Huang Zhigeng, 11Oct14 > */

#define CX_LOGL(level, ...)	CX_LOG_##level(level, __VA_ARGS__)

/* 随意的暂时性的打印 */
#if 0
#define CX_DEBUGP( x, args... ) \
	do {   \
		CX_PRINTF( "%s:%d "CLRg x, __FILE_NO_DIR__, __LINE__, ## args );\
		CX_PRINTF(CLRnl);\
	} while (0)
#define CX_TRACE(x,args...)	CX_PRINTF("%s:%d "CLRy"%s "CLRn x "\n", __FILE_NO_DIR__, __LINE__, __FUNCTION__, ##args)
#else
#define CX_DEBUGP( ...) 	CX_LOGL(CX_WARN,CLRb __VA_ARGS__)
#define CX_TRACE(...)	CX_LOGL(CX_WARN,CLRr __VA_ARGS__)
#endif

#define CX_ERROR( ...)\
	do {   \
		CX_PRINTF( CLRrgl"cx %s:%d ", __FILE_NO_DIR__, __LINE__);\
		CX_PRINTF( __VA_ARGS__ );\
		CX_PRINTF(CLRnl);\
	} while (0)

#if 0	/* gcc扩展可用，标准C不可用 */

#define CX_LOG_LEVEL(level, fmt, args...)	\
	do{\
		CX_LOG_PRINT_DAYTIME();\
		CX_LOG_PRINT_LEVEL(level);\
		CX_LOG_PRINT_FUNC(__FUNCTION__);\
		CX_LOG_PRINT_FILE(__FILE_NO_DIR__);\
		CX_LOG_PRINT_LINE(__LINE__);\
		CX_LOG_PRINT_PID();\
		CX_LOG_PRINT_TID();\
		CX_PRINTF(fmt CX_LOG_NEWLINE, ##args);		\
	}while(0)

/* 各个调试等级单独定义，可以单独控制是否开启 */
#define CX_LOG_CX_FATAL(level, fmt, args...)	CX_LOG_LEVEL(level, fmt, ##args)
#define CX_LOG_CX_ERR(level, fmt, args...)   	CX_LOG_LEVEL(level, fmt, ##args)
#define CX_LOG_CX_WARN(level, fmt, args...)   	CX_LOG_LEVEL(level, fmt, ##args)
#define CX_LOG_CX_DBG(level, fmt, args...)   	CX_LOG_LEVEL(level, fmt, ##args)
#define CX_LOG_CX_INFO(level, fmt, args...)   	CX_LOG_LEVEL(level, fmt, ##args)
#define CX_LOG_CX_DUMP(level, fmt, args...)   	CX_LOG_LEVEL(level, fmt, ##args)

#define CX_LOGL(level, fmt, args...)	CX_LOG_##level(level, fmt, ##args)

/* 随意的暂时性的打印 */
#if 0
#define CX_DEBUGP( x, args... ) \
	do {   \
		CX_PRINTF( "%s:%d "CLRg x, __FILE_NO_DIR__, __LINE__, ## args );\
		CX_PRINTF(CLRnl);\
	} while (0)
#define CX_TRACE(x,args...)	CX_PRINTF("%s:%d "CLRy"%s "CLRn x "\n", __FILE_NO_DIR__, __LINE__, __FUNCTION__, ##args)
#else
#define CX_DEBUGP( x, args... ) 	CX_LOGL(CX_WARN,CLRb x, ##args)
#define CX_TRACE(x,args...)	CX_LOGL(CX_WARN,CLRr x , ##args)
#endif

#define CX_ERROR( x, args... )\
	do {   \
		CX_PRINTF( CLRrgl"cx %s:%d " x, __FILE_NO_DIR__, __LINE__, ## args );\
		CX_PRINTF(CLRnl);\
	} while (0)

#endif

#else //CX_DEBUG
#define CX_LOGL(...)

#define CX_PRINTF(...)
#define CX_DEBUGP( x, ... )
#define CX_TRACE(...)
#if !defined(_WIN32)
#		if !defined(__KERNEL__) || defined(CX_DEBUG_USE_PRINTF_IN_KERNEL)
#define CX_ERROR(x, args...)	printf("\033[5;31;42m""%s:%d " x "\033[0m\n" , __FILE_NO_DIR__, __LINE__, ## args)
#		else	/* defined(__KERNEL__) || defined(CX_DEBUG_USE_PRINTF_IN_KERNEL) */
#define CX_ERROR(x, args...)	printk("\033[5;31;42m""%s:%d " x "\033[0m\n" , __FILE_NO_DIR__, __LINE__, ## args)
#		endif	/* defined(__KERNEL__) || defined(CX_DEBUG_USE_PRINTF_IN_KERNEL) */
#else
#define CX_ERROR(...)	do{printf("\033[5;31;42m""%s:%d ", __FILE_NO_DIR__, __LINE__);\
							printf(__VA_ARGS__);printf("\033[0m\n");}while(0)
#endif
#endif //CX_DEBUG

#endif	//#ifndef _CX_DEBUG_
