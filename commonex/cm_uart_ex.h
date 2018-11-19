//
// Created by v on 18-6-21.
//

#ifndef SRC_CM_UART_EX_H
#define SRC_CM_UART_EX_H

#include "cm/cm_uart.h"

class CMUart {
public:
    CMUart();
    ~CMUart();

    int init(const char *dev, int speed,int flow_ctrl,int databits,int stopbits,int parity);

protected:
    int recv(char *data, int len);

    int send(char *data, int len);

private:
    int        iFD;
};

#endif //SRC_CM_UART_EX_H
