//
// Created by 李玉航 on 2018/9/5.
//

#ifndef DHCAC_CONFIG_H
#define DHCAC_CONFIG_H

#include "limits.h"

#define DHCAC
#define DEBUG

#define CURRENT_SERVER_ADDR     ((ADDR_T)1)
#define UPDATE_TIME             ((uint16_t)10)
#define ADDR_TYPE_UINT8
#define CLIENT_ADDR_NULL        0x00
#define CURRENT_HARD_ADDR       0x11111111
//#define ADDR_TYPE_UINT16

#if !defined(ADDR_TYPE_UINT8) && !defined(ADDR_TYPE_UINT16)
    #error "be must defined ADDR_TYPE"
#endif
#ifdef ADDR_TYPE_UINT8
#define ADDR_T                      uint8_t
#define BOARDCAST_ADDR              (0xFF)
#endif
#if ADDR_TYPE_UINT16
#define ADDR_T                      uint16_t
#define BOARDCAST_ADDR              (0xFFFF)
#endif

#define TIME_T                      uint16_t



#define RESET           0
#define SET             1
#define ADDR_NULL       0

#endif //DHCAC_CONFIG_H
