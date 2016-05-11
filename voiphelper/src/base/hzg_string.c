/*  Copyright(c) 2009-2014 Shenzhen TP-LINK Technologies Co.Ltd.
 *
 * file		hzg_string.c
 * brief
 * details
 *
 * author	Huang Zhigeng
 * version
 * date		13Oct14
 *
 * history 	\arg
 */
#include <string.h>

#include <base/hzg_string.h>
#include <base/debug.h>

/**************************************************************************************************/
/*                                           DEFINES                                              */
/**************************************************************************************************/

/**************************************************************************************************/
/*                                           TYPES                                                */
/**************************************************************************************************/

/**************************************************************************************************/
/*                                           EXTERN_PROTOTYPES                                    */
/**************************************************************************************************/

/**************************************************************************************************/
/*                                           LOCAL_PROTOTYPES                                     */
/**************************************************************************************************/

/**************************************************************************************************/
/*                                           VARIABLES                                            */
/**************************************************************************************************/

/**************************************************************************************************/
/*                                           LOCAL_FUNCTIONS                                      */
/**************************************************************************************************/

/**************************************************************************************************/
/*                                           PUBLIC_FUNCTIONS                                     */
/**************************************************************************************************/

/**************************************************************************************************/
/*                                           GLOBAL_FUNCTIONS                                     */
/**************************************************************************************************/
char *str_trimSpace(char *pStr)
{
    char *pStrStart = pStr;

    if (pStr && *pStr)
    {
        while (WHITESPACE(*pStr))
        {
            pStrStart = ++pStr;
        }

        while (*pStr)
        {
            pStr++;
        }

        while (pStr > pStrStart && WHITESPACE(*--pStr))
        {
            *pStr = '\0';
        }
    }

    return pStrStart;
}
int str_isDigit(const char *pStr)
{
    int isDigit = 1;

    if (pStr == NULL || *pStr == '\0')
    {
        isDigit = 0;
    }
    else
    {
        if (*pStr == '-')
        {
            pStr++;
        }
        while (*pStr)
        {
            if (*pStr < '0' || *pStr > '9')
            {
                isDigit = 0;
                break;
            }
            pStr++;
        }
    }

    return isDigit;
}

const char *str_getNextInt(const char *pStr, int *pInt)
{
    SKIP_WHITE(pStr);

    int isDigit = 1;
    int isNegative = 0;

    *pInt = 0;

    if (pStr == NULL || *pStr == '\0')
    {
        isDigit = 0;
    }
    else
    {
        if (*pStr == '-')
        {
            pStr++;
            isNegative = 1;
        }
        while (*pStr && !WHITESPACE(*pStr))
        {
            if (*pStr < '0' || *pStr > '9')
            {
                isDigit = 0;
                break;
            }
            *pInt = *pInt * 10 + *pStr - '0';
            pStr++;
        }
    }
    if (isNegative)
    {
        *pInt = -*pInt;
    }

    return isDigit ? pStr : NULL;
}
const char *str_getNextStr(const char *pStr, char *pDest, size_t len)
{
    size_t index = 0;

    if (pStr && pDest)
    {
        SKIP_WHITE(pStr);

        while (*pStr && !WHITESPACE(*pStr) && index < len - 1)
        {
            *pDest++ = *pStr++;
            index++;
        }
        *pDest = '\0';
    }
    return index > 0 ? pStr : NULL;
}


char *str_strncpy(char *pDest, const char *pSrc, size_t len)
{
    if (pDest && pSrc)
    {
        strncpy(pDest, pSrc, len);
        pDest[len-1] = '\0';
    }
    return pDest;
}
/*NOTE: This will copy max (len-strlen(pDest)-1) bytes.*/
char *str_strncat(char *pDest, const char *pSrc, size_t len)
{
    if (pDest && pSrc)
    {
        size_t destLen = strlen(pDest);
        size_t index;

        for (index = destLen; index < len - 1 && *pSrc; index++)
        {
            pDest[index] = *pSrc++;
        }
        pDest[index] = '\0';

    }
    return pDest;
}
