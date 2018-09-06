//
// Created by 李玉航 on 2018/9/6.
//

#ifndef DHCAC_QUEUE_H
#define DHCAC_QUEUE_H


#define QUEUE_DATA_LEN          (50)

#include "config.h"
#include "types.h"


typedef struct {
    char data[QUEUE_DATA_LEN];
    uint8_t dataLen;
}queue_t;




#endif //DHCAC_QUEUE_H
