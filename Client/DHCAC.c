//
// Created by 李玉航 on 2018/9/5.
//

#include <afxres.h>
#include "DHCAC.h"
#include "stdio.h"
#include "windows.h"
#define AGAIN_CONNECT_MAX           3
#define AGAIN_SLEEP_DELAY           1000 // ms

static DHCAC_t currentDhcac;
static uint8_t regFlag = 0;
static uint16_t currentUpdateTime;

// 更新时间，每次减 1
void DHCAC_updateTime(void) {
    if (currentDhcac.updateTime > 0) {
        currentDhcac.updateTime--;
    }
    printf("%d\r\n", currentDhcac.updateTime);
}

// 打印字符
void printChar(uint8_t ch) {
    if ((ch >= ' ') && (ch <= '~')) {
        printf("%c", ch);
    } else {
        printf(".");
    }
}
void printHex(uint8_t ch) {
    printf("%02x ", ch);
}
void sendHex(uint8_t * data, uint16_t len) {
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

// 初始化结构体
void DHCAC_clientInitStruct(DHCAC_t * dhcac) {
    dhcac->state            = DHCAC_STATE_SERVER;
    dhcac->type             = 0;
    dhcac->len              = 0;
    dhcac->crc              = 0;
    dhcac->option           = DHCAC_OPTION_REG;
    dhcac->jumpCount        = 0;
    dhcac->updateTime       = 0;
    dhcac->clientAddr       = CLIENT_ADDR_NULL;
    dhcac->updateTime       = CLIENT_ADDR_NULL;
    dhcac->serverAddr       = BOARDCAST_ADDR;
    dhcac->nextRelayAddr    = 0;
    dhcac->clientHardAddr   = CURRENT_HARD_ADDR;
}
void DHCAC_clientAllotAddrForServer(DHCAC_t * dhcac, ADDR_T serverAddr) {
    dhcac->option           = DHCAC_OPTION_ALLOC;
    dhcac->serverAddr       = serverAddr;
}

void DHCAC_updateInfo(DHCAC_t * dhcac) {
    currentUpdateTime               = dhcac->updateTime;
    currentDhcac.updateTime         = dhcac->updateTime;
    currentDhcac.clientAddr         = dhcac->updateAddr;
    regFlag = 0;
}
DHCAC_RET_CODE DHCAC_ackForServer(DHCAC_t * dhcac) {
    if (DHCAC_GET_STATE(*dhcac) != DHCAC_STATE_CLIENT) {
        return DHCAC_RET_ERROR_STATE;
    }

    if (!DHCAC_CHECK_TYPE(dhcac->type)) {
        return DHCAC_RET_ERROR_TYPE;
    }

    if (0) {  // 校验
        return DHCAC_RET_ERROR_CRC;
    }

    if (DHCAC_GET_CLIENT_HARD_ADDR(*dhcac) != CURRENT_HARD_ADDR) {
        return DHCAC_RET_ERROR_CLIENT_ADDR;
    }

    if (DHCAC_GET_OPTION(*dhcac) == DHCAC_OPTION_REG_ACK) {
        dhcac->option = DHCAC_OPTION_ALLOC;
    } else if (DHCAC_GET_OPTION(*dhcac) == DHCAC_OPTION_ALLOC_ACK) {
        DHCAC_updateInfo(dhcac);
    } else {
        return DHCAC_RET_ERROR_OPTION;
    }

    return DHCAC_RET_OK;
}
void clientRegAck(DHCAC_t * dhcac) {
    dhcac->state = DHCAC_STATE_CLIENT;
    dhcac->option = DHCAC_OPTION_REG_ACK;
}
void clientAllotAck(DHCAC_t * dhcac) {
    dhcac->state = DHCAC_STATE_CLIENT;
    dhcac->option = DHCAC_OPTION_ALLOC_ACK;
    dhcac->updateTime = 10;
    dhcac->updateAddr = 10;
}
DHCAC_RET_CODE DHCAC_clientRegAddr(DHCAC_t * dhcac) {
    DHCAC_RET_CODE code = DHCAC_RET_FAILD;
    uint8_t faildCount = 0;

    while (code != DHCAC_RET_OK) {
        // 发送申请
        sendHex((uint8_t *)(dhcac), sizeof(DHCAC_t));
        // 申请回复
        clientRegAck(dhcac);
        code = DHCAC_ackForServer(dhcac);


        faildCount++;
        if (faildCount > AGAIN_CONNECT_MAX) {
            return DHCAC_RET_ERROR_TIME_OUT;
        }
        Sleep(AGAIN_SLEEP_DELAY);
    }

    return code;
}
DHCAC_RET_CODE DHCAC_clientAllotAddr(DHCAC_t * dhcac) {
    DHCAC_RET_CODE code = DHCAC_RET_FAILD;
    ADDR_T sAddr;
    uint8_t faildCount = 0;

    while (code != DHCAC_RET_OK) {
        // 发出注册
        sAddr = DHCAC_GET_SERVER_ADDR(*dhcac);
        DHCAC_clientAllotAddrForServer(dhcac, sAddr);
        sendHex((uint8_t *)(dhcac), sizeof(DHCAC_t));
        // 注册回复
        clientAllotAck(dhcac);
        code = DHCAC_ackForServer(dhcac);


        faildCount++;
        if (faildCount > AGAIN_CONNECT_MAX) {
            return DHCAC_RET_ERROR_TIME_OUT;
        }
        Sleep(AGAIN_SLEEP_DELAY);
    }
    return code;
}
DHCAC_RET_CODE DHCAC_clientRegAddrForServer(DHCAC_t * dhcac) {
    DHCAC_RET_CODE code = DHCAC_RET_FAILD;

    code = DHCAC_clientRegAddr(dhcac);
    if (code != DHCAC_RET_OK) {
        return code;
    }
    code = DHCAC_clientAllotAddr(dhcac);
    if (code != DHCAC_RET_OK) {
        return code;
    }

    return DHCAC_RET_OK;
}

// 检查更新时间
DHCAC_RET_CODE DHCAC_checkUpdateTime(DHCAC_t * dhcac) {
    DHCAC_RET_CODE code = DHCAC_RET_OK;
    if (currentDhcac.updateTime <= currentUpdateTime / 4) {  // 广播，向所有主机发送地址申请
        printf("boardcast register address\r\n");
        DHCAC_clientInitStruct(dhcac);
        code = DHCAC_clientRegAddrForServer(dhcac);
        if (code == DHCAC_RET_ERROR_TIME_OUT) {
            Sleep(5000);
        }
    }
    if (regFlag == 0) {
        if (currentDhcac.updateTime <= currentUpdateTime / 2) {  // 向上次连接的主机发送地址申请
            printf("reset register address\r\n");
            dhcac->updateAddr = 0;
            dhcac->option = DHCAC_OPTION_ALLOC;
            dhcac->state = DHCAC_STATE_SERVER;
            code = DHCAC_clientAllotAddr(dhcac);
            regFlag = 1;
        }
    }

    return code;
}