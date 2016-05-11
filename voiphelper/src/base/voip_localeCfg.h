/*  Copyright(c) 2009-2011 Shenzhen TP-LINK Technologies Co.Ltd.
 *
 * file		voip_localeCfg.h
 * brief	locale configuration
 * details
 *
 * author	Mark Lee
 * version	1.0.0
 * date		26Jul11
 *
 * history 	\arg
 */

#ifndef _VOIP_LOCALCFG_H_
/*
 * brief	prevent recursive including
 */
#define _VOIP_LOCALCFG_H_

/**************************************************************************************************/
/*                                           DEFINES                                              */
/**************************************************************************************************/
#ifdef VOIP_CFG_ALL

/*
 * brief	support all countries
 */
#define VOIP_CFG_AUSTRALIA		1
#define VOIP_CFG_BELGIUM		1
#define VOIP_CFG_BRAZIL			1
#define VOIP_CFG_CANADA			1
#undef  VOIP_CFG_CHILE
#define VOIP_CFG_CHINA			1
#define VOIP_CFG_COLOMBIA		1
#undef  VOIP_CFG_CZECH
#undef  VOIP_CFG_DENMARK
#define VOIP_CFG_ETSI			1
#undef  VOIP_CFG_FINLAND
#define VOIP_CFG_FRANCE			1
#define VOIP_CFG_GERMANY		1
#undef  VOIP_CFG_HUNGARY
#define VOIP_CFG_INDIA			1
#define VOIP_CFG_INDONESIA		1
#define VOIP_CFG_ITALY          1
#undef  VOIP_CFG_JAPAN
#define VOIP_CFG_LATVIA			1
#define VOIP_CFG_LITHUANIA		1
#define VOIP_CFG_MEXICO			1
#undef  VOIP_CFG_NETHERLANDS
#define VOIP_CFG_NEWZEALAND		1
#define VOIP_CFG_NORTHAMERICA	1
#undef  VOIP_CFG_NORWAY
#define VOIP_CFG_PERU			1
#define VOIP_CFG_POLAND			1
#define VOIP_CFG_PORTUGAL		1
#define VOIP_CFG_RUSSIA			1
#define VOIP_CFG_SERBIA			1
#define VOIP_CFG_SPAIN			1
#undef  VOIP_CFG_SWEDEN
#undef  VOIP_CFG_SWITZERLAND
#undef  VOIP_CFG_TAIWAN
#define VOIP_CFG_THAILAND		1
#define VOIP_CFG_TURKEY			1
#define VOIP_CFG_UK				1

#endif /* VOIP_CFG_ALL */

/**************************************************************************************************/
/*                                           TYPES                                                */
/**************************************************************************************************/

/**************************************************************************************************/
/*                                           VARIABLES                                            */
/**************************************************************************************************/

/**************************************************************************************************/
/*                                           FUNCTIONS                                            */
/**************************************************************************************************/


#endif  /* _VOIP_LOCALCFG_H_ */

