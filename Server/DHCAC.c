//
// Created by 李玉航 on 2018/9/5.
//

#include "DHCAC.h"
#include "stdio.h"




static uint32_t         addrStatus[ADDR_MAX];
static addrConfig_t     staticAddrConfig[STATIC_ADDR_CONFIG_MAX];
static addrConfig_t     dynamicAddrConfig[DYNAMIC_ADDR_CONFIG_MAX];
static ADDR_T           startAddr = 11;
static ADDR_T           endAddr = 111;


//  返回一个空闲地址
//  0xffff 表示不可用
ADDR_T DHCAC_getIdleAddr(void) {
    uint8_t i, j;
    uint32_t tmp = 1;

    for (i = 0;i < ADDR_MAX;i++) {
        if (addrStatus[i] != 0xFFFFFFFF) {
            for (j = 0;j < 32;j++) {
                if ((addrStatus[i] & tmp) == 0) {
                    return ((ADDR_T)i << 5) | (ADDR_T)j;
                }
                tmp <<= 1;
            }
        }
    }
    return (ADDR_T)0xffff;
}

void printChar(char ch) {
    if ((ch >= ' ') && (ch <= '~')) {
        printf("%c", ch);
    } else {
        printf(".");
    }
}
void printHex(char ch) {
    printf("%02x ", ch);
}
void sendHex(char * data, uint16_t len) {
    uint16_t i, base = 0;

    while (len >= 8) {
        for (i = 0;i < 8;i++) {
            printHex(data[base + i]);
        }
        printf("\t|\t");
        for (i = 0;i < 8;i++) {
            printChar(data[base + i]);
        }
        printf("\n");
        len -= 8;
        base += 8;
    }
}

// 获取一个可用的地址
// 入参： 地址池的开始地址和结束地址（不包括结束地址）
ADDR_T DHCAC_getIdleAddress(ADDR_T sAddr, ADDR_T eAddr) {
    ADDR_T i;


    for (i = sAddr;i < eAddr;i++) {
        if (DHCAC_GetAddrStatus(i) == 0) {  // 查询地址是否使用
            return i;
        }
    }

    return ADDR_NULL;
}

ADDR_T DHCAC_getSaveDynamicAddr(uint32_t hAddr) {
    uint16_t i = 0;

    for (i = startAddr;i < endAddr;i++) {
        if (DHCAC_GetAddrStatus(i) && (hAddr == dynamicAddrConfig[i].hardAddr)) {
            return dynamicAddrConfig[i].softAddr;
        }
    }

    return 0;
}
void DHCAC_updateStaticTime(void) {
    uint8_t i = 0;
    printf("static:\r\n");
    for (i = 0;i < STATIC_ADDR_CONFIG_MAX;i++) {
        if (staticAddrConfig[i].updateTime > 0) {
            staticAddrConfig[i].updateTime--;
            printf("%d %d\r\n", i, staticAddrConfig[i].updateTime);
        }
    }
}

void DHCAC_updateDynamicTime(void) {
    uint16_t i = 0;

    printf("dynamic:\r\n");
    for (i = startAddr;i < endAddr;i++) {
        if (DHCAC_GetAddrStatus(i)) {
            dynamicAddrConfig[i].updateTime--;
            printf("%d %d\r\n", i, dynamicAddrConfig[i].updateTime);
            if (dynamicAddrConfig[i].updateTime == 0) { // 如果减到 0 ，则剔除设备
                DHCAC_updateAddrStatus(i, RESET);
                printf("%d disconnect\r\n", i);
            }
        }
    }
}

void DHCAC_updateTime(void) {
    DHCAC_updateStaticTime();
    DHCAC_updateDynamicTime();
}


//  回复客户端
DHCAC_RET_CODE DHCAC_serverAllocAddressForClient(DHCAC_t * dhcac) {
    uint8_t status = 0;
    ADDR_T addr = 0;
    uint32_t hardAddr;

    if (DHCAC_GET_STATE(*dhcac) != DHCAC_STATE_SERVER) {
        return DHCAC_RET_ERROR_STATE;
    }

    if (!DHCAC_CHECK_TYPE(dhcac->type)) {
        return DHCAC_RET_ERROR_TYPE;
    }

    if (0) {  // 校验
        return DHCAC_RET_ERROR_CRC;
    }

    if ((DHCAC_GET_SERVER_ADDR(*dhcac) != CURRENT_SERVER_ADDR) &&
         (DHCAC_GET_SERVER_ADDR(*dhcac) != BOARDCAST_ADDR)) {
        return DHCAC_RET_ERROR_SERVER_ADDR;
    }

    if (DHCAC_GET_OPTION(*dhcac) == DHCAC_OPTION_REG) {  // 申请地址;
        if (DHCAC_getIdleAddress(startAddr, endAddr) != ADDR_NULL) {  // 存在地址
            DHCAC_SET_OPTION(*dhcac, DHCAC_OPTION_REG_ACK);
            DHCAC_SET_SERVER_ADDR(*dhcac, CURRENT_SERVER_ADDR);
        } else {
            return DHCAC_RET_ERROR_CLIENT_ADDR;
        }
    } else if (DHCAC_GET_OPTION(*dhcac) == DHCAC_OPTION_ALLOC) { // 注册地址
        addr = DHCAC_GET_CLIENT_ADDR(*dhcac);
        // 获取硬件地址
        hardAddr = DHCAC_GET_CLIENT_HARD_ADDR(*dhcac);

        if (addr == 0) {  // 没有地址，重新申请地址
            addr = DHCAC_getSaveDynamicAddr(hardAddr);
            if (addr == 0) {  // 没有可用的地址
                addr = DHCAC_getIdleAddress(startAddr, endAddr);
             }
        }

        // 更新配置信息
        DHCAC_SET_OPTION(*dhcac, DHCAC_OPTION_ALLOC_ACK);
        DHCAC_SET_UPDATE_ADDR(*dhcac, addr);
        DHCAC_SET_UPDATE_TIME(*dhcac, UPDATE_TIME);
        DHCAC_updateAddrStatus(addr, SET);
        // 绑定硬件地址和软件分配的地址
        DHCAC_setDynamicAddr(addr, hardAddr, addr, UPDATE_TIME);
        printf("addr:%d bits:%x %d\r\n", addr, addrStatus[(addr) >> 5], UPDATE_TIME);

    }
    return DHCAC_RET_OK;
}

void DHCAC_setStartAddress(ADDR_T addr) {
    startAddr = addr;
}
void DHCAC_setEndAddress(ADDR_T addr) {
    endAddr = addr;
}

