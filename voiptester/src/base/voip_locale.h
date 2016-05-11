/*  Copyright(c) 2009-2011 Shenzhen TP-LINK Technologies Co.Ltd.
 *
 * file		voip_locale.h
 * brief	header for locale
 * details
 *
 * author	Mark Lee
 * version	1.0.0
 * date		26Jul11
 *
 * history 	\arg
 */

#ifndef _VOIP_LOCALE_H_
/*
 * brief	prevent recursive including
 */
#define _VT_LOCALE_H_
#include <stdint.h>
#include "voip_localeCfg.h"

/**************************************************************************************************/
/*                                           DEFINES                                              */
/**************************************************************************************************/
/*
 * brief	locale make
 */
#define VOIP_LOCALE_MAKE( country )    VOIP_LOCALE_##country,
/*
 * brief	this will generate the enum from voip_localeArchive.h
 */

/**************************************************************************************************/
/*                                           TYPES                                                */
/**************************************************************************************************/
typedef enum VOIP_LOCALE
{
   #include "voip_localeArchive.h"

   VOIP_LOCALE_MAX

} VOIP_LOCALE;

/**************************************************************************************************/
/*                                           VARIABLES                                            */
/**************************************************************************************************/

/**************************************************************************************************/
/*                                           FUNCTIONS                                            */
/**************************************************************************************************/
int oalVoice_mapAlpha2toVrg(const char *locale, int *id, uint8_t *found, unsigned int len);

#endif  /* _VOIP_LOCALE_H_  */



