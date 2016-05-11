/*  Copyright(c) 2009-2014 Shenzhen TP-LINK Technologies Co.Ltd.
 *
 * file		cli_main.c
 * brief
 * details
 *
 * author	Huang Zhigeng
 * version
 * date		10Oct14
 *
 * history 	\arg
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>

#include <base/common.h>
#include <cli/cli_cmdmap.h>
#include <vt/vt_config.h>
#include <vt/vt_action.h>

/**************************************************************************************************/
/*                                           DEFINES                                              */
/**************************************************************************************************/
#define HIS_FILEPATH ".vc_history"

/**************************************************************************************************/
/*                                           TYPES                                                */
/**************************************************************************************************/

/**************************************************************************************************/
/*                                           EXTERN_PROTOTYPES                                    */
/**************************************************************************************************/
extern CMD_MAP_T g_topCmdMap[];

/**************************************************************************************************/
/*                                           LOCAL_PROTOTYPES                                     */
/**************************************************************************************************/
LOCAL void initReadline();
LOCAL void showWelcome();
LOCAL char **cmdCompletion(const char *pText, int start, int end);
LOCAL int setSearchedList(int start, int end);
LOCAL char *matchWordGenerator(const char *pText, int matchedNumbers);
LOCAL void printCmdDocs(CMD_MAP_T *pCmd);
LOCAL void printArgDocs(CMD_ARG_T *pArg, int depth);

/**************************************************************************************************/
/*                                           VARIABLES                                            */
/**************************************************************************************************/
LOCAL CMD_MAP_T *l_pCmdMapSearched = NULL;
LOCAL CMD_ARG_T *l_pArgsSearched = NULL;
LOCAL uint8_t isIgnoringCase = 1;
LOCAL CMD_MAP_T *l_localCmdMap = g_topCmdMap;

/**************************************************************************************************/
/*                                           LOCAL_FUNCTIONS                                      */
/**************************************************************************************************/
LOCAL void initReadline()
{
	rl_readline_name = "vt_shell";
	rl_attempted_completion_function = cmdCompletion;
}
LOCAL void showWelcome()
{
	printf("======================================================\n");
	printf("=========== Welcome to voip server tester ============\n");
	printf("======================================================\n");
}
LOCAL char **cmdCompletion(const char *pText, int start, int end)
{
	char **ppMatchWords = NULL;

	/*Default, we search the l_localCmdMap.*/
	l_pCmdMapSearched = l_localCmdMap;

	if (start == 0)
	{
		/*It means the word to be completed is at the first word, so it must be a command.*/
		l_pArgsSearched = NULL;
	}
	else
	{
		/*There are some other word before the word to be completed. So firstly we must set */
		/*l_pCmdMapSearched or l_pArgsSearched to the correct list and then search matching*/
		/*words from one of the two list.*/
		int len = rl_point;  /*rl_point from readline.h means the current position of cursor*/

		while (--len > 0 && !WHITESPACE(rl_line_buffer[len]))
		{
			/*Skip the word to be completed*/
		}

		if (setSearchedList(0, len) == 0)
		{
			/*Couldnot find matching words*/
			l_pCmdMapSearched = NULL;
			l_pArgsSearched = NULL;
		}
	}

	ppMatchWords = rl_completion_matches(pText, matchWordGenerator);

	l_pCmdMapSearched = NULL;
	l_pArgsSearched = NULL;

	return ppMatchWords;
}

LOCAL int setSearchedList(int start, int end)
{
	while (start < end && WHITESPACE(rl_line_buffer[start]))
	{
		/*Skip the heading white space.*/
		start++;
	}
	if (start >= end)
	{
		/*The function is called recursely, so this doesnot mean no matching word found.*/
		return -1;
	}

	int nextCharEnd = start;
	while (nextCharEnd < end && !WHITESPACE(rl_line_buffer[nextCharEnd]))
	{
		/*Search the position of first white space*/
		nextCharEnd++;
	}

	int len = nextCharEnd - start;
	CMD_MAP_T *pCmdCurrent = l_pCmdMapSearched;
	CMD_ARG_T *pArgCurrent = NULL;
	int searchResult = 0;

	if (l_pArgsSearched == NULL)
	{
		while (pCmdCurrent && pCmdCurrent->pName)
		{
			if (strncmp(pCmdCurrent->pName, rl_line_buffer+start, len) == 0)
			{
				/*cmd matched*/
				l_pCmdMapSearched = pCmdCurrent->pSubcmdMap;
				searchResult = setSearchedList(nextCharEnd, end);

				if (searchResult == 0)
				{
					/*Cannot found in subcmd map, so search the args map*/
					l_pCmdMapSearched = NULL;
					l_pArgsSearched = pCmdCurrent->pSupportedArgs;

					searchResult = setSearchedList(nextCharEnd, end);

					if (searchResult == 0)
					{
						/*Searching failed.*/
						l_pArgsSearched = NULL;
					}
				}
				else if (searchResult == -1)
				{
					/* Searching reach end. The word to be completed is either in one
					 * of the two list below or NULL.*/
					l_pCmdMapSearched = pCmdCurrent->pSubcmdMap;
					l_pArgsSearched = pCmdCurrent->pSupportedArgs;
					searchResult = 1;
				}
				break;
			}
			pCmdCurrent++;
		}
	}
	else
	{
		/*We search the arg list*/
		pArgCurrent = l_pArgsSearched;

		while (pArgCurrent && pArgCurrent->pName)
		{
			if (strncmp(pArgCurrent->pName, rl_line_buffer+start, end))
			{
				l_pArgsSearched = pArgCurrent->pSubArgs;
				searchResult = setSearchedList(nextCharEnd, end);

				if (searchResult == 0)
				{
					l_pArgsSearched = NULL;
				}
				else if (searchResult == -1)
				{
					l_pArgsSearched = pArgCurrent->pSubArgs;
					searchResult = 1;
				}
				break;
			}
			pArgCurrent++;
		}

	}
	return searchResult;
}
LOCAL char *matchWordGenerator(const char *pText, int matchedNumbers)
{
	const char *pName = NULL;
	LOCAL int s_listIndex, s_len;

	if (l_pCmdMapSearched == NULL && l_pArgsSearched == NULL)
	{
		return NULL;
	}

    int (*fpCmp)(const char *, const char *, size_t) = isIgnoringCase ? strncasecmp : strncmp;


	if (!matchedNumbers)
	{
		/*Only init at the first match for the specific pText.*/
		s_listIndex = 0;
		s_len = strlen(pText);
	}

	if (l_pCmdMapSearched != NULL)
	{
		while ((pName = l_pCmdMapSearched[s_listIndex].pName) != NULL)
		{
			s_listIndex++;
			if (fpCmp(pName, pText, s_len) == 0 || fpCmp(l_pCmdMapSearched[s_listIndex-1].pShortcut, pText, s_len) == 0)
			{
				return strdup(pName);
			}
		}
	}
	if (l_pArgsSearched != NULL)
	{
		while ((pName = l_pArgsSearched[s_listIndex].pName) != NULL)
		{
			s_listIndex++;
			if (fpCmp(pName, pText, s_len) == 0 || fpCmp(l_pArgsSearched[s_listIndex-1].pShortcut, pText, s_len) == 0)
			{
				return strdup(pName);
			}
		}
	}
	return NULL;
}

LOCAL void printCmdDocs(CMD_MAP_T *pCmd)
{
	printf(CLRerr"ERROR." CLRn"Invaild command.\nUsage : %s\n", pCmd->pDocs);

	if (pCmd->pSubcmdMap)
	{
		CMD_MAP_T *pMap = pCmd->pSubcmdMap;
		printf("Sub commands are: \n");

		while (pMap->pName)
		{
			printf("  %s %s(%s): %s\n", pCmd->pName, pMap->pName, pMap->pShortcut, pMap->pDocs);
			pMap++;
		}
	}
	else if (pCmd->pSupportedArgs)
	{
		printf("Arguments:\n");
		printArgDocs(pCmd->pSupportedArgs, 1);
	}
}
LOCAL void printArgDocs(CMD_ARG_T *pArg, int depth)
{
	int index = 0;

	while (pArg->pName)
	{
		for (index = 0; index < depth; index++)
		{
			printf("  ");
		}
		printf("%s(%s) : %s\n", pArg->pName, pArg->pShortcut, pArg->pDocs);

		if (pArg->pSubArgs)
		{
			printArgDocs(pArg->pSubArgs, depth+1);
		}

		pArg++;
	}
}
/**************************************************************************************************/
/*                                           PUBLIC_FUNCTIONS                                     */
/**************************************************************************************************/

/**************************************************************************************************/
/*                                           GLOBAL_FUNCTIONS                                     */
/**************************************************************************************************/
int cli_execCmd(CMD_MAP_T *pCmdMapSearched, char *pCmd)
{
	char *pWordEnd = NULL;
	int len = 0;
	CMD_MAP_T *pSpecificCmd = NULL;
	char *pCmdBak = pCmd;
	int execResult = -1;

	pCmd = str_trimSpace(pCmd);

	while (*pCmd)
	{
		while (*pCmd && WHITESPACE(*pCmd))
		{
			pCmd++;
		}

		pWordEnd = pCmd;
		while (*pWordEnd && !WHITESPACE(*pWordEnd))
		{
			pWordEnd++;
		}

		len = pWordEnd - pCmd;
		if (len > 0)
		{
			/*We only search the subcmd map.*/
			while (pCmdMapSearched && pCmdMapSearched->pName)
			{
				len = strlen(pCmdMapSearched->pName);
				if (strncmp(pCmd, pCmdMapSearched->pName, len) == 0)
				{
					//CX_LOGL(CX_INFO, "%s found.\n", pCmd);
					//printf("%s found. \n", pCmd);
					pSpecificCmd = pCmdMapSearched;
					pCmdMapSearched = pCmdMapSearched->pSubcmdMap;
					goto outLoopEnd;
				}
				pCmdMapSearched++;
			}
			if (pCmdMapSearched == NULL || pCmdMapSearched->pName == NULL)
			{
				/*Cannot match cmd any more*/
				//printf("%s not found.\n", pCmd);
				break;
			}
		}

	outLoopEnd:
		pCmd = pWordEnd;
	}

	if (pSpecificCmd)
	{
		if (pSpecificCmd->fpExecFunc)
		{
			execResult = pSpecificCmd->fpExecFunc(pCmd);
			if (execResult == CMDERR_INVAIL_ARG)
			{
				printCmdDocs(pSpecificCmd);
			}
			else if (execResult == CMDERR_FAILED)
			{
				printf(CLRerr"Executing command: %s failed."CLRnl, pCmdBak);
			}
			else
			{
				printf("Executing command successfully:)\n");
			}
		}
	}
	else
	{
		printf(CLRerr"ERROR." CLRn"Unkown command: %s\n", pCmdBak);
	}


	return execResult;
}
void cli_setIgnoringCase(uint8_t ignoring)
{
	isIgnoringCase = ignoring;
}
void cli_setLocalCmdMap(CMD_MAP_T *pCmdMap)
{
	l_localCmdMap = pCmdMap;
}

GLOBAL int main(int argc, char **argv)
{
    char *pReadline = NULL;

    initReadline();

    showWelcome();

    vt_initConfig(DEFAULT_CFG_PATH);
    vt_loadAction(DEFAULT_ACTION_PATH);

	stifle_history(g_vtConfig.historySize);
    read_history(HIS_FILEPATH);

    CX_LOGL(CX_INFO, "History size is : %d", g_vtConfig.historySize);

    while ((pReadline = readline(CLRcb"# Input your command: "CLRn)) != NULL)
    {
    	l_localCmdMap = g_topCmdMap;
        if (*pReadline)
        {
            add_history(pReadline);

            cli_execCmd(l_localCmdMap, pReadline);
    		write_history(HIS_FILEPATH);
        }

        free(pReadline);
        pReadline = NULL;
    }
	return 0;
}
