#include "stdio.h"
#include "DHCAC.h"
#include "timer.h"

int main(int argc, char **argv) {
    uint8_t ret = 0;
    uint16_t i = 0;
    DHCAC_t dhcac = {
            .state              = DHCAC_STATE_SERVER,
            .option             = DHCAC_OPTION_ALLOC,
            .clientHardAddr     = 0x11111111,
            .serverAddr         = CURRENT_SERVER_ADDR,
    };


    timerAddFunction(DHCAC_updateTime);


    ret = DHCAC_serverAllocAddressForClient(&dhcac);
    printf("%d: %d %d\n", ret, dhcac.state, dhcac.option);
    printf("client addr:%d %d\n", dhcac.updateAddr, dhcac.updateTime);
    dhcac.option = DHCAC_OPTION_ALLOC;
    ret = DHCAC_serverAllocAddressForClient(&dhcac);
    printf("%d: %d %d\n", ret, dhcac.state, dhcac.option);
    printf("client addr:%d %d\n", dhcac.updateAddr, dhcac.updateTime);
    dhcac.option = DHCAC_OPTION_ALLOC;
    ret = DHCAC_serverAllocAddressForClient(&dhcac);
    printf("%d: %d %d\n", ret, dhcac.state, dhcac.option);
    printf("client addr:%d %d\n", dhcac.updateAddr, dhcac.updateTime);
    dhcac.option = DHCAC_OPTION_ALLOC;
    ret = DHCAC_serverAllocAddressForClient(&dhcac);
    printf("%d: %d %d\n", ret, dhcac.state, dhcac.option);
    printf("client addr:%d %d\n", dhcac.updateAddr, dhcac.updateTime);
    while (1) {
        i++;
        if (i == (UPDATE_TIME / 2)) {
            i = 0;
            dhcac.option = DHCAC_OPTION_ALLOC;
            ret = DHCAC_serverAllocAddressForClient(&dhcac);
            printf("%d: %d %d\n", ret, dhcac.state, dhcac.option);
            printf("client addr:%d %d\n", dhcac.updateAddr, dhcac.updateTime);
        }
        timerCycle();

    }
    return 0;
}