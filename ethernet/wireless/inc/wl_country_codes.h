/*****************************************************************************
 Copyright 2018 Broadcom Limited.  All rights reserved.

 This program is the proprietary software of Broadcom Limited and/or its
 licensors, and may only be used, duplicated, modified or distributed pursuant
 to the terms and conditions of a separate, written license agreement executed
 between you and Broadcom (an "Authorized License").

 Except as set forth in an Authorized License, Broadcom grants no license
 (express or implied), right to use, or waiver of any kind with respect to the
 Software, and Broadcom expressly reserves all rights in and to the Software
 and all intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED
 LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD
 IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.

  Except as expressly set forth in the Authorized License,
 1. This program, including its structure, sequence and organization,
    constitutes the valuable trade secrets of Broadcom, and you shall use all
    reasonable efforts to protect the confidentiality thereof, and to use this
    information only in connection with your use of Broadcom integrated
    circuit products.

 2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
    AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
    WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT
    TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED
    WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A
    PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR COMPLETENESS,
    QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE TO DESCRIPTION.
    YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR PERFORMANCE OF THE
    SOFTWARE.

 3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
    LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT,
    OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO
    YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN
    ADVISED OF THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS
    OF THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER
    IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF
    ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.
******************************************************************************/
/**
    @defgroup grp_country_code 80211 country codes
    @ingroup grp_wireless

    @addtogroup grp_country_code
    @{

    @file wl_country_codes.h
    @brief Wireless country codes
    This header file defines country codes for wlan

    @version 1.26 Imported from docx
*/

#ifndef WL_COUNTRY_CODE_H
#define WL_COUNTRY_CODE_H

/**
    @name Wireless Ethernet Transceiver Drive Interface IDs
    @{
    @brief Interface IDs for Wireless Ethernet Transceiver Driver
*/
#define BRCM_SWARCH_WLXCVR_MKCOUNTRY_MACRO          (0x0A)  /**< @brief #WLXCVR_MK_CNTRY */
#define BRCM_SWARCH_WLXCVR_COUNTRYCODE_TYPE         (0x0B)  /**< @brief #WLXCVR_CountryCode */
/** @} */


/**
    @brief Make country code helper macro
*/
#define WLXCVR_MK_CNTRY(a, b, rev)          (((unsigned char)(a)) + (((unsigned char)(b))<<8) + (((unsigned short)(rev))<<16))

/**
    @name WLXCVR_CountryCode
    @{
    @brief wlan country codes
*/
typedef uint32_t WLXCVR_CountryCode;
#define WLXCVR_COUNTRY_AFGHANISTAN                               WLXCVR_MK_CNTRY( 'A', 'F', 0 )     /**< @brief AF Afghanistan */
#define WLXCVR_COUNTRY_ALBANIA                                   WLXCVR_MK_CNTRY( 'A', 'L', 0 )     /**< @brief AL Albania */
#define WLXCVR_COUNTRY_ALGERIA                                   WLXCVR_MK_CNTRY( 'D', 'Z', 0 )     /**< @brief DZ Algeria */
#define WLXCVR_COUNTRY_AMERICAN_SAMOA                            WLXCVR_MK_CNTRY( 'A', 'S', 0 )     /**< @brief AS American_Samoa */
#define WLXCVR_COUNTRY_ANGOLA                                    WLXCVR_MK_CNTRY( 'A', 'O', 0 )     /**< @brief AO Angola */
#define WLXCVR_COUNTRY_ANGUILLA                                  WLXCVR_MK_CNTRY( 'A', 'I', 0 )     /**< @brief AI Anguilla */
#define WLXCVR_COUNTRY_ANTIGUA_AND_BARBUDA                       WLXCVR_MK_CNTRY( 'A', 'G', 0 )     /**< @brief AG Antigua_and_Barbuda */
#define WLXCVR_COUNTRY_ARGENTINA                                 WLXCVR_MK_CNTRY( 'A', 'R', 0 )     /**< @brief AR Argentina */
#define WLXCVR_COUNTRY_ARMENIA                                   WLXCVR_MK_CNTRY( 'A', 'M', 0 )     /**< @brief AM Armenia */
#define WLXCVR_COUNTRY_ARUBA                                     WLXCVR_MK_CNTRY( 'A', 'W', 0 )     /**< @brief AW Aruba */
#define WLXCVR_COUNTRY_AUSTRALIA                                 WLXCVR_MK_CNTRY( 'A', 'U', 0 )     /**< @brief AU Australia */
#define WLXCVR_COUNTRY_AUSTRIA                                   WLXCVR_MK_CNTRY( 'A', 'T', 0 )     /**< @brief AT Austria */
#define WLXCVR_COUNTRY_AZERBAIJAN                                WLXCVR_MK_CNTRY( 'A', 'Z', 0 )     /**< @brief AZ Azerbaijan */
#define WLXCVR_COUNTRY_BAHAMAS                                   WLXCVR_MK_CNTRY( 'B', 'S', 0 )     /**< @brief BS Bahamas */
#define WLXCVR_COUNTRY_BAHRAIN                                   WLXCVR_MK_CNTRY( 'B', 'H', 0 )     /**< @brief BH Bahrain */
#define WLXCVR_COUNTRY_BAKER_ISLAND                              WLXCVR_MK_CNTRY( '0', 'B', 0 )     /**< @brief 0B Baker_Island */
#define WLXCVR_COUNTRY_BANGLADESH                                WLXCVR_MK_CNTRY( 'B', 'D', 0 )     /**< @brief BD Bangladesh */
#define WLXCVR_COUNTRY_BARBADOS                                  WLXCVR_MK_CNTRY( 'B', 'B', 0 )     /**< @brief BB Barbados */
#define WLXCVR_COUNTRY_BELARUS                                   WLXCVR_MK_CNTRY( 'B', 'Y', 0 )     /**< @brief BY Belarus */
#define WLXCVR_COUNTRY_BELGIUM                                   WLXCVR_MK_CNTRY( 'B', 'E', 0 )     /**< @brief BE Belgium */
#define WLXCVR_COUNTRY_BELIZE                                    WLXCVR_MK_CNTRY( 'B', 'Z', 0 )     /**< @brief BZ Belize */
#define WLXCVR_COUNTRY_BENIN                                     WLXCVR_MK_CNTRY( 'B', 'J', 0 )     /**< @brief BJ Benin */
#define WLXCVR_COUNTRY_BERMUDA                                   WLXCVR_MK_CNTRY( 'B', 'M', 0 )     /**< @brief BM Bermuda */
#define WLXCVR_COUNTRY_BHUTAN                                    WLXCVR_MK_CNTRY( 'B', 'T', 0 )     /**< @brief BT Bhutan */
#define WLXCVR_COUNTRY_BOLIVIA                                   WLXCVR_MK_CNTRY( 'B', 'O', 0 )     /**< @brief BO Bolivia */
#define WLXCVR_COUNTRY_BOSNIA_AND_HERZEGOVINA                    WLXCVR_MK_CNTRY( 'B', 'A', 0 )     /**< @brief BA Bosnia_and_Herzegovina */
#define WLXCVR_COUNTRY_BOTSWANA                                  WLXCVR_MK_CNTRY( 'B', 'W', 0 )     /**< @brief BW Botswana */
#define WLXCVR_COUNTRY_BRAZIL                                    WLXCVR_MK_CNTRY( 'B', 'R', 0 )     /**< @brief BR Brazil */
#define WLXCVR_COUNTRY_BRITISH_INDIAN_OCEAN_TERRITORY            WLXCVR_MK_CNTRY( 'I', 'O', 0 )     /**< @brief IO British_Indian_Ocean_Territory */
#define WLXCVR_COUNTRY_BRUNEI_DARUSSALAM                         WLXCVR_MK_CNTRY( 'B', 'N', 0 )     /**< @brief BN Brunei_Darussalam */
#define WLXCVR_COUNTRY_BULGARIA                                  WLXCVR_MK_CNTRY( 'B', 'G', 0 )     /**< @brief BG Bulgaria */
#define WLXCVR_COUNTRY_BURKINA_FASO                              WLXCVR_MK_CNTRY( 'B', 'F', 0 )     /**< @brief BF Burkina_Faso */
#define WLXCVR_COUNTRY_BURUNDI                                   WLXCVR_MK_CNTRY( 'B', 'I', 0 )     /**< @brief BI Burundi */
#define WLXCVR_COUNTRY_CAMBODIA                                  WLXCVR_MK_CNTRY( 'K', 'H', 0 )     /**< @brief KH Cambodia */
#define WLXCVR_COUNTRY_CAMEROON                                  WLXCVR_MK_CNTRY( 'C', 'M', 0 )     /**< @brief CM Cameroon */
#define WLXCVR_COUNTRY_CANADA                                    WLXCVR_MK_CNTRY( 'C', 'A', 0 )     /**< @brief CA Canada */
#define WLXCVR_COUNTRY_CAPE_VERDE                                WLXCVR_MK_CNTRY( 'C', 'V', 0 )     /**< @brief CV Cape_Verde */
#define WLXCVR_COUNTRY_CAYMAN_ISLANDS                            WLXCVR_MK_CNTRY( 'K', 'Y', 0 )     /**< @brief KY Cayman_Islands */
#define WLXCVR_COUNTRY_CENTRAL_AFRICAN_REPUBLIC                  WLXCVR_MK_CNTRY( 'C', 'F', 0 )     /**< @brief CF Central_African_Republic */
#define WLXCVR_COUNTRY_CHAD                                      WLXCVR_MK_CNTRY( 'T', 'D', 0 )     /**< @brief TD Chad */
#define WLXCVR_COUNTRY_CHILE                                     WLXCVR_MK_CNTRY( 'C', 'L', 0 )     /**< @brief CL Chile */
#define WLXCVR_COUNTRY_CHINA                                     WLXCVR_MK_CNTRY( 'C', 'N', 0 )     /**< @brief CN China */
#define WLXCVR_COUNTRY_CHRISTMAS_ISLAND                          WLXCVR_MK_CNTRY( 'C', 'X', 0 )     /**< @brief CX Christmas_Island */
#define WLXCVR_COUNTRY_COLOMBIA                                  WLXCVR_MK_CNTRY( 'C', 'O', 0 )     /**< @brief CO Colombia */
#define WLXCVR_COUNTRY_COMOROS                                   WLXCVR_MK_CNTRY( 'K', 'M', 0 )     /**< @brief KM Comoros */
#define WLXCVR_COUNTRY_CONGO                                     WLXCVR_MK_CNTRY( 'C', 'G', 0 )     /**< @brief CG Congo */
#define WLXCVR_COUNTRY_CONGO_THE_DEMOCRATIC_REPUBLIC_OF_THE      WLXCVR_MK_CNTRY( 'C', 'D', 0 )     /**< @brief CD Congo,_The_Democratic_Republic_Of_The */
#define WLXCVR_COUNTRY_COSTA_RICA                                WLXCVR_MK_CNTRY( 'C', 'R', 0 )     /**< @brief CR Costa_Rica */
#define WLXCVR_COUNTRY_COTE_DIVOIRE                              WLXCVR_MK_CNTRY( 'C', 'I', 0 )     /**< @brief CI Cote_D'ivoire */
#define WLXCVR_COUNTRY_CROATIA                                   WLXCVR_MK_CNTRY( 'H', 'R', 0 )     /**< @brief HR Croatia */
#define WLXCVR_COUNTRY_CUBA                                      WLXCVR_MK_CNTRY( 'C', 'U', 0 )     /**< @brief CU Cuba */
#define WLXCVR_COUNTRY_CYPRUS                                    WLXCVR_MK_CNTRY( 'C', 'Y', 0 )     /**< @brief CY Cyprus */
#define WLXCVR_COUNTRY_CZECH_REPUBLIC                            WLXCVR_MK_CNTRY( 'C', 'Z', 0 )     /**< @brief CZ Czech_Republic */
#define WLXCVR_COUNTRY_DENMARK                                   WLXCVR_MK_CNTRY( 'D', 'K', 0 )     /**< @brief DK Denmark */
#define WLXCVR_COUNTRY_DJIBOUTI                                  WLXCVR_MK_CNTRY( 'D', 'J', 0 )     /**< @brief DJ Djibouti */
#define WLXCVR_COUNTRY_DOMINICA                                  WLXCVR_MK_CNTRY( 'D', 'M', 0 )     /**< @brief DM Dominica */
#define WLXCVR_COUNTRY_DOMINICAN_REPUBLIC                        WLXCVR_MK_CNTRY( 'D', 'O', 0 )     /**< @brief DO Dominican_Republic */
#define WLXCVR_COUNTRY_DOWN_UNDER                                WLXCVR_MK_CNTRY( 'A', 'U', 0 )     /**< @brief AU G'Day mate! */
#define WLXCVR_COUNTRY_ECUADOR                                   WLXCVR_MK_CNTRY( 'E', 'C', 0 )     /**< @brief EC Ecuador */
#define WLXCVR_COUNTRY_EGYPT                                     WLXCVR_MK_CNTRY( 'E', 'G', 0 )     /**< @brief EG Egypt */
#define WLXCVR_COUNTRY_EL_SALVADOR                               WLXCVR_MK_CNTRY( 'S', 'V', 0 )     /**< @brief SV El_Salvador */
#define WLXCVR_COUNTRY_EQUATORIAL_GUINEA                         WLXCVR_MK_CNTRY( 'G', 'Q', 0 )     /**< @brief GQ Equatorial_Guinea */
#define WLXCVR_COUNTRY_ERITREA                                   WLXCVR_MK_CNTRY( 'E', 'R', 0 )     /**< @brief ER Eritrea */
#define WLXCVR_COUNTRY_ESTONIA                                   WLXCVR_MK_CNTRY( 'E', 'E', 0 )     /**< @brief EE Estonia */
#define WLXCVR_COUNTRY_ETHIOPIA                                  WLXCVR_MK_CNTRY( 'E', 'T', 0 )     /**< @brief ET Ethiopia */
#define WLXCVR_COUNTRY_FALKLAND_ISLANDS_MALVINAS                 WLXCVR_MK_CNTRY( 'F', 'K', 0 )     /**< @brief FK Falkland_Islands_(Malvinas) */
#define WLXCVR_COUNTRY_FAROE_ISLANDS                             WLXCVR_MK_CNTRY( 'F', 'O', 0 )     /**< @brief FO Faroe_Islands */
#define WLXCVR_COUNTRY_FIJI                                      WLXCVR_MK_CNTRY( 'F', 'J', 0 )     /**< @brief FJ Fiji */
#define WLXCVR_COUNTRY_FINLAND                                   WLXCVR_MK_CNTRY( 'F', 'I', 0 )     /**< @brief FI Finland */
#define WLXCVR_COUNTRY_FRANCE                                    WLXCVR_MK_CNTRY( 'F', 'R', 0 )     /**< @brief FR France */
#define WLXCVR_COUNTRY_FRENCH_GUINA                              WLXCVR_MK_CNTRY( 'G', 'F', 0 )     /**< @brief GF French_Guina */
#define WLXCVR_COUNTRY_FRENCH_POLYNESIA                          WLXCVR_MK_CNTRY( 'P', 'F', 0 )     /**< @brief PF French_Polynesia */
#define WLXCVR_COUNTRY_FRENCH_SOUTHERN_TERRITORIES               WLXCVR_MK_CNTRY( 'T', 'F', 0 )     /**< @brief TF French_Southern_Territories */
#define WLXCVR_COUNTRY_GABON                                     WLXCVR_MK_CNTRY( 'G', 'A', 0 )     /**< @brief GA Gabon */
#define WLXCVR_COUNTRY_GAMBIA                                    WLXCVR_MK_CNTRY( 'G', 'M', 0 )     /**< @brief GM Gambia */
#define WLXCVR_COUNTRY_GEORGIA                                   WLXCVR_MK_CNTRY( 'G', 'E', 0 )     /**< @brief GE Georgia */
#define WLXCVR_COUNTRY_GERMANY                                   WLXCVR_MK_CNTRY( 'D', 'E', 0 )     /**< @brief DE Germany */
#define WLXCVR_COUNTRY_GHANA                                     WLXCVR_MK_CNTRY( 'G', 'H', 0 )     /**< @brief GH Ghana */
#define WLXCVR_COUNTRY_GIBRALTAR                                 WLXCVR_MK_CNTRY( 'G', 'I', 0 )     /**< @brief GI Gibraltar */
#define WLXCVR_COUNTRY_GREECE                                    WLXCVR_MK_CNTRY( 'G', 'R', 0 )     /**< @brief GR Greece */
#define WLXCVR_COUNTRY_GRENADA                                   WLXCVR_MK_CNTRY( 'G', 'D', 0 )     /**< @brief GD Grenada */
#define WLXCVR_COUNTRY_GUADELOUPE                                WLXCVR_MK_CNTRY( 'G', 'P', 0 )     /**< @brief GP Guadeloupe */
#define WLXCVR_COUNTRY_GUAM                                      WLXCVR_MK_CNTRY( 'G', 'U', 0 )     /**< @brief GU Guam */
#define WLXCVR_COUNTRY_GUATEMALA                                 WLXCVR_MK_CNTRY( 'G', 'T', 0 )     /**< @brief GT Guatemala */
#define WLXCVR_COUNTRY_GUERNSEY                                  WLXCVR_MK_CNTRY( 'G', 'G', 0 )     /**< @brief GG Guernsey */
#define WLXCVR_COUNTRY_GUINEA                                    WLXCVR_MK_CNTRY( 'G', 'N', 0 )     /**< @brief GN Guinea */
#define WLXCVR_COUNTRY_GUINEA_BISSAU                             WLXCVR_MK_CNTRY( 'G', 'W', 0 )     /**< @brief GW Guinea-bissau */
#define WLXCVR_COUNTRY_GUYANA                                    WLXCVR_MK_CNTRY( 'G', 'Y', 0 )     /**< @brief GY Guyana */
#define WLXCVR_COUNTRY_HAITI                                     WLXCVR_MK_CNTRY( 'H', 'T', 0 )     /**< @brief HT Haiti */
#define WLXCVR_COUNTRY_HOLY_SEE_VATICAN_CITY_STATE               WLXCVR_MK_CNTRY( 'V', 'A', 0 )     /**< @brief VA Holy_See_(Vatican_City_State) */
#define WLXCVR_COUNTRY_HONDURAS                                  WLXCVR_MK_CNTRY( 'H', 'N', 0 )     /**< @brief HN Honduras */
#define WLXCVR_COUNTRY_HONG_KONG                                 WLXCVR_MK_CNTRY( 'H', 'K', 0 )     /**< @brief HK Hong_Kong */
#define WLXCVR_COUNTRY_HUNGARY                                   WLXCVR_MK_CNTRY( 'H', 'U', 0 )     /**< @brief HU Hungary */
#define WLXCVR_COUNTRY_ICELAND                                   WLXCVR_MK_CNTRY( 'I', 'S', 0 )     /**< @brief IS Iceland */
#define WLXCVR_COUNTRY_INDIA                                     WLXCVR_MK_CNTRY( 'I', 'N', 0 )     /**< @brief IN India */
#define WLXCVR_COUNTRY_INDONESIA                                 WLXCVR_MK_CNTRY( 'I', 'D', 0 )     /**< @brief ID Indonesia */
#define WLXCVR_COUNTRY_IRAN_ISLAMIC_REPUBLIC_OF                  WLXCVR_MK_CNTRY( 'I', 'R', 0 )     /**< @brief IR Iran,_Islamic_Republic_Of */
#define WLXCVR_COUNTRY_IRAQ                                      WLXCVR_MK_CNTRY( 'I', 'Q', 0 )     /**< @brief IQ Iraq */
#define WLXCVR_COUNTRY_IRELAND                                   WLXCVR_MK_CNTRY( 'I', 'E', 0 )     /**< @brief IE Ireland */
#define WLXCVR_COUNTRY_ISRAEL                                    WLXCVR_MK_CNTRY( 'I', 'L', 0 )     /**< @brief IL Israel */
#define WLXCVR_COUNTRY_ITALY                                     WLXCVR_MK_CNTRY( 'I', 'T', 0 )     /**< @brief IT Italy */
#define WLXCVR_COUNTRY_JAMAICA                                   WLXCVR_MK_CNTRY( 'J', 'M', 0 )     /**< @brief JM Jamaica */
#define WLXCVR_COUNTRY_JAPAN                                     WLXCVR_MK_CNTRY( 'J', 'P', 0 )     /**< @brief JP Japan */
#define WLXCVR_COUNTRY_JERSEY                                    WLXCVR_MK_CNTRY( 'J', 'E', 0 )     /**< @brief JE Jersey */
#define WLXCVR_COUNTRY_JORDAN                                    WLXCVR_MK_CNTRY( 'J', 'O', 0 )     /**< @brief JO Jordan */
#define WLXCVR_COUNTRY_KAZAKHSTAN                                WLXCVR_MK_CNTRY( 'K', 'Z', 0 )     /**< @brief KZ Kazakhstan */
#define WLXCVR_COUNTRY_KENYA                                     WLXCVR_MK_CNTRY( 'K', 'E', 0 )     /**< @brief KE Kenya */
#define WLXCVR_COUNTRY_KIRIBATI                                  WLXCVR_MK_CNTRY( 'K', 'I', 0 )     /**< @brief KI Kiribati */
#define WLXCVR_COUNTRY_KOREA_REPUBLIC_OF                         WLXCVR_MK_CNTRY( 'K', 'R', 1 )     /**< @brief KR Korea,_Republic_Of */
#define WLXCVR_COUNTRY_KOSOVO                                    WLXCVR_MK_CNTRY( '0', 'A', 0 )     /**< @brief 0A Kosovo */
#define WLXCVR_COUNTRY_KUWAIT                                    WLXCVR_MK_CNTRY( 'K', 'W', 0 )     /**< @brief KW Kuwait */
#define WLXCVR_COUNTRY_KYRGYZSTAN                                WLXCVR_MK_CNTRY( 'K', 'G', 0 )     /**< @brief KG Kyrgyzstan */
#define WLXCVR_COUNTRY_LAO_PEOPLES_DEMOCRATIC_REPUBIC            WLXCVR_MK_CNTRY( 'L', 'A', 0 )     /**< @brief LA Lao_People's_Democratic_Repubic */
#define WLXCVR_COUNTRY_LATVIA                                    WLXCVR_MK_CNTRY( 'L', 'V', 0 )     /**< @brief LV Latvia */
#define WLXCVR_COUNTRY_LEBANON                                   WLXCVR_MK_CNTRY( 'L', 'B', 0 )     /**< @brief LB Lebanon */
#define WLXCVR_COUNTRY_LESOTHO                                   WLXCVR_MK_CNTRY( 'L', 'S', 0 )     /**< @brief LS Lesotho */
#define WLXCVR_COUNTRY_LIBERIA                                   WLXCVR_MK_CNTRY( 'L', 'R', 0 )     /**< @brief LR Liberia */
#define WLXCVR_COUNTRY_LIBYAN_ARAB_JAMAHIRIYA                    WLXCVR_MK_CNTRY( 'L', 'Y', 0 )     /**< @brief LY Libyan_Arab_Jamahiriya */
#define WLXCVR_COUNTRY_LIECHTENSTEIN                             WLXCVR_MK_CNTRY( 'L', 'I', 0 )     /**< @brief LI Liechtenstein */
#define WLXCVR_COUNTRY_LITHUANIA                                 WLXCVR_MK_CNTRY( 'L', 'T', 0 )     /**< @brief LT Lithuania */
#define WLXCVR_COUNTRY_LUXEMBOURG                                WLXCVR_MK_CNTRY( 'L', 'U', 0 )     /**< @brief LU Luxembourg */
#define WLXCVR_COUNTRY_MACAO                                     WLXCVR_MK_CNTRY( 'M', 'O', 0 )     /**< @brief MO Macao */
#define WLXCVR_COUNTRY_MACEDONIA_FORMER_YUGOSLAV_REPUBLIC_OF     WLXCVR_MK_CNTRY( 'M', 'K', 0 )     /**< @brief MK Macedonia,_Former_Yugoslav_Republic_Of */
#define WLXCVR_COUNTRY_MADAGASCAR                                WLXCVR_MK_CNTRY( 'M', 'G', 0 )     /**< @brief MG Madagascar */
#define WLXCVR_COUNTRY_MALAWI                                    WLXCVR_MK_CNTRY( 'M', 'W', 0 )     /**< @brief MW Malawi */
#define WLXCVR_COUNTRY_MALAYSIA                                  WLXCVR_MK_CNTRY( 'M', 'Y', 0 )     /**< @brief MY Malaysia */
#define WLXCVR_COUNTRY_MALDIVES                                  WLXCVR_MK_CNTRY( 'M', 'V', 0 )     /**< @brief MV Maldives */
#define WLXCVR_COUNTRY_MALI                                      WLXCVR_MK_CNTRY( 'M', 'L', 0 )     /**< @brief ML Mali */
#define WLXCVR_COUNTRY_MALTA                                     WLXCVR_MK_CNTRY( 'M', 'T', 0 )     /**< @brief MT Malta */
#define WLXCVR_COUNTRY_MAN_ISLE_OF                               WLXCVR_MK_CNTRY( 'I', 'M', 0 )     /**< @brief IM Man,_Isle_Of */
#define WLXCVR_COUNTRY_MARTINIQUE                                WLXCVR_MK_CNTRY( 'M', 'Q', 0 )     /**< @brief MQ Martinique */
#define WLXCVR_COUNTRY_MAURITANIA                                WLXCVR_MK_CNTRY( 'M', 'R', 0 )     /**< @brief MR Mauritania */
#define WLXCVR_COUNTRY_MAURITIUS                                 WLXCVR_MK_CNTRY( 'M', 'U', 0 )     /**< @brief MU Mauritius */
#define WLXCVR_COUNTRY_MAYOTTE                                   WLXCVR_MK_CNTRY( 'Y', 'T', 0 )     /**< @brief YT Mayotte */
#define WLXCVR_COUNTRY_MEXICO                                    WLXCVR_MK_CNTRY( 'M', 'X', 0 )     /**< @brief MX Mexico */
#define WLXCVR_COUNTRY_MICRONESIA_FEDERATED_STATES_OF            WLXCVR_MK_CNTRY( 'F', 'M', 0 )     /**< @brief FM Micronesia,_Federated_States_Of */
#define WLXCVR_COUNTRY_MOLDOVA_REPUBLIC_OF                       WLXCVR_MK_CNTRY( 'M', 'D', 0 )     /**< @brief MD Moldova,_Republic_Of */
#define WLXCVR_COUNTRY_MONACO                                    WLXCVR_MK_CNTRY( 'M', 'C', 0 )     /**< @brief MC Monaco */
#define WLXCVR_COUNTRY_MONGOLIA                                  WLXCVR_MK_CNTRY( 'M', 'N', 0 )     /**< @brief MN Mongolia */
#define WLXCVR_COUNTRY_MONTENEGRO                                WLXCVR_MK_CNTRY( 'M', 'E', 0 )     /**< @brief ME Montenegro */
#define WLXCVR_COUNTRY_MONTSERRAT                                WLXCVR_MK_CNTRY( 'M', 'S', 0 )     /**< @brief MS Montserrat */
#define WLXCVR_COUNTRY_MOROCCO                                   WLXCVR_MK_CNTRY( 'M', 'A', 0 )     /**< @brief MA Morocco */
#define WLXCVR_COUNTRY_MOZAMBIQUE                                WLXCVR_MK_CNTRY( 'M', 'Z', 0 )     /**< @brief MZ Mozambique */
#define WLXCVR_COUNTRY_MYANMAR                                   WLXCVR_MK_CNTRY( 'M', 'M', 0 )     /**< @brief MM Myanmar */
#define WLXCVR_COUNTRY_NAMIBIA                                   WLXCVR_MK_CNTRY( 'N', 'A', 0 )     /**< @brief NA Namibia */
#define WLXCVR_COUNTRY_NAURU                                     WLXCVR_MK_CNTRY( 'N', 'R', 0 )     /**< @brief NR Nauru */
#define WLXCVR_COUNTRY_NEPAL                                     WLXCVR_MK_CNTRY( 'N', 'P', 0 )     /**< @brief NP Nepal */
#define WLXCVR_COUNTRY_NETHERLANDS                               WLXCVR_MK_CNTRY( 'N', 'L', 0 )     /**< @brief NL Netherlands */
#define WLXCVR_COUNTRY_NETHERLANDS_ANTILLES                      WLXCVR_MK_CNTRY( 'A', 'N', 0 )     /**< @brief AN Netherlands_Antilles */
#define WLXCVR_COUNTRY_NEW_CALEDONIA                             WLXCVR_MK_CNTRY( 'N', 'C', 0 )     /**< @brief NC New_Caledonia */
#define WLXCVR_COUNTRY_NEW_ZEALAND                               WLXCVR_MK_CNTRY( 'N', 'Z', 0 )     /**< @brief NZ New_Zealand */
#define WLXCVR_COUNTRY_NICARAGUA                                 WLXCVR_MK_CNTRY( 'N', 'I', 0 )     /**< @brief NI Nicaragua */
#define WLXCVR_COUNTRY_NIGER                                     WLXCVR_MK_CNTRY( 'N', 'E', 0 )     /**< @brief NE Niger */
#define WLXCVR_COUNTRY_NIGERIA                                   WLXCVR_MK_CNTRY( 'N', 'G', 0 )     /**< @brief NG Nigeria */
#define WLXCVR_COUNTRY_NORFOLK_ISLAND                            WLXCVR_MK_CNTRY( 'N', 'F', 0 )     /**< @brief NF Norfolk_Island */
#define WLXCVR_COUNTRY_NORTHERN_MARIANA_ISLANDS                  WLXCVR_MK_CNTRY( 'M', 'P', 0 )     /**< @brief MP Northern_Mariana_Islands */
#define WLXCVR_COUNTRY_NORWAY                                    WLXCVR_MK_CNTRY( 'N', 'O', 0 )     /**< @brief NO Norway */
#define WLXCVR_COUNTRY_OMAN                                      WLXCVR_MK_CNTRY( 'O', 'M', 0 )     /**< @brief OM Oman */
#define WLXCVR_COUNTRY_PAKISTAN                                  WLXCVR_MK_CNTRY( 'P', 'K', 0 )     /**< @brief PK Pakistan */
#define WLXCVR_COUNTRY_PALAU                                     WLXCVR_MK_CNTRY( 'P', 'W', 0 )     /**< @brief PW Palau */
#define WLXCVR_COUNTRY_PANAMA                                    WLXCVR_MK_CNTRY( 'P', 'A', 0 )     /**< @brief PA Panama */
#define WLXCVR_COUNTRY_PAPUA_NEW_GUINEA                          WLXCVR_MK_CNTRY( 'P', 'G', 0 )     /**< @brief PG Papua_New_Guinea */
#define WLXCVR_COUNTRY_PARAGUAY                                  WLXCVR_MK_CNTRY( 'P', 'Y', 0 )     /**< @brief PY Paraguay */
#define WLXCVR_COUNTRY_PERU                                      WLXCVR_MK_CNTRY( 'P', 'E', 0 )     /**< @brief PE Peru */
#define WLXCVR_COUNTRY_PHILIPPINES                               WLXCVR_MK_CNTRY( 'P', 'H', 0 )     /**< @brief PH Philippines */
#define WLXCVR_COUNTRY_POLAND                                    WLXCVR_MK_CNTRY( 'P', 'L', 0 )     /**< @brief PL Poland */
#define WLXCVR_COUNTRY_PORTUGAL                                  WLXCVR_MK_CNTRY( 'P', 'T', 0 )     /**< @brief PT Portugal */
#define WLXCVR_COUNTRY_PUETO_RICO                                WLXCVR_MK_CNTRY( 'P', 'R', 0 )     /**< @brief PR Pueto_Rico */
#define WLXCVR_COUNTRY_QATAR                                     WLXCVR_MK_CNTRY( 'Q', 'A', 0 )     /**< @brief QA Qatar */
#define WLXCVR_COUNTRY_REUNION                                   WLXCVR_MK_CNTRY( 'R', 'E', 0 )     /**< @brief RE Reunion */
#define WLXCVR_COUNTRY_ROMANIA                                   WLXCVR_MK_CNTRY( 'R', 'O', 0 )     /**< @brief RO Romania */
#define WLXCVR_COUNTRY_RUSSIAN_FEDERATION                        WLXCVR_MK_CNTRY( 'R', 'U', 0 )     /**< @brief RU Russian_Federation */
#define WLXCVR_COUNTRY_RWANDA                                    WLXCVR_MK_CNTRY( 'R', 'W', 0 )     /**< @brief RW Rwanda */
#define WLXCVR_COUNTRY_SAINT_KITTS_AND_NEVIS                     WLXCVR_MK_CNTRY( 'K', 'N', 0 )     /**< @brief KN Saint_Kitts_and_Nevis */
#define WLXCVR_COUNTRY_SAINT_LUCIA                               WLXCVR_MK_CNTRY( 'L', 'C', 0 )     /**< @brief LC Saint_Lucia */
#define WLXCVR_COUNTRY_SAINT_PIERRE_AND_MIQUELON                 WLXCVR_MK_CNTRY( 'P', 'M', 0 )     /**< @brief PM Saint_Pierre_and_Miquelon */
#define WLXCVR_COUNTRY_SAINT_VINCENT_AND_THE_GRENADINES          WLXCVR_MK_CNTRY( 'V', 'C', 0 )     /**< @brief VC Saint_Vincent_and_The_Grenadines */
#define WLXCVR_COUNTRY_SAMOA                                     WLXCVR_MK_CNTRY( 'W', 'S', 0 )     /**< @brief WS Samoa */
#define WLXCVR_COUNTRY_SANIT_MARTIN_SINT_MARTEEN                 WLXCVR_MK_CNTRY( 'M', 'F', 0 )     /**< @brief MF Sanit_Martin_/_Sint_Marteen */
#define WLXCVR_COUNTRY_SAO_TOME_AND_PRINCIPE                     WLXCVR_MK_CNTRY( 'S', 'T', 0 )     /**< @brief ST Sao_Tome_and_Principe */
#define WLXCVR_COUNTRY_SAUDI_ARABIA                              WLXCVR_MK_CNTRY( 'S', 'A', 0 )     /**< @brief SA Saudi_Arabia */
#define WLXCVR_COUNTRY_SENEGAL                                   WLXCVR_MK_CNTRY( 'S', 'N', 0 )     /**< @brief SN Senegal */
#define WLXCVR_COUNTRY_SERBIA                                    WLXCVR_MK_CNTRY( 'R', 'S', 0 )     /**< @brief RS Serbia */
#define WLXCVR_COUNTRY_SEYCHELLES                                WLXCVR_MK_CNTRY( 'S', 'C', 0 )     /**< @brief SC Seychelles */
#define WLXCVR_COUNTRY_SIERRA_LEONE                              WLXCVR_MK_CNTRY( 'S', 'L', 0 )     /**< @brief SL Sierra_Leone */
#define WLXCVR_COUNTRY_SINGAPORE                                 WLXCVR_MK_CNTRY( 'S', 'G', 0 )     /**< @brief SG Singapore */
#define WLXCVR_COUNTRY_SLOVAKIA                                  WLXCVR_MK_CNTRY( 'S', 'K', 0 )     /**< @brief SK Slovakia */
#define WLXCVR_COUNTRY_SLOVENIA                                  WLXCVR_MK_CNTRY( 'S', 'I', 0 )     /**< @brief SI Slovenia */
#define WLXCVR_COUNTRY_SOLOMON_ISLANDS                           WLXCVR_MK_CNTRY( 'S', 'B', 0 )     /**< @brief SB Solomon_Islands */
#define WLXCVR_COUNTRY_SOMALIA                                   WLXCVR_MK_CNTRY( 'S', 'O', 0 )     /**< @brief SO Somalia */
#define WLXCVR_COUNTRY_SOUTH_AFRICA                              WLXCVR_MK_CNTRY( 'Z', 'A', 0 )     /**< @brief ZA South_Africa */
#define WLXCVR_COUNTRY_SPAIN                                     WLXCVR_MK_CNTRY( 'E', 'S', 0 )     /**< @brief ES Spain */
#define WLXCVR_COUNTRY_SRI_LANKA                                 WLXCVR_MK_CNTRY( 'L', 'K', 0 )     /**< @brief LK Sri_Lanka */
#define WLXCVR_COUNTRY_SURINAME                                  WLXCVR_MK_CNTRY( 'S', 'R', 0 )     /**< @brief SR Suriname */
#define WLXCVR_COUNTRY_SWAZILAND                                 WLXCVR_MK_CNTRY( 'S', 'Z', 0 )     /**< @brief SZ Swaziland */
#define WLXCVR_COUNTRY_SWEDEN                                    WLXCVR_MK_CNTRY( 'S', 'E', 0 )     /**< @brief SE Sweden */
#define WLXCVR_COUNTRY_SWITZERLAND                               WLXCVR_MK_CNTRY( 'C', 'H', 0 )     /**< @brief CH Switzerland */
#define WLXCVR_COUNTRY_SYRIAN_ARAB_REPUBLIC                      WLXCVR_MK_CNTRY( 'S', 'Y', 0 )     /**< @brief SY Syrian_Arab_Republic */
#define WLXCVR_COUNTRY_TAIWAN_PROVINCE_OF_CHINA                  WLXCVR_MK_CNTRY( 'T', 'W', 0 )     /**< @brief TW Taiwan,_Province_Of_China */
#define WLXCVR_COUNTRY_TAJIKISTAN                                WLXCVR_MK_CNTRY( 'T', 'J', 0 )     /**< @brief TJ Tajikistan */
#define WLXCVR_COUNTRY_TANZANIA_UNITED_REPUBLIC_OF               WLXCVR_MK_CNTRY( 'T', 'Z', 0 )     /**< @brief TZ Tanzania,_United_Republic_Of */
#define WLXCVR_COUNTRY_THAILAND                                  WLXCVR_MK_CNTRY( 'T', 'H', 0 )     /**< @brief TH Thailand */
#define WLXCVR_COUNTRY_TOGO                                      WLXCVR_MK_CNTRY( 'T', 'G', 0 )     /**< @brief TG Togo */
#define WLXCVR_COUNTRY_TONGA                                     WLXCVR_MK_CNTRY( 'T', 'O', 0 )     /**< @brief TO Tonga */
#define WLXCVR_COUNTRY_TRINIDAD_AND_TOBAGO                       WLXCVR_MK_CNTRY( 'T', 'T', 0 )     /**< @brief TT Trinidad_and_Tobago */
#define WLXCVR_COUNTRY_TUNISIA                                   WLXCVR_MK_CNTRY( 'T', 'N', 0 )     /**< @brief TN Tunisia */
#define WLXCVR_COUNTRY_TURKEY                                    WLXCVR_MK_CNTRY( 'T', 'R', 0 )     /**< @brief TR Turkey */
#define WLXCVR_COUNTRY_TURKMENISTAN                              WLXCVR_MK_CNTRY( 'T', 'M', 0 )     /**< @brief TM Turkmenistan */
#define WLXCVR_COUNTRY_TURKS_AND_CAICOS_ISLANDS                  WLXCVR_MK_CNTRY( 'T', 'C', 0 )     /**< @brief TC Turks_and_Caicos_Islands */
#define WLXCVR_COUNTRY_TUVALU                                    WLXCVR_MK_CNTRY( 'T', 'V', 0 )     /**< @brief TV Tuvalu */
#define WLXCVR_COUNTRY_UGANDA                                    WLXCVR_MK_CNTRY( 'U', 'G', 0 )     /**< @brief UG Uganda */
#define WLXCVR_COUNTRY_UKRAINE                                   WLXCVR_MK_CNTRY( 'U', 'A', 0 )     /**< @brief UA Ukraine */
#define WLXCVR_COUNTRY_UNITED_ARAB_EMIRATES                      WLXCVR_MK_CNTRY( 'A', 'E', 0 )     /**< @brief AE United_Arab_Emirates */
#define WLXCVR_COUNTRY_UNITED_KINGDOM                            WLXCVR_MK_CNTRY( 'G', 'B', 0 )     /**< @brief GB United_Kingdom */
#define WLXCVR_COUNTRY_UNITED_STATES                             WLXCVR_MK_CNTRY( 'U', 'S', 0 )     /**< @brief US United_States */
#define WLXCVR_COUNTRY_UNITED_STATES_REV4                        WLXCVR_MK_CNTRY( 'U', 'S', 4 )     /**< @brief US United_States Revision 4 */
#define WLXCVR_COUNTRY_UNITED_STATES_NO_DFS                      WLXCVR_MK_CNTRY( 'Q', '2', 0 )     /**< @brief Q2 United_States_(No_DFS) */
#define WLXCVR_COUNTRY_UNITED_STATES_MINOR_OUTLYING_ISLANDS      WLXCVR_MK_CNTRY( 'U', 'M', 0 )     /**< @brief UM United_States_Minor_Outlying_Islands */
#define WLXCVR_COUNTRY_URUGUAY                                   WLXCVR_MK_CNTRY( 'U', 'Y', 0 )     /**< @brief UY Uruguay */
#define WLXCVR_COUNTRY_UZBEKISTAN                                WLXCVR_MK_CNTRY( 'U', 'Z', 0 )     /**< @brief UZ Uzbekistan */
#define WLXCVR_COUNTRY_VANUATU                                   WLXCVR_MK_CNTRY( 'V', 'U', 0 )     /**< @brief VU Vanuatu */
#define WLXCVR_COUNTRY_VENEZUELA                                 WLXCVR_MK_CNTRY( 'V', 'E', 0 )     /**< @brief VE Venezuela */
#define WLXCVR_COUNTRY_VIET_NAM                                  WLXCVR_MK_CNTRY( 'V', 'N', 0 )     /**< @brief VN Viet_Nam */
#define WLXCVR_COUNTRY_VIRGIN_ISLANDS_BRITISH                    WLXCVR_MK_CNTRY( 'V', 'G', 0 )     /**< @brief VG Virgin_Islands,_British */
#define WLXCVR_COUNTRY_VIRGIN_ISLANDS_US                         WLXCVR_MK_CNTRY( 'V', 'I', 0 )     /**< @brief VI Virgin_Islands,_U.S. */
#define WLXCVR_COUNTRY_WALLIS_AND_FUTUNA                         WLXCVR_MK_CNTRY( 'W', 'F', 0 )     /**< @brief WF Wallis_and_Futuna */
#define WLXCVR_COUNTRY_WEST_BANK                                 WLXCVR_MK_CNTRY( '0', 'C', 0 )     /**< @brief 0C West_Bank */
#define WLXCVR_COUNTRY_WESTERN_SAHARA                            WLXCVR_MK_CNTRY( 'E', 'H', 0 )     /**< @brief EH Western_Sahara */
#define WLXCVR_COUNTRY_WORLD_WIDE_XX                             WLXCVR_MK_CNTRY( 'X', 'X', 0 )     /**< @brief Worldwide Locale (passive Ch12-14) */
#define WLXCVR_COUNTRY_YEMEN                                     WLXCVR_MK_CNTRY( 'Y', 'E', 0 )     /**< @brief YE Yemen */
#define WLXCVR_COUNTRY_ZAMBIA                                    WLXCVR_MK_CNTRY( 'Z', 'M', 0 )     /**< @brief ZM Zambia */
#define WLXCVR_COUNTRY_ZIMBABWE                                  WLXCVR_MK_CNTRY( 'Z', 'W', 0 )     /**< @brief ZW Zimbabwe */
/** @} */

#endif /* WL_COUNTRY_CODE_H */
/** @} */


