/*  Copyright(c) 2009-2014 Shenzhen TP-LINK Technologies Co.Ltd.
 *
 * file		cli_cmdmap.h
 * brief
 * details
 *
 * author	Huang Zhigeng
 * version
 * date		10Oct14
 *
 * history 	\arg
 */

#ifndef __CLI_CMDMAP_H__
#define __CLI_CMDMAP_H__


/**************************************************************************************************/
/*                                           DEFINES                                              */
/**************************************************************************************************/
#define CMDERR_INVAIL_ARG  -2
#define CMDERR_FAILED      -1
#define CMDERR_OK           0


/**************************************************************************************************/
/*                                           TYPES                                                */
/**************************************************************************************************/
/* A command can be: cmd subcmd1 subcmd2 arg1 arg2. Note that arg cannot followed by cmd.*/
typedef struct _CMD_MAP_T CMD_MAP_T;
typedef struct _CMD_ARG_T CMD_ARG_T;
typedef int (*CMD_HANDLER)(const char *pData);

struct _CMD_ARG_T
{
	char *pName;
	char *pDocs;
	CMD_ARG_T *pSubArgs;
};

struct _CMD_MAP_T
{
	char *pName;
	char *pDocs;
	//int (*fpExecFunc)(const char *pData);
    CMD_HANDLER fpExecFunc;
	CMD_MAP_T *pSubcmdMap;
	CMD_ARG_T *pSupportedArgs;
};
/**************************************************************************************************/
/*                                           VARIABLES                                            */
/**************************************************************************************************/

/**************************************************************************************************/
/*                                           FUNCTIONS                                            */
/**************************************************************************************************/


#endif	/* __CLI_CMDMAP_H__ */

