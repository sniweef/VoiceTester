/*  Copyright(c) 2009-2014 Shenzhen TP-LINK Technologies Co.Ltd.
 *
 * file		string.c
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

#include <base/string.h>
#include <base/debug.h>

/**************************************************************************************************/
/*                                           DEFINES                                              */
/**************************************************************************************************/
#define STR_ABS(x)  ((x) >= 0 ? (x) : -(x))

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
/*NOTE: DONOT forget to free the result when you don't need it any more!*/
/*pFirstName is always the program name.*/
char **str_dumpArgv(const char *pFirstName, const char *pSrc, int *pArgc)
{
    char **pResult = NULL;
    const char *pSrcTmp = pSrc;

    if (pSrc && pArgc)
    {
        int wordCount = 0;
        const char *pWordBegin;

        while (*pSrcTmp)
        {
            SKIP_WHITE(pSrcTmp);
            pWordBegin = pSrcTmp;
            REACH_NEXT_WHITE(pSrcTmp);

            if (pSrcTmp > pWordBegin)
            {
                wordCount++;
            }
        }
        //CX_LOGL(CX_INFO, "Get word count: %d", wordCount);
        if (wordCount > 0)
        {
            int index = 0;
            int len = 0;

            pResult = calloc(wordCount+2, sizeof(char *)); /*First element is pFirstName. Last is NULL*/

            pResult[0] = (char *)pFirstName;     /*Avoid warning*/

            for (index = 1; index <= wordCount; index++)
            {
                SKIP_WHITE(pSrc);
                pWordBegin = pSrc;
                REACH_NEXT_WHITE(pSrc);
                len = pSrc - pWordBegin;

                pResult[index] = calloc(len+1, sizeof(char));
                str_strncpy(pResult[index], pWordBegin, len+1);
            }
            pResult[wordCount+1] = NULL;

            *pArgc = wordCount+1;
        }
    }
    return pResult;
}
void str_freeArgv(int argc, char **pArgv)
{
    int index = 0;

    /*pArgv[0] is just pointing to a const string. */
    for (index = 1; index < argc; index++)
    {
        if (pArgv[index])
        {
            free(pArgv[index]);
        }
    }
    free(pArgv);
}


int str_isLetter(char c)
{
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

int str_letterCaseEqual(char a, char b)
{
    if (!str_isLetter(a) || !str_isLetter(b))
    {
        return 0;
    }
    return STR_ABS(a - b)  == STR_ABS('a' - 'A');
}

int str_caseStartWith(const char *pStr, const char *pStart)
{
    if (pStr == NULL || pStart == NULL)
    {
        return 0;
    }

    while (*pStr != '\0' && *pStart != '\0')
    {
        if (*pStr == *pStart || str_letterCaseEqual(*pStr, *pStart))
        {
            pStr++;
            pStart++;
        }
        else
        {
            break;
        }
    }

    return *pStart == '\0';
}


int str_startWith(const char *pStr, const char *pStart)
{
    //assert(pStr && pStart);
    if (pStr == NULL || pStart == NULL)
    {
        return 0;
    }

    while (*pStr != '\0' && *pStart != '\0')
    {
        if (*pStr == *pStart)
        {
            pStr++;
            pStart++;
        }
        else
        {
            break;
        }
    }

    return *pStart == '\0';
}
