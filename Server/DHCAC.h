//
// Created by 李玉航 on 2018/9/5.
//

#ifndef DHCAC_DHCAC_H
#define DHCAC_DHCAC_H

#include "types.h"
#include "config.h"


#define ADDR_MAX                            (8)
#define STATIC_ADDR_CONFIG_MAX              (10)
#define DYNAMIC_ADDR_CONFIG_MAX             (ADDR_MAX << 5)





#define CLV_SET_CODE(_clv, _code)                           _clv.code = _code
#define CLV_SET_LEN(_clv, _len)                             _clv.len = _len
#define CLV_SET_VALUE_INT(_clv, _value)                     _clv.value.value_i = _value
#define CLV_SET_VALUE_FLOAT(_clv, _value)                   _clv.value.value_f = _value
#define CLV_SET_VALUE_STRING(_clv, _value)                  _clv.value.value_cp = _value

#define CLV_GET_CODE(_clv)                                  _clv.code
#define CLV_GET_LEN(_clv)                                   _clv.len
#define CLV_GET_VALUE_INT(_clv)                             _clv.value.value_i
#define CLV_GET_VALUE_FLOAT(_clv)                           _clv.value.value_f
#define CLV_GET_VALUE_STRING(_clv)                          _clv.value.value_cp


#ifdef ADDR_TYPE_UINT8
#define DHCAC_SET_STATE(_dhcac, _state)                     (_dhcac).state = (_state)
#define DHCAC_SET_TYPE(_dhcac, _type)                       (_dhcac).type = (_type)
#elif defined(ADDR_TYPE_UINT16)
#define DHCAC_SET_STATE(_dhcac, _state)                     (_dhcac).type = ((_dhcac).type & 0x7FFF) | ((_state) << 15)
#define DHCAC_SET_TYPE(_dhcac, _type)                       (_dhcac).type = ((_dhcac).type & 0x8000) | ((_type) & 0x7FFF)
#endif
#define DHCAC_SET_LEN(_dhcac, _len)                         (_dhcac).len = (_len)
#define DHCAC_SET_CRC(_dhcac, _crc)                         (_dhcac).crc = (_crc)
#define DHCAC_SET_OPTION(_dhcac, _option)                   (_dhcac).option = (_option)
#define DHCAC_SET_UPDATE_TIME(_dhcac, time)                 (_dhcac).updateTime = (time)
#define DHCAC_SET_JUMP_COUNT(_dhcac, _jc)                   (_dhcac).jumpCount = (_jc)
#define DHCAC_SET_CLIENT_ADDR(_dhcac, _addr)                (_dhcac).clientAddr = (_addr)
#define DHCAC_SET_UPDATE_ADDR(_dhcac, _addr)                (_dhcac).updateAddr = (_addr)
#define DHCAC_SET_SERVER_ADDR(_dhcac, _addr)                (_dhcac).serverAddr = (_addr)
#define DHCAC_SET_NEXT_RELAY_ADDR(_dhcac, _addr)            (_dhcac).nextRelayAddr = (_addr)
#define DHCAC_SET_CLIENT_HARD_ADDR(_dhcac, _addr)           (_dhcac).clientHardAddr = (_addr)

#ifdef ADDR_TYPE_UINT8
#define DHCAC_GET_STATE(_dhcac)                             (_dhcac).state
#define DHCAC_GET_TYPE(_dhcac)                              (_dhcac).type
#elif defined(ADDR_TYPE_UINT16)
#define DHCAC_GET_STATE(_dhcac)                             (_dhcac.type | 0x8000)?1:0
#define DHCAC_GET_TYPE(_dhcac)                              (_dhcac.type & 0x7FFF)
#endif
#define DHCAC_GET_LEN(_dhcac)                               (_dhcac).len
#define DHCAC_GET_CRC(_dhcac)                               (_dhcac).crc
#define DHCAC_GET_OPTION(_dhcac)                            (_dhcac).option
#define DHCAC_GET_UPDATE_TIME(_dhcac)                       (_dhcac).updateTime
#define DHCAC_GET_JUMP_COUNT(_dhcac)                        (_dhcac).jumpCount
#define DHCAC_GET_CLIENT_ADDR(_dhcac)                       (_dhcac).clientAddr
#define DHCAC_GET_UPDATE_ADDR(_dhcac)                       (_dhcac).updateAddr
#define DHCAC_GET_SERVER_ADDR(_dhcac)                       (_dhcac).serverAddr
#define DHCAC_GET_NEXT_RELAY_ADDR(_dhcac)                   (_dhcac).nextRelayAddr
#define DHCAC_GET_CLIENT_HARD_ADDR(_dhcac)                  (_dhcac).clientHardAddr

#define DHCAC_STATE_SERVER                                  (1)
#define DHCAC_STATE_CLIENT                                  (0)

#define DHCAC_OPTION_REG                                    (1)
#define DHCAC_OPTION_REG_ACK                                (2)
#define DHCAC_OPTION_ALLOC                                  (3)
#define DHCAC_OPTION_ALLOC_ACK                              (4)

#define DHCAC_CHECK_ADDR(addr)                              ((addr) >= (startAddr) && \
                                                             (addr) <= (endAddr))
#define DHCAC_CHECK_TYPE(type)                              ((type) == 0)



#define DHCAC_GetAddrStatus(n)                              (addrStatus[(n) >> 5] & ((uint32_t)1 << ((n) & 0x1f)))
#define DHCAC_updateAddrStatus(n, s)                        do { \
                                                                addrStatus[(n) >> 5] &= ~((uint32_t)(1) << ((n) & 0x1f)); \
                                                                addrStatus[(n) >> 5] |= ((uint32_t)(s) << ((n) & 0x1f)); \
                                                            } while (0)
#define DHCAC_setDynamicTime(n, t)                          do { \
                                                                    dynamicAddrConfig[n].updateTime = t; \
                                                            } while (0)
#define DHCAC_setDynamicAddr(n, h, s, t)                    do { \
                                                                dynamicAddrConfig[n].hardAddr   = h; \
                                                                dynamicAddrConfig[n].softAddr   = s; \
                                                                dynamicAddrConfig[n].updateTime = t; \
                                                            } while (0)
#define DHCAC_clearDynamicAddr(n, h, s, t)                  do { \
                                                                dynamicAddrConfig[n].hardAddr   = 0; \
                                                                dynamicAddrConfig[n].softAddr   = 0; \
                                                                dynamicAddrConfig[n].updateTime = 0; \
                                                            } while (0)
#define DHCAC_setStaticAddress(n, h, s)                     do { \
                                                                staticAddrConfig[n].hardAddr = h; \
                                                                staticAddrConfig[n].softAddr = s; \
                                                                DHCAC_updateAddrStatus(n, SET); \
                                                            } while (0)
#define DHCAC_clearStaticAddress(n)                         do { \
                                                                staticAddrConfig[n].hardAddr = 0; \
                                                                staticAddrConfig[n].softAddr = 0; \
                                                                DHCAC_updateAddrStatus(n, RESET); \
                                                            } while (0)

typedef struct addrConfig {
    uint32_t    hardAddr;
    ADDR_T      softAddr;
    uint16_t    updateTime;
}addrConfig_t;

typedef struct {
#if ADDR_TYPE == uint8_t
    uint8_t     state;
    uint8_t     type;
    uint8_t     len;
    uint8_t     crc;
    uint8_t     option;
    uint8_t     jumpCount;
    uint16_t    updateTime;
    uint8_t     clientAddr;
    uint8_t     updateAddr;
    uint8_t     serverAddr;
    uint8_t     nextRelayAddr;
    uint32_t    clientHardAddr;
#elif ADDR_TYPE == uint16_t
    uint16_t    type;
    uint8_t     len;
    uint8_t     crc;
    uint8_t     option;
    uint8_t     jumpCount;
    uint16_t    back1;
    uint16_t    clientAddr;
    uint16_t    updateAddr;
    uint16_t    serverAddr;
    uint16_t    nextRelayAddr;
    uint32_t    clientHardAddr;
#else
#error "error"
#endif
}DHCAC_t;


typedef enum {
    DHCAC_RET_OK                    = 0,
    DHCAC_RET_FAILD                 = 1,
    DHCAC_RET_ERROR_STATE           = 2,
    DHCAC_RET_ERROR_TYPE            = 3,
    DHCAC_RET_ERROR_CRC             = 4,
    DHCAC_RET_ERROR_CLIENT_ADDR     = 5,
    DHCAC_RET_ERROR_SERVER_ADDR     = 6,
}DHCAC_RET_CODE;

typedef struct {
    uint8_t         code;
    uint8_t         len;
    union {
        uint32_t    value_i;
        float       value_f;
        char *      value_cp;
    }value;
}CLV_t;
void DHCAC_updateTime(void);
DHCAC_RET_CODE DHCAC_serverAllocAddressForClient(DHCAC_t * dhcac);

#endif //DHCAC_CHCAC_H
