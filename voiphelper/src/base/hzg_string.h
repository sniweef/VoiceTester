/*  Copyright(c) 2009-2014 Shenzhen TP-LINK Technologies Co.Ltd.
 *
 * file		hzg_string.h
 * brief
 * details
 *
 * author	Huang Zhigeng
 * version
 * date		13Oct14
 *
 * history 	\arg
 */

 #include <stdlib.h>

/**************************************************************************************************/
/*                                           DEFINES                                              */
/**************************************************************************************************/
/*
 * brief	visible string for debug purpose
 */
#define STR( arg) { arg, #arg }
#define STR1(arg) #arg

#define WHITESPACE(c) ((c) == ' ' || (c) == '\t')

#define SKIP_WHITE(pChar)                   \
    while (*pChar && WHITESPACE(*pChar))    \
    {                                       \
        pChar++;                            \
    }

#define REACH_NEXT_WHITE(pChar)               \
    while (!WHITESPACE(*pChar))              \
    {                                       \
        pChar++;                            \
    }



/**************************************************************************************************/
/*                                           TYPES                                                */
/**************************************************************************************************/

/**************************************************************************************************/
/*                                           VARIABLES                                            */
/**************************************************************************************************/

/**************************************************************************************************/
/*                                           FUNCTIONS                                            */
/**************************************************************************************************/
char *str_trimSpace(char *pStr);

int str_isDigit(const char *pStr);

const char *str_getNextInt(const char *pStr, int *pInt);
const char *str_getNextStr(const char *pStr, char *pDest, size_t len);

char *str_strncpy(char *pDest, const char *pSrc, size_t len);
char *str_strncat(char *pDest, const char *pSrc, size_t len);
