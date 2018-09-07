#include "stdio.h"
#include "DHCAC.h"
#include "timer.h"

int main(int argc, char **argv) {
    uint8_t ret = 0;
    uint16_t i = 0;
    DHCAC_t dhcac;

    timerAddFunction(DHCAC_updateTime);


    DHCAC_clientInitStruct(&dhcac);
    printf("init success\r\n");
    ret = DHCAC_clientRegAddrForServer(&dhcac);
    printf("ret = :%d\r\n", ret);

    while (1) {
        DHCAC_checkUpdateTime(&dhcac);
        timerCycle();
    }
    return 0;
}