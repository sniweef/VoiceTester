/*  Copyright(c) 2009-2014 Shenzhen TP-LINK Technologies Co.Ltd.
 *
 * file     voip_locale.c
 * brief
 * details
 *
 * author   Huang Zhigeng
 * version
 * date     14Oct14
 *
 * history  \arg
 */
#include <string.h>

#include <base/debug.h>
#include <base/voip_locale.h>

/**************************************************************************************************/
/*                                           DEFINES                                              */
/**************************************************************************************************/
#define VOIP_LOCALE_MAX (-1)


/**************************************************************************************************/
/*                                           TYPES                                                */
/**************************************************************************************************/
/*
 *  brief       valid region locale
 *
 */
typedef struct __oal_vrg_cm_tr104_locale_map
{
    int      nVrgId;
    char  *  pCmTxt;
    char  *  pTr104Txt;
    char         pName[64];
} oal_vrg_cm_tr104_locale_map_t;

/**************************************************************************************************/
/*                                           VARIABLES                                            */
/**************************************************************************************************/

static oal_vrg_cm_tr104_locale_map_t  l_oalLocaleMap[] =
{
    /*  VRG Locale define - CM Locale define (Alpha-3) - TR104 Region (Alpha-2) - Name */
    /* �Ĵ����� */
#ifdef VOIP_CFG_AUSTRALIA
    { VOIP_LOCALE_AU,    "AUS",    "AU",        "AUSTRALIA"},
#endif

    /* �׶���˹ */
#ifdef VOIP_CFG_BELARUS
    { VOIP_LOCALE_BY,    "BLR",    "BY",        "BELARUS"},
#endif /* VOIP_CFG_BELARUS */

    /* ����ʱ */
#ifdef VOIP_CFG_BELGIUM
    { VOIP_LOCALE_BE,    "BEL",    "BE",        "BELGIUM"},
#endif

    /* ���� */
#ifdef VOIP_CFG_BRAZIL
    { VOIP_LOCALE_BR,    "BRA",    "BR",        "BRAZIL"},
#endif

    /* ���ô� */
#ifdef VOIP_CFG_CANADA
    { VOIP_LOCALE_CA,    "CAN",    "CA",        "CANADA"},
#endif /* VOIP_CFG_CANADA */

    /* ���� */
#ifdef VOIP_CFG_CHILE
    { VOIP_LOCALE_CL,    "CHL",    "CL",        "CHILE"},
#endif

    /* �й� */
#ifdef VOIP_CFG_CHINA
    { VOIP_LOCALE_CN,    "CHN",    "CN",        "CHINA"},
#endif

    /* ���ױ��� */
#ifdef VOIP_CFG_COLOMBIA
    { VOIP_LOCALE_CO,    "COL",    "CO",        "COLOMBIA"},
#endif /* VOIP_CFG_COLOMBIA */

    /* ���޵��� */
#ifdef VOIP_CFG_CROATIA
    { VOIP_LOCALE_HR,    "HRV",    "HR",        "CROATIA"},
#endif /* VOIP_CFG_CROATIA */

    /* �Ű� */
#ifdef VOIP_CFG_CUBA
    { VOIP_LOCALE_CU,    "CUB",    "CU",        "CUBA"},
#endif /* VOIP_CFG_CUBA */

    /* �ݿ� */
#ifdef VOIP_CFG_CZECH
    { VOIP_LOCALE_CZ,    "CZH",    "CZ",        "CZECH"},
#endif

    /* ���� */
#ifdef VOIP_CFG_DENMARK
    { VOIP_LOCALE_DK,    "DNK",    "DK",        "DENMARK"},
#endif

    /* ��ɳ���� */
#ifdef VOIP_CFG_ESTONIA
    { VOIP_LOCALE_EE,    "EST",    "EE",        "ESTONIA"},
#endif /* VOIP_CFG_ESTONIA */

    /* ŷ��ͨѶ��׼ */
#ifdef VOIP_CFG_ETSI
    { VOIP_LOCALE_EU,    "ETS",    "XE",        "ETSI"},
#endif

    /* ���� */
#ifdef VOIP_CFG_FINLAND
    { VOIP_LOCALE_FI,    "FIN",    "FI",        "FINLAND"},
#endif

    /* ���� */
#ifdef VOIP_CFG_FRANCE
    { VOIP_LOCALE_FR,    "FRA",    "FR",        "FRANCE"},
#endif

    /* �¹� */
#ifdef VOIP_CFG_GERMANY
    { VOIP_LOCALE_DE,    "DEU",    "DE",        "GERMANY"},
#endif

    /* ϣ�� */
#ifdef VOIP_CFG_GREECE
    { VOIP_LOCALE_GR,    "GRC",    "GR",        "GREECE"},
#endif /* VOIP_CFG_GREECE */

    /* ������ */
#ifdef VOIP_CFG_HUNGARY
    { VOIP_LOCALE_HU,    "HUN",    "HU",        "HUNGARY"},
#endif

    /* ӡ�� */
#ifdef VOIP_CFG_INDIA
    { VOIP_LOCALE_IN,    "IND",    "IN",        "INDIA"},
#endif

    /* ӡ�������� */
#ifdef VOIP_CFG_INDONESIA
    { VOIP_LOCALE_ID,    "IDN",    "ID",        "INDONESIA"},
#endif /* VOIP_CFG_INDONESIA */

    /* ������ */
#ifdef VOIP_CFG_IRELAND
    { VOIP_LOCALE_IE,    "IRL",    "IE",        "IRELAND"},
#endif /* VOIP_CFG_IRELAND */

    /* ��ɫ�� */
#ifdef VOIP_CFG_ISRAEL
    { VOIP_LOCALE_IL,    "ISR",    "IL",        "ISRAEL"},
#endif /* VOIP_CFG_ISRAEL */

    /* ����� */
#ifdef VOIP_CFG_ITALY
    { VOIP_LOCALE_IT,    "ITA",    "IT",        "ITALY"},
#endif

    /* �ձ� */
#ifdef VOIP_CFG_JAPAN
    { VOIP_LOCALE_JP,    "JPN",    "JP",        "JAPAN"},
#endif

    /* ���� */
#ifdef VOIP_CFG_KOREA
    { VOIP_LOCALE_KR,    "KOR",    "KR",        "KOREA"},
#endif /* VOIP_CFG_KOREA */

    /* ����ά�� */
#ifdef VOIP_CFG_LATVIA
    { VOIP_LOCALE_LV,    "LVA",    "LV",        "LATVIA"},
#endif /* VOIP_CFG_LATVIA */

    /* ������ */
#ifdef VOIP_CFG_LITHUANIA
    { VOIP_LOCALE_LT,    "LTU",    "LT",        "LITHUANIA"},
#endif /* VOIP_CFG_LITHUANIA */

    /* ī���� */
#ifdef VOIP_CFG_MEXICO
    { VOIP_LOCALE_MX,    "MEX",    "MX",        "MEXICO"},
#endif /* VOIP_CFG_MEXICO */

    /* ���� */
#ifdef VOIP_CFG_NETHERLANDS
    { VOIP_LOCALE_NL,    "NLD",    "NL",        "NETHERLANDS"},
#endif

    /* ������ */
#ifdef VOIP_CFG_NEWZEALAND
    { VOIP_LOCALE_NZ,    "NZL",    "NZ",        "NEWZEALAND"},
#endif

    /* ���� */
#ifdef VOIP_CFG_NORTHAMERICA
    { VOIP_LOCALE_US,    "USA",    "US",        "NORTHAMERICA"},
#endif

    /* Ų�� */
#ifdef VOIP_CFG_NORWAY
    { VOIP_LOCALE_NO,    "NOR",    "NO",        "NORWAY"},
#endif

    /* ��³ */
#ifdef VOIP_CFG_PERU
    { VOIP_LOCALE_PE,    "PER",    "PE",        "PERU"},
#endif /* VOIP_CFG_PERU */

    /* ���� */
#ifdef VOIP_CFG_POLAND
    { VOIP_LOCALE_PL,    "POL",    "PL",        "POLAND"},
#endif /* VOIP_CFG_POLAND */

    /* ������ */
#ifdef VOIP_CFG_PORTUGAL
    { VOIP_LOCALE_PT,    "PRT",    "PT",        "PORTUGAL"},
#endif /* VOIP_CFG_PORTUGAL */

    /* ������ */
#ifdef VOIP_CFG_QATAR
    { VOIP_LOCALE_QA,    "QAT",    "QA",        "QATAR"},
#endif /* VOIP_CFG_QATAR */

    /* �������� */
#ifdef VOIP_CFG_ROMANIA
    { VOIP_LOCALE_RO,    "ROM",    "RO",        "ROMANIA"},
#endif /* VOIP_CFG_ROMANIA */

    /* ����˹ */
#ifdef VOIP_CFG_RUSSIA
    { VOIP_LOCALE_RU,    "RUS",    "RU",        "RUSSIA"},
#endif /* VOIP_CFG_RUSSSIA */

    /* ����ά�� */
#ifdef VOIP_CFG_SERBIA
    { VOIP_LOCALE_RS,    "SRB",    "RS",        "SERBIA"},
#endif /* VOIP_CFG_SERBIA */

    /* �Ϸ� */
#ifdef VOIP_CFG_SOUTHAFRICA
    { VOIP_LOCALE_ZA,    "ZAF",    "ZA",        "SOUTHAFRICA"},
#endif /* VOIP_CFG_SOUTHAFRICA */

    /* ������ */
#ifdef VOIP_CFG_SPAIN
    { VOIP_LOCALE_ES,    "ESP",    "ES",        "SPAIN"},
#endif

    /* ��� */
#ifdef VOIP_CFG_SWEDEN
    { VOIP_LOCALE_SE,    "SWE",    "SE",        "SWEDEN"},
#endif

    /* ��ʿ */
#ifdef VOIP_CFG_SWITZERLAND
    { VOIP_LOCALE_CH,    "CHE",    "CH",        "SWITZERLAND"},
#endif

    /* ̩�� */
#ifdef VOIP_CFG_THAILAND
    { VOIP_LOCALE_TH,    "THA",    "TH",        "THAILAND"},
#endif /* VOIP_CFG_THAILAND */

    /* ������ */
#ifdef VOIP_CFG_TURKEY
    { VOIP_LOCALE_TR,    "TUR",    "TR",        "TURKEY"},
#endif  /* VOIP_CFG_TURKEY */

    /* Ӣ�� */
#ifdef VOIP_CFG_UK
    { VOIP_LOCALE_GB,    "GBR",    "GB",        "UK"},
#endif

    /* ̨�� */
#ifdef VOIP_CFG_TAIWAN
    { VOIP_LOCALE_TW,    "TWN",    "TW",        "TAIWAN"},
#endif

    { VOIP_LOCALE_MAX,      "",                     "XX",       "NONE"}
};  /* end of oal_vrg_cm_tr104_locale_map_t  l_oalLocaleMap */

/**************************************************************************************************/
/*                                           FUNCTIONS                                            */
/**************************************************************************************************/
/*
 *  fn          oalVoice_mapAlpha2toVrg
 *
 *  brief       map from alph2 to vrg
 *
 *  param[in]   locale      --  alph2 locale
 *  param[out]  id          --  vrg id
 *  param[out]  found       --  map successfully or not
 *  param[in]   len         --  locale buffer length
 *
 *  return      CMM_RET
 */
int oalVoice_mapAlpha2toVrg(const char * locale,
                            int * id,
                            uint8_t * found,
                            unsigned int len)
{
    int i = 0;
    int  ret = -1;
    //CX_LOGL(CX_DBG, "map: %s, locale: %s", l_oalLocaleMap[i].pTr104Txt, locale);
    while (l_oalLocaleMap[i].nVrgId != VOIP_LOCALE_MAX)
    {
        //CX_LOGL(CX_DBG, "map: %s, locale: %s", l_oalLocaleMap[i].pTr104Txt, locale);

        if (strncmp(locale, l_oalLocaleMap[i].pTr104Txt, len) == 0)
        {
            *found = 1;
            *id = l_oalLocaleMap[i].nVrgId;
            return 0;
        }

        i++;
    }

    return (ret);
}
